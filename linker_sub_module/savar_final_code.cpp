//
//  main.cpp
//  tokenizer
//
//  Created by Savar Chaturvedi on 30/01/24.
//
 
#include <iostream>
#include <map>
#include <fstream>
#include <regex>
#include <set>
#include <vector>
#include <iomanip>
 
using namespace std;
 
map<string,int> symboltable;
int line_number=1, offset=1,prev_line_number=1;
int module_base=0;
string file_name="/Users/savarchaturvedi/Desktop/lab1_assign/input-1";
string temp;
const string delimeter=" \t\n";
int module_number=0;
int current_index=0;
bool is_new_module=true;
int prev_offset=1;
int curr_instr=0;
bool stop_further_execution=false;
map<int,int> module_table;
string delimiters = " \t\n";
int no_of_instruction_in_current_module=0;
bool pass_2_on=false;
int counter_inst=0;
vector<string> refrenced_variables;
bool flag=true;
string current_token;
map<string,int> symbol_module_mapper;
vector<string> warnings_in_module;
set<string> duplicate_symbols;
multimap<string, int> all_symbol_module_mapper;
set<int> used_variables_pass_2;
set<string> symbols_in_whole_pass2;
 
 
 
 
 
void initialize_global_variables();
int tokeniser(string);
void update_module_table();
void update_symbol_table(string,int);
int readInt(string);
bool symbol_checker(string);
string readSymbol(string);
void read_def_list(string);
void parse_1(string);
void read_user_list(string);
int read_instructions_list(string);
string read_marie(string);
void parse_2(string);
void update_cursor_location(string, int);
void update_cursor_line(string, int);
void parse_error(int);
void warning_print_checker(int);
void print_symbol_table(void);
 
 
 
int tokeniser(string inputstream)
{
    //cout<<"this is tokeniser function"<<endl;
    int end;
    while(current_index<inputstream.length())
    {
        end = inputstream.find_first_of(delimiters, current_index);
        if(end==-1)
        {
            end = inputstream.size();
        }
        if(end!=current_index)
        {
            return(end);
        }
        //there is a flag switch need to check that
        //flag=false;
        update_cursor_location(inputstream, end);
        //flag=true;
        current_index=end+1;
    }
    return(-1);
}
 
 
void update_module_table()
{
    //cout<<"this is update_modeule_table function"<<endl;
    if(module_table.find(module_number) == module_table.end())
    {
        module_table.insert(make_pair(module_number, no_of_instruction_in_current_module+module_base));
    }
}
 
void update_symbol_table(string s,int val)
{
    //cout<<"this is update_symbol_table"<<endl;
    if(symboltable.find(s)==symboltable.end())
    {
        int base_add=0;
        auto it = module_table.find(module_number);
        
        if (it != module_table.end()) {
            base_add= it->second;
        }
        symbol_module_mapper.insert(make_pair(s,module_number));
        
        symboltable.insert(make_pair(s,base_add+val));
        all_symbol_module_mapper.insert(make_pair(s, module_number));
    }
    else{
        duplicate_symbols.insert(s);
        all_symbol_module_mapper.insert(make_pair(s, module_number));
        string lo="Warning: Module " + std::to_string(module_number) + ": " + s + " redefinition ignored\n";
        warnings_in_module.push_back(lo);
    }
}
    
    
    
    int readInt(string input_stream)
    {
        //cout<<"this is readInt function"<<endl;
        int end=tokeniser(input_stream);
        
        if(current_index<input_stream.size())
        {
            
            string val = input_stream.substr(current_index, end-current_index);
            update_cursor_location(input_stream, end);
            
            
            std::regex digitPattern("^[0-9]+$");
            
            // Check if the entire string matches the pattern
            if (!std::regex_match(val, digitPattern)) {
                // If it doesn't match, call the parse_error function
                parse_error(0);
            }
            
            int actualInt = stoi(val);
            current_token = to_string(actualInt);
            current_index=end+1;
            return(actualInt);
        }
        else{
            if(is_new_module){
                stop_further_execution = true;
                return(-1);
            }
            parse_error(0);
        }
        return(-1);
    }
    
    bool symbol_checker(string val)
    {
        //cout<<"this is symbol_checker function"<<endl;
        if(!regex_match(val, regex("[a-zA-Z][a-zA-Z0-9]*")))
        {
            parse_error(1);
        }
        
        if(val.size()>16)
        {
            parse_error(3);
        }
        
        return(true);
        
    }
    string readSymbol(string inputstream)
    {
        //cout<<"this is readSymbol function"<<endl;
        int end=tokeniser(inputstream);
        if(current_index<inputstream.size())
        {
            
            string val = inputstream.substr(current_index, end-current_index);
            current_token = val;
            update_cursor_location(inputstream, end);
            
            bool is_valid_symbol=symbol_checker(val);
            current_index=end+1;
            return(val);
        }
        else{
            parse_error(1);
        }
        return("");//check here what needs to be printed;
    }
    bool marie_checker(string val)
    {
        //cout<<"this is marie_checker function"<<endl;
        regex validChars("[MARIE]");
        
        if (std::regex_match(val, validChars) and val.length()==1)
        {
            return(true);
        }
        parse_error(2);
        return(false);
    }
    
    string read_marie(string inputstream)
    {
        //cout<<"this is read_marie function"<<endl;
        int end=tokeniser(inputstream);
        if(current_index<inputstream.size())
        {
            string val = inputstream.substr(current_index, end-current_index);
            current_token=val;
            update_cursor_location(inputstream, end);
            bool ch=marie_checker(val);
            current_index=end+1;
            return(val);
        }
        else{
            parse_error(2);
            return("");
        }
    }
    
    void read_def_list(string inputstream)
    {
        //cout<<"this is read_def_list function"<<endl;
        if(!pass_2_on)
        {
            update_module_table();
            is_new_module = true;
            int no_of_defined_vars=readInt(inputstream);
            if(stop_further_execution){
                    return;
                }
            is_new_module = false;

            
            if (no_of_defined_vars > 16) {
                parse_error(4);
            }
            
            prev_offset+=current_token.size();
            
            for(int i=0;i<no_of_defined_vars;i++)
            {
                string s1=readSymbol(inputstream);
                prev_offset+=current_token.size();
                int i1=readInt(inputstream);
                prev_offset+=current_token.size();
                update_symbol_table(s1,i1);
                
            }
        }
        else
        {
            is_new_module = true;
            int no_of_defined_vars=readInt(inputstream);
            if(stop_further_execution){
                    return;
                }
            is_new_module = false;

            
            for(int i=0;i<no_of_defined_vars;i++)
            {
                string s1=readSymbol(inputstream);
                int i1=readInt(inputstream);
                //update_symbol_table(s1,i1);
                
            }
        }
        
    }
    
    
    
    
    void read_user_list(string input_sting)
    {
        if(stop_further_execution){
                return;
            }

        //cout<<"this is read_user_list function"<<endl;
        int no_of_referenced_variables=readInt(input_sting);
        if (no_of_referenced_variables > 16) {
            parse_error(5);
        }
        prev_offset+=current_token.size();
        for(int i=0;i<no_of_referenced_variables;i++)
        {
            string symb=readSymbol(input_sting);
            refrenced_variables.push_back(symb);
            prev_offset+=current_token.size();
        }
    }
    
    
    
    int read_instructions_list(string input_string)
    {
        if(stop_further_execution){
                return(0);
            }

        //cout<<"this is read_instructions_list function"<<endl;
        if(!pass_2_on)
        {
            int no_of_instructions=readInt(input_string);
            if(no_of_instructions > 512 - module_base){
                parse_error(6);
            }
            prev_offset+=current_token.size();
            
            for(int i=0;i<no_of_instructions;i++)
            {
                string instruction_type=read_marie(input_string);
                prev_offset+=current_token.size();
                
                int inst=readInt(input_string);
                prev_offset+=current_token.size();
            }
            return(no_of_instructions);
        }
        else
        {
            if(module_number==0)
            {
                cout<<"Memory Map"<<endl;
            }
            int no_of_instructions=readInt(input_string);
            
            
            for(int i=0;i<no_of_instructions;i++)
            {
                string instruction_type=read_marie(input_string);
                if(instruction_type=="R")
                {
                    int inst=readInt(input_string);
                    int opcode=inst/1000;
                    int operand=inst%1000;
                    //int op_out=module_table[module_number]+operand;
                    if(opcode >=10)
                    {
                        cout << setw(3) << setfill('0') << counter_inst << ": "<<9999<<" Error: " << "Illegal opcode; treated as 9999"<< std::endl;;
                    }
                    else if(operand >= no_of_instructions)
                    {
                        cout << setw(3) << setfill('0') << counter_inst;        // Formatting the key part
                        cout << ": " << setw(4) << setfill('0') << (opcode*1000)+module_table[module_number]<<" Error: " << " Relative address exceeds module size; relative zero used"<< std::endl;
                    }
                    else
                    {
                        int op_out=module_table[module_number]+operand;
                        cout << setw(3) << setfill('0') << counter_inst;        // Formatting the key part
                        cout << ": " << setw(4) << setfill('0') << (opcode*1000)+op_out<<std::endl;
                    }
                    
                }
                else if(instruction_type=="M")
                {
                    
                    int inst=readInt(input_string);
                    int opcode=inst/1000;
                    int operand=inst%1000;
                    if(opcode >=10)
                    {
                        cout << setw(3) << setfill('0') << counter_inst << ": "<<9999<<" Error: " << "Illegal opcode; treated as 9999"<< std::endl;;
                    }
                    else if (module_table.find(operand) == module_table.end()) {
                        cout << setw(3) << setfill('0') << counter_inst;        // Formatting the key part
                        cout << ": " << setw(4) << setfill('0') << (opcode*1000)<<" Error: " << "Illegal module operand ; treated as module=0"<< std::endl;
                        
                        
                    }
                    else{
                        int op_out=module_table[operand];
                        cout << setw(3) << setfill('0') << counter_inst;        // Formatting the key part
                        cout << ": " << setw(4) << setfill('0') << (opcode*1000)+op_out<<endl;
                        
                        
                        
                    }
                    
                }
                else if(instruction_type=="A")
                {
                    int inst=readInt(input_string);
                    int opcode=inst/1000;
                    int operand=inst%1000;
                    if(opcode >=10)
                    {
                        cout << setw(3) << setfill('0') << counter_inst << ": "<<9999<<" Error: " << "Illegal opcode; treated as 9999"<< std::endl;;
                    }
                    else if(operand>=512)
                    {
                        cout << setw(3) << setfill('0') << counter_inst;        // Formatting the key part
                        cout << ": " << setw(4) << setfill('0') << (opcode*1000)<<" Error: " << "Absolute address exceeds machine size; zero used"<<endl;  // Formatting the value part
                        
                        
                    }
                    else{
                        cout << setw(3) << setfill('0') << counter_inst;        // Formatting the key part
                        cout << ": " << setw(4) << setfill('0') << (opcode*1000)+operand<<endl;  // Formatting the value part
                        
                    }
                    
                }
                else if(instruction_type=="I")
                {
                    int inst=readInt(input_string);
                    int opcode=inst/1000;
                    int operand=inst%1000;
                    if(opcode >=10)
                    {
                        cout << setw(3) << setfill('0') << counter_inst << ": "<<9999<<" Error: " << "Illegal opcode; treated as 9999"<< std::endl;;
                    }
                    else if(operand>=900)
                    {
                        cout << setw(3) << setfill('0') << counter_inst;        // Formatting the key part
                        cout << ": " << setw(4) << setfill('0') << (opcode*1000)+999<<" Error: " << "Illegal immediate operand; treated as 999"<<endl;  // Formatting the value part
                    }
                    else
                    {
                        cout << setw(3) << setfill('0') << counter_inst;        // Formatting the key part
                        cout << ": " << setw(4) << setfill('0') << (opcode*1000)+operand<<endl;  // Formatting the value part
                        
                    }
                }
                else if(instruction_type=="E")
                {
                    int inst=readInt(input_string);
                    int opcode=inst/1000;
                    int operand=inst%1000;
                    //string ref_var=refrenced_variables[operand];
                    int abs_add_ref_var=0;
                    if(opcode >=10)
                    {
                        cout << setw(3) << setfill('0') << counter_inst << ": "<<9999<<" Error: " << "Illegal opcode; treated as 9999"<< std::endl;;
                    }
                    else if(operand >= refrenced_variables.size())
                    {
                        abs_add_ref_var=0;
                        cout << setw(3) << setfill('0') << counter_inst;        // Formatting the key part
                        cout << ": " << setw(4) << setfill('0') << (opcode * 1000) + abs_add_ref_var<<" Error: " << "External operand exceeds length of uselist; treated as relative=0"<<endl;  // Formatting the value part
                    }
                    //cout << setw(3) << setfill('0') << counter_inst << ": "<<(opcode*1000)+abs_add_ref_var<<endl;
                    else if(symboltable.find(refrenced_variables[operand]) != symboltable.end())
                    {
                        string ref_var=refrenced_variables[operand];
                        used_variables_pass_2.insert(operand);
                        symbols_in_whole_pass2.insert(refrenced_variables[operand]);
                        abs_add_ref_var=symboltable[ref_var];
                        
                        cout << setw(3) << setfill('0') << counter_inst;        // Formatting the key part
                        cout << ": " << setw(4) << setfill('0') <<(opcode*1000)+abs_add_ref_var<<endl;  // Formatting the value part
                        
                        
                        
                    }
                    else{
                        used_variables_pass_2.insert(operand);
                        symbols_in_whole_pass2.insert(refrenced_variables[operand]);
                        abs_add_ref_var=0;
                        cout << setw(3) << setfill('0') << counter_inst;        // Formatting the key part
                        cout << ": " << setw(4) << setfill('0') <<(opcode*1000)+abs_add_ref_var<<" Error: " << refrenced_variables[operand]
                        << " is not defined; zero used" << std::endl; // Formatting the value part
    
                    }
                    
                }
                counter_inst=counter_inst+1;
            }
            return(no_of_instructions);
            
        }
        
    }


    
    
    
    void parse_1(string input_string)
    {
        //cout<<"this is parse_1 function"<<endl;
        initialize_global_variables();
        
        while(current_index<input_string.size())
        {
            read_def_list(input_string);
            read_user_list(input_string);
            int no_of_instructions_in_module=read_instructions_list(input_string);
            //checking for warnings.
            
            warning_print_checker(no_of_instructions_in_module);
            vector<string>::iterator itr;
            for(itr = warnings_in_module.begin(); itr!=warnings_in_module.end();++itr){
                cout << itr->c_str();
            }
            warnings_in_module.clear();
            
            
            
            module_base=module_base+no_of_instructions_in_module;
            refrenced_variables.clear();
            module_number=module_number+1;
        }
        
    }
    void warning_print_checker(int no_of_instructions)
    {
        //cout<<"this is warning_print_checker function"<<endl;
        //THIS NEEDS TO BE CHANGED IF POSSIBLE.
        //iterate in symbol table and for each value of the symbols in it check the module no of those varibales is
        // same as present module number if so check if 5
        multimap<string, int>::iterator itr;
                for (itr = all_symbol_module_mapper.begin(); itr != all_symbol_module_mapper.end(); ++itr)
                {
                    int variable_location = symboltable.find(itr->first)->second;
                    if (itr->second == module_number && variable_location >= module_base+no_of_instructions) {
                        std::string l = "Warning: Module " + std::to_string(module_number) +
                        ": " + itr->first + "=" + std::to_string(symboltable[itr->first]-module_table[module_number]) +
                        " valid=[0.." + std::to_string(no_of_instructions - 1) + "] assume zero relative\n";
                        warnings_in_module.push_back(l);
                        symboltable.erase(itr->first.c_str());
                        symboltable.insert(make_pair(itr->first, module_base));
                    }
                }
        
    }
    
    void parse_2(string input_string)
    {
        //cout<<"this is parse_2 function"<<endl;
        initialize_global_variables();
        while(current_index<input_string.size())
        {
            read_def_list(input_string);
            read_user_list(input_string);
            
            read_instructions_list(input_string);
            
            for(int i=0;i<refrenced_variables.size();i++)
            {
                if(used_variables_pass_2.find(i)==used_variables_pass_2.end())
                {
                    std::cout << "Warning: Module " << module_number << ": uselist[" << i << "]=" << refrenced_variables[i] << " was not used\n";
 
                }
            }
            
            
            used_variables_pass_2.clear();
            refrenced_variables.clear();
            
            
            module_number=module_number+1;
            
        }
    }
 
    
    void print_symbol_table()
    {
        //cout<<"this is print_symbol_table function"<<endl;
        cout<<"Symbol Table"<<endl;
        for (const auto& pair : symboltable) {
            std::cout << pair.first << "=" << pair.second;
            if(duplicate_symbols.find(pair.first) != duplicate_symbols.end()){
                        cout << " Error: This variable is multiple times defined; first value used";
                    }
            cout << '\n';
        }
        cout << '\n';
        
 
        
    }
    int main(int argc, const char * argv[]) {
        //cout<<"this is main function"<<endl;
        file_name=argv[1];
        
        
        ifstream inputFile(argv[1]);
        
        // Check if the file is open
        if (!inputFile.is_open()) {
            std::cerr << "Error opening the file." << std::endl;
            return 1;
        }
        string temp_string="";
        string input_string="";
        while(getline(inputFile,temp_string))
        {
            input_string=input_string+temp_string+"\n";
            
            
        }
        
        
        parse_1(input_string);
        
        print_symbol_table();
        
        
        
        // making the referenced_varible cleared from previous pass
        refrenced_variables.clear();
        pass_2_on=true;
        
        
        parse_2(input_string);
        bool first_ch=true;
        for (auto it = symboltable.begin(); it != symboltable.end(); ++it)
        {
            string symb=it->first;
            if(symbols_in_whole_pass2.find(symb)==symbols_in_whole_pass2.end())
            {
                if(first_ch)
                {
                    cout<<endl;
                    first_ch=false;
                }
                
                std::cout << "Warning: Module " << symbol_module_mapper[it->first] << ": " << symb << " was defined but never used\n";
            }
        }
        
        
        
        return(1);
        
    }
    
    
    
    void parse_error(int errcode) {
        //cout<<"this is parse_error function"<<endl;
        static string errstr[] = {
            "NUM_EXPECTED",
            "SYM_EXPECTED",
            "MARIE_EXPECTED",
            "SYM_TOO_LONG",
            "TOO_MANY_DEF_IN_MODULE",
            "TOO_MANY_USE_IN_MODULE",
            "TOO_MANY_INSTR"};
        printf("Parse Error line %d offset %d: %s\n", prev_line_number, prev_offset, errstr[errcode].c_str());
        std::exit(EXIT_FAILURE);
    }
    
    
    
    
    void update_cursor_location(string inputstream, int end){
        //cout<<"this is update_cursor_location function"<<endl;
        
        
        prev_offset = offset;
        offset += end-current_index+1;
        if(end<inputstream.size()) {
            update_cursor_line(inputstream, end);
        }
    }
    
    void update_cursor_line(string inputstream, int end){
       // cout<<"this is update_cursor_line function"<<endl;
        prev_line_number = line_number;
        if(inputstream.at(end) == '\n'){
            line_number++;
            offset = 1;
        }
    }
    
    void initialize_global_variables(){
        //cout<<"this is initalize_global_variables function"<<endl;
        line_number = 1;
        offset = 1;
        prev_line_number = 1;
        prev_offset = 1;
        module_base = 0;
        module_number=0;
        current_index = 0;
        curr_instr = 0;
        is_new_module = true;
        stop_further_execution = false;
        no_of_instruction_in_current_module=0;
        counter_inst=0;
        
    }


