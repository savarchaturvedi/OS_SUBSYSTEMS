#include <iostream>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <deque>
#include <vector>
#include <climits>
//savar

using namespace std;

int number_of_frames = 16;
char algo = '\0';
string input_file;
string random_file;
const int MAX_VPAGES = 64;
unsigned long long inst_count;
unsigned long long ctx_switches;
unsigned long long process_exits;
unsigned long long cost;
bool o_flag = false;
bool p_flag = false;
bool f_flag = false;
bool s_flag = false;
int* random_values;
int ind;
int size_of_random_values_array;









struct VMA {
    int start_vpage;
    int end_vpage;
    bool write_protected;
    bool file_mapped;
};
struct pstats {
    unsigned long unmaps;
    unsigned long maps;
    unsigned long ins;
    unsigned long outs;
    unsigned long fins;
    unsigned long fouts;
    unsigned long zeros;
    unsigned long segv;
    unsigned long segprot;
};
struct frame_t
{
    int process_id=-1;
    int v_page=-1;
    int frame_number=-1;
    unsigned int age=0;
};

struct inst_pair
{
    char type;
    int value;
};

struct pte_t {
         unsigned int PRESENT:1=0;
         unsigned int REFERENCED:1=0;
         unsigned int MODIFIED:1=0;
         unsigned int WRITE_PROTECT:1=0;
         unsigned int PAGEDOUT:1=0;
         unsigned int FRAME_NO:7=0;
         unsigned int is_file_mapped:1=0;
} ;



class Process {
public:
    int process_id;
    std::vector<VMA> vmas;
    pte_t page_table[MAX_VPAGES];
    pstats process_stats;
    
    Process(int id) : process_id(id) {
        // Initialize page_table elements
        for (int i = 0; i < MAX_VPAGES; ++i) {
            page_table[i].PRESENT = 0;
            page_table[i].REFERENCED = 0;
            page_table[i].MODIFIED = 0;
            page_table[i].WRITE_PROTECT = 0;
            page_table[i].PAGEDOUT = 0;
            page_table[i].FRAME_NO = 0;
        }
        //process_stats = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    }
    // Method to print information about the process
};
void read_random_num_file(string rf){
    ifstream input_file(rf);
    string temp;
    int index=0;
    getline(input_file, temp);
    size_of_random_values_array=stoi(temp);
    random_values=new int[size_of_random_values_array];
    while(getline(input_file, temp))
    {
        if(index==size_of_random_values_array)
        {
            break;
        }
        random_values[index] = stoi(temp);
        index++;
    }
}
int get_random_values(int number_of_frames)
{
    ind=ind%size_of_random_values_array;
    int g=random_values[ind];
    int return_val=g%number_of_frames;
    ind=ind+1;
    return(return_val);
}
std::unordered_map<int, Process*> processTable;
std::deque<inst_pair> instruction_queue;
frame_t* frame_table;
Process* current_running_process=nullptr;
pte_t* current_page_table =nullptr;
std::deque<frame_t*> free_frames;

void printStats(int pid,pstats* stats) {
    printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n",
            pid,
            stats->unmaps, stats->maps, stats->ins, stats->outs,
            stats->fins, stats->fouts, stats->zeros,
            stats->segv, stats->segprot);
}



class Pager {
public:
    virtual frame_t* select_victim_frame() = 0;// Pure virtual function
    virtual void reset_age(int frame_number)=0;
};

class FIFO_Pager : public Pager {
private:
    int hand = 0;
    
public:
    void reset_age(int frame_number)
    {
        return;
    }
    // Implement the select_victim_frame() method
    frame_t* select_victim_frame() {
        hand=(hand)%(number_of_frames);
        frame_t* frame_return=&frame_table[hand];
        hand=hand+1;
        return(frame_return);
    }
};


class CLOCK_PAGER :public Pager{
private :
    int hand=0;
    
public:
    void reset_age(int frame_number)
    {
        return;
    }
    frame_t* select_victim_frame() {
        hand=hand%number_of_frames;
        int temp=hand;
        int c=0;
        while(temp!=hand || c==0)
        {
            
            
            if(processTable[frame_table[temp].process_id]->page_table[frame_table[temp].v_page].REFERENCED==1)
            {
                processTable[frame_table[temp].process_id]->page_table[frame_table[temp].v_page].REFERENCED=0;
                
            }
            else
            {
                hand=temp+1;
                return(&frame_table[temp]);
            }
            temp=temp+1;
            temp=temp%number_of_frames;
            c=1;
                
        }
        hand=temp+1;
        return(&frame_table[temp]);
    }
};




class RANDOM_PAGER:public Pager
{
    private :
        int hand=0;
        
    public:
    void reset_age(int frame_number)
    {
        
        return;
    }
    frame_t* select_victim_frame()
    {
        int t=get_random_values(number_of_frames);
        return(&frame_table[t]);
        
    }
};

class NRU_PAGER: public Pager{
    private :
        int hand=0;
        int last=0;
    public :
    void reset_age(int frame_number)
    {
        return;
    }
        frame_t* select_victim_frame()
        {
            hand=hand%number_of_frames;
            int c=0;
            int temp=hand;
            int class_0=-1,class_1=-1,class_2=-1,class_3=-1;
            while(c==0 || temp!=hand)
            {
                c=1;
                Process* p=processTable[frame_table[temp].process_id];
                pte_t pte=p->page_table[frame_table[temp].v_page];
                int pred_class=2*pte.REFERENCED+pte.MODIFIED;
                if(pred_class==0)
                {
                    if(class_0==-1)
                    {
                        class_0=temp;
                    }
                }
                else if(pred_class==1)
                {
                    if(class_1==-1)
                    {
                        class_1=temp;
                    }
                }
                else if(pred_class==2)
                {
                    if(class_2==-1)
                    {
                        class_2=temp;
                    }
                }
                else if (pred_class==3)
                {
                    if(class_3==-1)
                    {
                        class_3=temp;
                    }
                }
                if(class_0!=-1)
                {
                    break;
                }
                temp=temp+1;
                temp=temp%number_of_frames;
            }
            int class_hand[] = {class_0, class_1, class_2, class_3};
            for (int i = 0; i < 4; ++i) {
                if (class_hand[i] != -1)
                {
                    temp = class_hand[i];
                    break;
                }
            }
            hand=temp+1;
            if(inst_count-1-last>=47)
            {
                last=inst_count;
                int j=0;
                while(j<number_of_frames)
                {
                    processTable[frame_table[j].process_id]->page_table[frame_table[j].v_page].REFERENCED=0;
                    j=j+1;
                }
            }
            return(&frame_table[temp]);
        }
    
};


class Ager: public Pager{
private:
    int hand =0;
    public :
    
    void reset_age(int frame_number)
    {
        frame_table[frame_number].age=0;
        return;
    }
    
    frame_t* select_victim_frame()
    {
        hand=hand%number_of_frames;
        int temp=hand;
        int c=0;
        unsigned int min_age_value=UINT_MAX;
        int min_age_hand=0;
        while(c==0 || temp!=hand)
        {
            
            c=1;
            frame_table[temp].age=frame_table[temp].age>>1;
            if(processTable[frame_table[temp].process_id]->page_table[frame_table[temp].v_page].REFERENCED==1)
            {
                frame_table[temp].age=(0x80000000 | frame_table[temp].age);
                processTable[frame_table[temp].process_id]->page_table[frame_table[temp].v_page].REFERENCED=0;
            }
            if(min_age_value==UINT_MAX || frame_table[temp].age<min_age_value)
            {
                min_age_hand=temp;
                min_age_value=frame_table[temp].age;
            }
            temp=temp+1;
            temp=temp%number_of_frames;
           
        }
        hand=min_age_hand+1;
        return(&frame_table[min_age_hand]);
    }
    
};

class workingSetPager: public Pager{
    private :
    int hand=0;
    public :
    
    void reset_age(int frame_number)
    {
        frame_table[frame_number].age=inst_count;
    }
    frame_t* select_victim_frame()
    {
        hand=hand%number_of_frames;
        int c=0;
        int temp=hand;
        int hand_consider=temp;
        unsigned int max_age=0;
        while(c==0 || temp!=hand)
        {
            c=1;
            if(processTable[frame_table[temp].process_id]->page_table[frame_table[temp].v_page].REFERENCED==0)
            {
                if(inst_count-frame_table[temp].age>=50)
                {
                    hand_consider=temp;
                    break;
                }
                else
                {
                    if(inst_count-frame_table[temp].age>max_age)
                    {
                        max_age=inst_count-frame_table[temp].age;
                        hand_consider=temp;
                    }
                }
            }
            else{
                frame_table[temp].age=inst_count;
                processTable[frame_table[temp].process_id]->page_table[frame_table[temp].v_page].REFERENCED=0;
                
            }
            temp=temp+1;
            temp=temp%number_of_frames;
            
        }
        hand=hand_consider+1;
        return(&frame_table[hand_consider]);
    }
    
    
};


    
    
Pager* pager=nullptr;
extern int optind;



void frame_table_printer()
{
    std::cout << "FT:";
    for (int i = 0; i < number_of_frames; ++i) {
        
        if(frame_table[i].process_id==-1)
        {
            cout<<" *";
        }
        else
        {
            std::cout<< " " << frame_table[i].process_id << ":" << frame_table[i].v_page;
            
        }
    }
    std::cout <<"\n";
}


void page_table_printer()
{
    for(int i=0;i<processTable.size();i++)
    {
        cout<<"PT["<<i<<"]:";
        
        pte_t*  table_to_print=processTable[i]->page_table;
        for(int j=0;j<MAX_VPAGES;++j)
        {
            if(table_to_print[j].PRESENT==0)
            {
                if(table_to_print[j].PAGEDOUT==1 && table_to_print[j].is_file_mapped==0)
                {
                    cout<<" #";
                }
                else
                {
                    cout<<" *";
                }
            }
            else
            {
                char R='-';
                char M='-';
                char S='-';
                if(table_to_print[j].REFERENCED==1)
                {
                    R='R';
                    
                }
                
                if(table_to_print[j].MODIFIED==1)
                {
                    M='M';
                }
                if(table_to_print[j].PAGEDOUT==1 && table_to_print[j].is_file_mapped==0)
                {
                    S='S';
                }
                if(table_to_print[j].REFERENCED==1)
                {
                    R='R';
                }
                cout<<" "<<j<<":"<<R<<M<<S;
            }
            
        }
        cout<<"\n";

    }
}




void read_input_file(string input_file)
{
    fstream file(input_file);
    if(!file.is_open())
    {
        cout<<" the file was not opended due to error"<<endl;
    }
    string line;
    int no_of_processes=0;
    int no_of_vmas=0;
    while(getline(file,line))
    {
        if(line.empty() || line.front()=='#')
        {
            continue;
        }
        else{
            
            no_of_processes=stoi(line);
            int k=0;
            while(k<no_of_processes)
            {
                processTable[k]=new Process(k);
                k=k+1;
            }
            int i=0;
            while(i<no_of_processes && getline(file,line))
            {
                
                if(line.front()=='#')
                {
                    continue;
                }
                else
                {
                    no_of_vmas=stoi(line);
                    
                    int j=0;
                    while(j<no_of_vmas && getline(file,line))
                    {
                        if(line.front()=='#')
                        {
                            continue;
                        }
                        else{
                            std::istringstream iss(line);
                            int value1, value2, value3, value4;
                            bool v3=false;
                            bool v4=false;
                            
                            if (iss >> value1 >> value2 >> value3 >> value4)
                            {
                                if(value3==1)
                                {
                                    v3=true;
                                }
                                if(value4==1)
                                {
                                    v4=true;
                                }
                                VMA newStruct;
                                newStruct.start_vpage=value1;
                                newStruct.end_vpage=value2;
                                newStruct.write_protected=v3;
                                newStruct.file_mapped=v4;
                                processTable[i]->vmas.push_back(newStruct);
                            }
                            
                            j=j+1;
                        }
                    }
                    //add to the process_id_pointer_mapper;
                    i=i+1;
                }
            }
            //check if the getline is getting executed.
            
            while(getline(file,line))
            {
                if(line.empty() || line.front()=='#')
                {
                    continue;
                }
                else{
                    std::istringstream iss(line);
                    char c1;
                    int v1;
                    if(iss>>c1>>v1)
                    {
                        instruction_queue.push_back({c1,v1});
                    }
                }
            }


            //this is where we shall parse the commands

        }
    }
    file.close();
    
}


bool check_part_of_vma(int vpage,pte_t *pte)
{
    vector<VMA> vmas1=current_running_process->vmas;
    for (const auto& vma : vmas1)
    {
        if (vpage >= vma.start_vpage && vpage <= vma.end_vpage)
        {
            if(vma.write_protected)
            {
                pte->WRITE_PROTECT=1;
            }
            else
            {
                pte->WRITE_PROTECT=0;
            }
            if(vma.file_mapped)
            {
                pte->is_file_mapped=1;
            }
            else
            {
                pte->is_file_mapped=0;
            }
            return true;
        }
    }
    return false;
    
}
void pager_init(char algo)
{
    if(algo=='c')
    {
        pager = new CLOCK_PAGER();
    }
    else if(algo=='f')
    {
        pager=new FIFO_Pager();
    }
    else if(algo=='r')
    {
        pager=new RANDOM_PAGER();
    }
    else if(algo=='e')
    {
        pager=new NRU_PAGER();
    }
    else if(algo=='a')
    {
        pager=new Ager();
    }
    else if(algo=='w')
    {
        pager=new workingSetPager();
    }
}

void free_frame_generator()
{
    for (int i = 0; i < number_of_frames; ++i) {
        frame_table[i].frame_number=i;
            free_frames.push_back(&frame_table[i]); // Assigning frame_number i
        }
}

frame_t *get_frame()
{
    if(free_frames.size()!=0)
    {
        frame_t *frame=free_frames.front();
        free_frames.pop_front();
        pager->reset_age(frame->frame_number);
        return(frame);
    }
    else{
        frame_t *frame=pager->select_victim_frame();
        pager->reset_age(frame->frame_number);
        return(frame);
    }
}


int main(int argc, char *argv[]) {

    
    int opt;
        int num_frames = 16;
        char algo = '\0';
        bool o_flag = false, f_flag = false, s_flag = false, p_flag = false;
        //string input_file, random_file;

        // Parse command line options
        while ((opt = getopt(argc, argv, "f:a:o:")) != -1) {
            switch (opt) {
                case 'f':
                    num_frames = atoi(optarg);
                    break;
                case 'a':
                    algo = optarg[0];
                    break;
                case 'o':
                    // Iterate through the characters of optarg to set flags
                    for (char *ptr_string = optarg; *ptr_string != '\0'; ptr_string++) {
                        switch (*ptr_string) {
                            case 'O':
                                o_flag = true;
                                break;
                            case 'F':
                                f_flag = true;
                                break;
                            case 'S':
                                s_flag = true;
                                break;
                            case 'P':
                                p_flag = true;
                                break;
                            default:
                                cerr << "Unknown option in -o: " << *ptr_string << endl;
                                exit(EXIT_FAILURE);
                        }
                    }
                    break;
                default:
                    cerr << "Usage: " << argv[0] << " -f<num_frames> -a<algo> [-o<options>] inputfile randomfile" << endl;
                    exit(EXIT_FAILURE);
            }
        }

        // Check if input and random file names are provided
        if (optind + 2 != argc) {
            cerr << "Usage: " << argv[0] << " -f<num_frames> -a<algo> [-o<options>] inputfile randomfile" << endl;
            exit(EXIT_FAILURE);
        }

        input_file = argv[optind];
        random_file = argv[optind + 1];
    
    
    
    //input_file=inp_file;
    pager_init(algo);
    number_of_frames=num_frames;
    
    
    
    
    //input_file="/Users/savarchaturvedi/Desktop/os_labb_3/os_labb_3/lab3_assign/in1";
    read_input_file(input_file);
    read_random_num_file(random_file);
    //pager = new FIFO_Pager();
    
    
    
    
    //frame table
    
    frame_table=new frame_t[number_of_frames];
    free_frame_generator();
    int inst_counter=0;
    
    
    
    
    
    while(!instruction_queue.empty())
    {
         
        //frame_table_printer();
        inst_count=inst_count+1;
        inst_pair instruction=instruction_queue.front();
        char current_instruction_type=instruction.type;
        int current_instruction_value=instruction.value;
        instruction_queue.pop_front();
        cout << inst_count-1 << ": ==> " << current_instruction_type << " " << current_instruction_value << endl;
        //cout<<inst_count-1<<": ==> "<<current_instruction_type<<" "<<current_instruction_value<<endl;
        if(current_instruction_type=='c')
        {
            ctx_switches=ctx_switches+1;
            cost=cost+130;
            current_running_process=processTable[current_instruction_value];
            current_page_table=current_running_process->page_table;
            
        }
        else if(current_instruction_type=='r' || current_instruction_type=='w')
        {
            cost=cost+1;
            pte_t *pte=&current_running_process->page_table[current_instruction_value];
            if(pte->PRESENT==0)
            {
                bool is_valid_vpage=check_part_of_vma(current_instruction_value,pte);
                if(is_valid_vpage)
                {
                    
                    frame_t *newframe=get_frame();
                    
                    int old_process_id=newframe->process_id;
                    if(old_process_id==-1)
                    {
                        //from free_queue
                        current_running_process->page_table[current_instruction_value].FRAME_NO=newframe->frame_number;
                        //mapping_operation
                        newframe->process_id=current_running_process->process_id;
                        newframe->v_page=current_instruction_value;
                        
                        if(current_running_process->page_table[current_instruction_value].PAGEDOUT==1 || current_running_process->page_table[current_instruction_value].is_file_mapped==1)
                        {
                            if(current_running_process->page_table[current_instruction_value].is_file_mapped==1)
                            {
                                current_running_process->process_stats.fins=current_running_process->process_stats.fins+1;
                                cost=cost+2350;
                                cout << " FIN" << endl;
                            }
                            else{
                                cost=cost+3200;
                                current_running_process->process_stats.ins=current_running_process->process_stats.ins+1;
                                cout << " IN" << endl;

                            }
                        }
                        else
                        {
                            current_running_process->process_stats.zeros=current_running_process->process_stats.zeros+1;
                            cost=cost+150;
                            cout << " ZERO" << endl;
                            
                        }
                        
                    }
                    else
                    {
                        pte_t *pte_old=&processTable[old_process_id]->page_table[newframe->v_page];
                        processTable[old_process_id]->process_stats.unmaps=processTable[old_process_id]->process_stats.unmaps+1;
                        cost=cost+410;
                        //current_running_process->process_stats.unmaps=current_running_process->process_stats.unmaps+1;
                        //cout << " UNMAP " << newframe->process_id << ":" << <newframe->v_page << endl;
                        //cout<<" UNMAP "<<newframe->process_id<<":"<<newframe->v_page<<endl;
                        cout << " UNMAP " << newframe->process_id << ":" << newframe->v_page << endl;

                        if(pte_old->MODIFIED)
                        {
                            pte_old->PAGEDOUT=1;
                            if(pte_old->is_file_mapped)
                            {
                                processTable[old_process_id]->process_stats.fouts=processTable[old_process_id]->process_stats.fouts+1;
                                cost=cost+2800;
                                //current_running_process->process_stats.fouts=current_running_process->process_stats.fouts+1;
                                cout << " FOUT" << endl;

                            }
                            else
                            {
                                cost=cost+2750;
                                processTable[old_process_id]->process_stats.outs=processTable[old_process_id]->process_stats.outs+1;
                                //current_running_process->process_stats.outs=current_running_process->process_stats.outs+1;
                                cout << " OUT" << endl;
                            }
                        }
                        
                        pte_old->FRAME_NO=0;
                        pte_old->MODIFIED=0;
                        pte_old->PRESENT=0;
                        pte_old->REFERENCED=0;

                        
                        
                        current_running_process->page_table[current_instruction_value].FRAME_NO=newframe->frame_number;
                        if(current_running_process->page_table[current_instruction_value].PAGEDOUT==1 || current_running_process->page_table[current_instruction_value].is_file_mapped==1)
                        {
                            if(current_running_process->page_table[current_instruction_value].is_file_mapped==1)
                            {
                                current_running_process->process_stats.fins=current_running_process->process_stats.fins+1;
                                cost=cost+2350;
                                cout << " FIN" << endl;
                            }
                            else{
                                cost=cost+3200;
                                current_running_process->process_stats.ins=current_running_process->process_stats.ins+1;
                                cout << " IN" << endl;

                            }
                        }
                        else
                        {
                            current_running_process->process_stats.zeros=current_running_process->process_stats.zeros+1;
                            cost=cost+150;
                            cout << " ZERO" << endl;

                            
                        }
                        newframe->process_id=current_running_process->process_id;
                        newframe->v_page=current_instruction_value;
                        
                        
                      //this is the frame used by some old processs
                        
                    }
                    
                    pte->PRESENT=1;
                    
                    if(current_instruction_type=='r')
                    {
                        current_running_process->page_table[current_instruction_value].REFERENCED=1;
                        current_running_process->process_stats.maps=current_running_process->process_stats.maps+1;
                        cost=cost+350;
                        cout << " MAP " << newframe->frame_number << endl;
                    }
                    else
                    {
                        current_running_process->process_stats.maps=current_running_process->process_stats.maps+1;
                        cost=cost+350;
                        cout << " MAP " << newframe->frame_number << endl;
                        if(current_running_process->page_table[current_instruction_value].WRITE_PROTECT)
                        {
                            current_running_process->process_stats.segprot=current_running_process->process_stats.segprot+1;
                            cost=cost+410;
                            cout << " SEGPROT" << endl;
                            current_running_process->page_table[current_instruction_value].REFERENCED=1;
                        }
                        else
                        {
                            current_running_process->page_table[current_instruction_value].REFERENCED=1;
                            current_running_process->page_table[current_instruction_value].MODIFIED=1;
                        }
                    }
                    /*current_running_process->process_stats.maps=current_running_process->process_stats.maps+1;
                    cost=cost+350;
                    cout << " MAP " << newframe->frame_number << endl;*/
                   

                 
                    
                }
                else
                {
                    current_running_process->process_stats.segv=current_running_process->process_stats.segv+1;
                    cost=cost+440;
                    cout << " SEGV" << endl;
                    continue;
                    
                }
                
            }
            else
            {
                //this is valid means already present in frame so just set the bits of reference.
                if(current_instruction_type=='r')
                {
                    current_running_process->page_table[current_instruction_value].REFERENCED=1;
                }
                else
                {
                    if(current_running_process->page_table[current_instruction_value].WRITE_PROTECT)
                    {
                        current_running_process->process_stats.segprot=current_running_process->process_stats.segprot+1;
                        cost=cost+410;
                        cout << " SEGPROT" << endl;
                        current_running_process->page_table[current_instruction_value].REFERENCED=1;
                    }
                    else
                    {
                        current_running_process->page_table[current_instruction_value].REFERENCED=1;
                        current_running_process->page_table[current_instruction_value].MODIFIED=1;
                    }
                }
            }
        
        }
        else
        {
            //this is where we need to implement command e;
            process_exits=process_exits+1;
            cost=cost+1230;
            cout << "EXIT current process " <<current_running_process->process_id<< endl;
            //cout<<"EXIT current process "<<current_running_process->process_id<<"\n";
            Process* current_process_to_exit=processTable[current_instruction_value];
            
            for(int pag=0;pag<MAX_VPAGES;pag++)
            {
                if(current_process_to_exit->page_table[pag].PRESENT==1)
                {
                    current_process_to_exit->process_stats.unmaps=current_process_to_exit->process_stats.unmaps+1;
                    cost=cost+410;
                    cout << " UNMAP " <<current_process_to_exit->process_id << ":" << pag << endl;
                    
                    //cout<<" UNMAP "<<current_process_to_exit->process_id<<":"<<pag<<endl;
                    
                    if(current_process_to_exit->page_table[pag].MODIFIED and current_process_to_exit->page_table[pag].is_file_mapped)
                    {
                        current_process_to_exit->process_stats.fouts=current_process_to_exit->process_stats.fouts+1;
                        cost=cost+2800;
                        cout << " FOUT" << endl;
                    }
                    frame_table[current_process_to_exit->page_table[pag].FRAME_NO].process_id=-1;
                    frame_table[current_process_to_exit->page_table[pag].FRAME_NO].v_page=-1;
                    free_frames.push_back(&frame_table[current_process_to_exit->page_table[pag].FRAME_NO]);
                }
                current_process_to_exit->page_table[pag].PRESENT=0;
                current_process_to_exit->page_table[pag].PAGEDOUT=0;
                current_process_to_exit->page_table[pag].REFERENCED=0;
                current_process_to_exit->page_table[pag].MODIFIED=0;
                current_process_to_exit->page_table[pag].WRITE_PROTECT=0;
                

            }
            //current_process_to_exit=nullptr;
        }
        /*cout<<"after exececuting the insturction the frame table "<<endl;
        frame_table_printer();
        cout<<"***************************"<<endl;
        page_table_printer();*/
        
        inst_counter=inst_counter+1;

    }
    
    page_table_printer();
    frame_table_printer();
    processTable.size();
    for (int proces=0;proces<processTable.size();proces++) {
        printStats(processTable[proces]->process_id,&processTable[proces]->process_stats);
        
    }
    std::cout << "TOTALCOST " << inst_count << " " << ctx_switches << " " << process_exits << " " << cost << " " << sizeof(pte_t) << std::endl;
    


}














