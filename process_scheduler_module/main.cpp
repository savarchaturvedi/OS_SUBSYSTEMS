//
//  main.cpp
//  savar_try
//
//  Created by Savar Chaturvedi on 03/03/24.
//

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <map>
#include <vector>
#include <string>
#include <unistd.h>
#include <list>

using namespace std;
typedef enum { CREATED, READY , RUNNING, BLOCKED, EXITED } transition_state;

typedef enum { TRANS_TO_READY , TRANS_TO_RUNNING, TRANS_TO_BLOCKED, PREEMPTED_TO_READY , TRANS_TO_EXITED } transition;


class Process
{
public:
    int arrival_time;
    int time_on_cpu;
    int cpu_burst;
    int io_burst;
    int process_id;
    int static_priority;

    int dynamic_priority;
    int finishing_time;
    int time_spent_in_IO;
    int time_spent_ready_state;
    int time_left_on_cpu;
    int current_cpu_burst;
    int latest_event_timestamp;
    transition_state current_state;
    
    Process(int AT, int TC, int CB, int IO,int pid, int stat_prio)
    {
        this->arrival_time = AT;
        this->time_on_cpu=TC;
        this->cpu_burst=CB;
        this->io_burst=IO;
        this->process_id=pid;
        this->static_priority=stat_prio;
        this->dynamic_priority = this->static_priority-1;
        this->time_spent_in_IO=0;
        this->time_spent_ready_state=0;
        this->time_left_on_cpu=time_on_cpu;
        this->current_cpu_burst=-1;
        this->current_state = CREATED;
        this->latest_event_timestamp= this->arrival_time;
        
    }
};

class Event
{
public:
    Process* event_process; // Replace with Process* event_process
    int event_timestamp;
    int event_counter;
    transition event_transition;
    
    Event(int evnt_ts, Process* evnt_process, int evnt_cntr, transition evnt_trnsn)
    {
        this->event_timestamp=evnt_ts;
           this->event_process=evnt_process;
           this->event_counter=evnt_cntr;
           this->event_transition=evnt_trnsn;
    }
    
    
    
};

class Scheduler
{
public:
    virtual void add_process_to_ready_queue(Process* process) =0;
    virtual Process* get_next_process_from_ready_queue()=0;
    virtual bool test_preempt(Process *p ) = 0;
};

class FCFS: public Scheduler{
public:
    deque<Process*> ready_queue;
    void add_process_to_ready_queue(Process* process)
    {
        ready_queue.push_back(process);
    }
    
    Process* get_next_process_from_ready_queue()
    {
        if(ready_queue.size()!=0)
        {
            Process* next_Process=ready_queue.front();
            ready_queue.pop_front();
            return(next_Process);
        }
        return(nullptr);
    }
    
    bool test_preempt(Process* p)
    {
        return(false);
    }
};
class LCFS: public Scheduler{
public:
    deque<Process*> ready_queue;
    void add_process_to_ready_queue(Process* process)
    {
        ready_queue.push_back(process);
    }
    
    Process* get_next_process_from_ready_queue()
    {
        if(ready_queue.size()!=0)
        {
            Process* next_Process=ready_queue.back();
            ready_queue.pop_back();
            return(next_Process);
        }
        return(nullptr);
    }
    
    bool test_preempt(Process* p)
    {
        return(false);
    }
};

class SRTF: public Scheduler{
public:
    deque<Process*> ready_queue;
    bool f=false;
    void add_process_to_ready_queue(Process* process)
    {
        deque<Process*>::iterator it;
        for(it=ready_queue.begin();it!=ready_queue.end();++it)
        {
            if((*it)->time_left_on_cpu>process->time_left_on_cpu)
            {
                break;
            }
        }
            ready_queue.insert(it,process);
    }
    
    Process* get_next_process_from_ready_queue()
    {
        if(ready_queue.size()!=0)
        {
            Process* next_Process=ready_queue.front();
            ready_queue.pop_front();
            return(next_Process);
        }
        return(nullptr);
    }
    
    bool test_preempt(Process* p)
    {
        return(false);
    }
};



class RR: public Scheduler{
public:
    deque<Process*> ready_queue;
    void add_process_to_ready_queue(Process* process)
    {
        ready_queue.push_back(process);
    }
    
    Process* get_next_process_from_ready_queue()
    {
        if(ready_queue.size()!=0)
        {
            Process* next_Process=ready_queue.front();
            ready_queue.pop_front();
            return(next_Process);
        }
        return(nullptr);
    }
    
    bool test_preempt(Process* p)
    {
        return(false);
    }
};

class PRIO :public Scheduler
{
public:
    
    queue<Process*>* active_multi_level_queue;
    queue<Process*>* expired_multi_level_queue;
    int maxprio;
    PRIO(int maxprio)
    {
        this->maxprio=maxprio;
        active_multi_level_queue=new queue<Process*>[maxprio];
        expired_multi_level_queue=new queue<Process*>[maxprio];
    }
    void add_process_to_ready_queue(Process* process)
    {
        if(process->dynamic_priority>=0)
        {
            active_multi_level_queue[process->dynamic_priority].push(process);
        }
        else
        {
            process->dynamic_priority=process->static_priority-1;
            expired_multi_level_queue[process->dynamic_priority].push(process);
        }
    }
    
    Process* get_next_process_from_ready_queue()
    {
        for(int i=maxprio-1;i>=0;i--)
        {
            if(active_multi_level_queue[i].size()>0)
            {
                Process* next_process=active_multi_level_queue[i].front();
                active_multi_level_queue[i].pop();
                return(next_process);
            }
        }
        queue<Process*>* temp=active_multi_level_queue;
        active_multi_level_queue=expired_multi_level_queue;
        expired_multi_level_queue=temp;
        
        for(int i=maxprio-1;i>=0;i--)
        {
            if(active_multi_level_queue[i].size()>0)
            {
                Process* next_process=active_multi_level_queue[i].front();
                active_multi_level_queue[i].pop();
                return(next_process);
            }
        }
        return(nullptr);
    }
    
    bool test_preempt(Process *process)
    {
        return(false);
    }
};




class PREPRIO: public Scheduler{
    public:
    queue<Process*>* active_multi_level_queue;
    queue<Process*>* expired_multi_level_queue;
    int maxprio;
    Process* running_process;
    
    PREPRIO(int maxprio)
    {
        this->maxprio=maxprio;
        active_multi_level_queue=new queue<Process*>[maxprio];
        expired_multi_level_queue=new queue<Process*>[maxprio];
    }
    void add_process_to_ready_queue(Process* process)
    {
        if(process->dynamic_priority>=0)
        {
            active_multi_level_queue[process->dynamic_priority].push(process);
        }
        else
        {
            process->dynamic_priority=process->static_priority-1;
            expired_multi_level_queue[process->dynamic_priority].push(process);
        }
    }
    Process* get_next_process_from_ready_queue(){
            for(int i=maxprio-1;i>=0;i--){
                if(active_multi_level_queue[i].size()>0){
                    Process* next_process = active_multi_level_queue[i].front();
                    active_multi_level_queue[i].pop();
                    running_process = next_process;
                    return next_process;
                }
            }
            // swap expired and active if active is empty throughout
            queue<Process*>* temp = active_multi_level_queue;
            active_multi_level_queue = expired_multi_level_queue;
            expired_multi_level_queue = temp;
            for(int i=maxprio-1;i>=0;i--){
                if(active_multi_level_queue[i].size()>0){
                    Process* next_process = active_multi_level_queue[i].front();
                    active_multi_level_queue[i].pop();
                    running_process = next_process;
                    return next_process;
                }
            }
            running_process = nullptr;
            return nullptr;
        }
        bool test_preempt(Process *p ){
            // check when doing preemptive scheduler
            if(running_process != nullptr && running_process->dynamic_priority < p->dynamic_priority){
                return true;
            }
            return false;
        }
    
};





std::deque<Event*> sim_queue;
int event_counter = 1;
int pid=0;
unordered_map<int, Process*> all_process_map;

Scheduler *scheduler;
int quantum = 10000;
int process_run_start_time=0,process_run_total=0;
int count_processes_in_io=0;
int no_process_io_total=0;

unordered_map<Process*, Event*> process_to_event_map;

int max_priority=4;
int size_of_random_values;
int current_time = -1, prev_time=0;
Process* CURRENT_RUNNING_PROCESS = nullptr;
bool CALL_SCHEDULER = false;

void read_input_file(string path);
void read_random_num_file(string rpath);
void add_event1(Event* e);
Event* get_first_event();
void pop_front_event() ;
int* randomvalues;
int ofs=0;
int get_random_num(int burst);
char type_of_scheduler='L';
void printing();


bool erase_from_the_map(Process* p1)
{
    auto it = process_to_event_map.find(p1);
    if(it!=process_to_event_map.end()){
        process_to_event_map.erase(it);
    }
}

void add_event1(Event* e)
{
    int pos=0;
    int flag=0;
    for(deque<Event*>::iterator i = sim_queue.begin() ; i!= sim_queue.end(); i++)
    {
        pos++;
        Event* curr_event = *i;
        
        if(curr_event->event_timestamp>e->event_timestamp)
        {
            flag=1;
            break;
        }
    }
    
    if(flag==0)
    {
        sim_queue.push_back(e);
    }
    else
    {
        deque<Event*>::iterator it = sim_queue.begin();
        advance(it,pos-1);
        sim_queue.insert(it,e);
    }
}

Event* get_first_event() {
    if (!sim_queue.empty()) {
        return sim_queue.front(); // Return a pointer to the first element
    } else {
        return nullptr; // Return nullptr if the event_queue is empty
    }
}

void pop_front_event() {
    if (!sim_queue.empty()) {
        sim_queue.pop_front(); // Removes the first event from the deque
    }
}


int main(int argc, char* argv[]){
    

    type_of_scheduler = 'P';
    int c;
    char* cvalue;
    



    while((c = getopt(argc, argv, "tveps:")) != -1){

        if(c=='s')
        {
            cvalue = static_cast<char *>(malloc(strlen(optarg) + 1));
            strcpy(cvalue, optarg);
            type_of_scheduler=cvalue[0];
            if(type_of_scheduler=='E')
            {
                char* temp = strtok(cvalue, ":");
                quantum = stoi(temp+1);
                temp = strtok(NULL,":");
                if(temp!=NULL)
                {
                        max_priority = stoi(temp);
                }
            }
            else if(type_of_scheduler=='P')
            {
                char* temp = strtok(cvalue, ":");
                quantum = stoi(temp+1);
                temp = strtok(NULL,":");
                if(temp!=NULL)
                {
                        max_priority = stoi(temp);
                }
            }
            else if (type_of_scheduler=='R')
            {
                char* temp = strtok(cvalue, ":");
                quantum = stoi(temp+1);
                temp = strtok(NULL,":");
                if(temp!=NULL)
                {
                        max_priority = stoi(temp);
                }
                
            }
            
        }
        else
        {
            cout<<"NOT TAKEN CARE OF CASE"<<endl;
        }
        
    }
    
    //quantum = take input;
    //string path = argv[optind];
    //string path = "/Users/savarchaturvedi/Desktop/os_ass2/lab/input0";
    //string rpath = argv[optind+1];
    //string rpath = "/Users/savarchaturvedi/Desktop/os_ass2/lab/rfile";
    read_random_num_file(argv[optind+1]);
    read_input_file(argv[optind]);
    //read_random_num_file(rpath);
    //read_input_file(path);
    //quantum=2;
    if (type_of_scheduler == 'F')
    {
    scheduler = new FCFS;
    }
    else if (type_of_scheduler == 'L')
    {
    scheduler = new LCFS;
    }
    else if (type_of_scheduler == 'S')
    {
    scheduler = new SRTF;
    }
    else if (type_of_scheduler == 'R')
    {
    scheduler = new RR;
    }
    else if (type_of_scheduler == 'P')
    {
    scheduler = new PRIO(max_priority);
    }
    else if (type_of_scheduler == 'E')
    {
    scheduler = new PREPRIO(max_priority);
    }
    else
    {
    cout<<"there is some error";
    }

    while(!sim_queue.empty()){
        Event* current_event = get_first_event();
        current_time = current_event->event_timestamp;
        Process* evnt_process = current_event->event_process;
        if(count_processes_in_io==0)
        {
            no_process_io_total+=current_time-prev_time;
        }
        
        prev_time=current_time;


        if(current_event->event_transition==TRANS_TO_READY)
        {
            scheduler->add_process_to_ready_queue(evnt_process);
            if(evnt_process->current_state==BLOCKED){
                count_processes_in_io=count_processes_in_io-1;
                evnt_process->time_spent_in_IO += current_time - evnt_process->latest_event_timestamp;
            }
            evnt_process->latest_event_timestamp=current_time;
            evnt_process->current_state=READY;
            CALL_SCHEDULER = true;
            if(scheduler->test_preempt(evnt_process))
            {
                auto k_v_iter=process_to_event_map.find(CURRENT_RUNNING_PROCESS);
                if(k_v_iter->second->event_timestamp>current_time)
                {
                    
                   Event* next_event_for_running_process = k_v_iter->second;
                   vector<Event*> temp_vector;
                   // Iterating until next_event_for_running_process is equal to event_queue.top()
                   for (; next_event_for_running_process != get_first_event(); pop_front_event()) {
                    temp_vector.push_back(get_first_event());
                    }
                    // Pop the top event_queue to ensure next_event_for_running_process is removed
                    pop_front_event();
                    

                    auto it1 = temp_vector.begin();
                    while (it1 != temp_vector.end()) {

                        add_event1(*it1);
                        
                        ++it1;
                        }

                    temp_vector.clear();
                    Event* new_premption_event=new Event(current_time,CURRENT_RUNNING_PROCESS,event_counter++,PREEMPTED_TO_READY);
                    add_event1(new_premption_event);

                    erase_from_the_map(CURRENT_RUNNING_PROCESS);
                    
                    
                    process_to_event_map.insert(make_pair(CURRENT_RUNNING_PROCESS,new_premption_event));
                }
                
            }
        }
        else if(current_event->event_transition==TRANS_TO_RUNNING)
        {
            if(CURRENT_RUNNING_PROCESS->current_cpu_burst==-1){
                    int time_granted = get_random_num(CURRENT_RUNNING_PROCESS->cpu_burst);
                    if(CURRENT_RUNNING_PROCESS->time_left_on_cpu < time_granted){
                        time_granted = CURRENT_RUNNING_PROCESS->time_left_on_cpu;
                    }
                    CURRENT_RUNNING_PROCESS->current_cpu_burst = time_granted;
                }
                
                int cpu_time_granted = evnt_process->current_cpu_burst;
                bool will_process_be_preempted = false;
                process_run_start_time=current_time;
                evnt_process->time_spent_ready_state += current_time - evnt_process->latest_event_timestamp;
                evnt_process->current_state=RUNNING;
                evnt_process->latest_event_timestamp=current_time;
                Event* future_event;
                bool g=false;
                if (evnt_process->current_cpu_burst > quantum) {
                    g=true;
                    
                }
                if(g)
                {
                    cpu_time_granted = quantum;
                    will_process_be_preempted = true;
                }
                if(evnt_process->time_left_on_cpu-cpu_time_granted==0){
                
                    
                    future_event = new Event(current_time + cpu_time_granted, evnt_process, event_counter++,TRANS_TO_EXITED);
                    
                    
                }
                else if(!will_process_be_preempted){
                    
                    future_event = new Event(current_time + cpu_time_granted, evnt_process, event_counter++,TRANS_TO_BLOCKED);
                    
                    
                }
                else{
                    
                    future_event = new Event(current_time + cpu_time_granted, evnt_process, event_counter++,PREEMPTED_TO_READY);
                    
                    
                }
                add_event1(future_event);
                erase_from_the_map(evnt_process);
                process_to_event_map.insert(make_pair(evnt_process, future_event));
        }
        else if(current_event->event_transition==TRANS_TO_BLOCKED)
        {
            CURRENT_RUNNING_PROCESS = nullptr;
            evnt_process->time_left_on_cpu-=evnt_process->current_cpu_burst;
            evnt_process->dynamic_priority = evnt_process->static_priority-1;
            evnt_process->latest_event_timestamp=current_time;

                process_run_total+=current_time-process_run_start_time;
                int curr_io_burst = get_random_num(evnt_process->io_burst);
                
                Event *future_event = new Event(current_time + curr_io_burst, evnt_process, event_counter++, TRANS_TO_READY);
                
                
                count_processes_in_io=count_processes_in_io+1;
                evnt_process->current_state=BLOCKED;
                evnt_process->current_cpu_burst= -1;
                
                CALL_SCHEDULER=true;
                erase_from_the_map(evnt_process);
                add_event1(future_event);
                
        }
        else if(current_event->event_transition==PREEMPTED_TO_READY)
        {
            erase_from_the_map(evnt_process);
            
            evnt_process->dynamic_priority--;
            
                
                evnt_process->current_cpu_burst-=(current_time-evnt_process->latest_event_timestamp);
               
                process_run_total+=current_time-process_run_start_time;
                
                scheduler->add_process_to_ready_queue(evnt_process);
                CALL_SCHEDULER = true;

                evnt_process->current_state=READY;
                evnt_process->time_left_on_cpu-=(current_time-evnt_process->latest_event_timestamp);
                CURRENT_RUNNING_PROCESS = nullptr;
                evnt_process->latest_event_timestamp=current_time;

        }
        else
        {
            erase_from_the_map(evnt_process);
            evnt_process->latest_event_timestamp=current_time;
            evnt_process->finishing_time=current_time;
            CURRENT_RUNNING_PROCESS = nullptr;
                process_run_total+=current_time-process_run_start_time;
                CALL_SCHEDULER = true;
                evnt_process->current_state=EXITED;
                
                

        }
        pop_front_event();
        
        if(CALL_SCHEDULER){
            if(sim_queue.size()!= 0){
                if( get_first_event()->event_timestamp == current_time)
                {
                continue;
                }
            }
            CALL_SCHEDULER = false;
            if(CURRENT_RUNNING_PROCESS == nullptr){
                CURRENT_RUNNING_PROCESS = scheduler->get_next_process_from_ready_queue();
                if(CURRENT_RUNNING_PROCESS == nullptr){
                    continue;
                }
                Event* future_event = new Event(current_time, CURRENT_RUNNING_PROCESS, event_counter++, TRANS_TO_RUNNING);
                add_event1(future_event);
                
            }
        }
    }
    printing();
}

void per_process_values(Process* p)
{
    int totalTime = p->finishing_time - p->arrival_time;

    // Use a vector of ints for the details that are purely numeric
    vector<int> details;

// Push back each detail individually
details.push_back(p->arrival_time);
details.push_back(p->time_on_cpu);
details.push_back(p->cpu_burst);
details.push_back(p->io_burst);
details.push_back(p->static_priority);
details.push_back(p->finishing_time);
details.push_back(p->finishing_time - p->arrival_time);
details.push_back(p->time_spent_in_IO);
details.push_back(p->time_spent_ready_state);
    // Output process_id specially to handle leading zeros
    cout << setw(4) << setfill('0') << p->process_id << ": ";

    // Iterate through the vector and print each detail with appropriate formatting
    for (int i = 0; i < details.size(); ++i) {
        if (i == 4) { // Check if we're about to print static_priority
            cout << details[i] << " | ";
        } else {
            // Apply formatting suitable for most details
            cout << setw(5) << setfill(' ') << details[i] << " ";
        }
    }

    cout << "\n";
    
    
}


void printing()
{
    
    if (type_of_scheduler == 'F')
    {
    cout << "FCFS\n";
    }
    else if (type_of_scheduler == 'L')
    {
    cout << "LCFS\n";
    }
    else if (type_of_scheduler == 'S')
    {
    cout << "SRTF\n";
    }
    else if (type_of_scheduler == 'R')
    {
    cout << "RR " << quantum << "\n";
    }
    else if (type_of_scheduler == 'P')
    {
    cout << "PRIO " << quantum << "\n";
    }
    else if (type_of_scheduler == 'E')
    {
    cout << "PREPRIO " << quantum << "\n";
    }
    else
    {
    cout<<"there is some error";
    }
    int total_turnaround_time=0,total_cpu_waiting_time=0;
    for(int i=0;i<pid;i++){
        Process* proc = all_process_map.find(i)->second;
        per_process_values(proc);

        total_turnaround_time+=proc->finishing_time-proc->arrival_time;
        total_cpu_waiting_time+=proc->time_spent_ready_state;
    }
    vector<double> metrics;

    // Calculating and adding the metrics to the vector using push_back
    metrics.push_back(100.0 * (process_run_total) / current_time);
    metrics.push_back(100.0 * (current_time - no_process_io_total) / current_time);
    metrics.push_back(1.0 * total_turnaround_time / pid);
    metrics.push_back(1.0 * total_cpu_waiting_time / pid);
    metrics.push_back(100.0 * pid / current_time);

    // Printing the metrics from the vector
    cout << "SUM: " << current_time << " ";
    for (size_t i = 0; i < metrics.size(); ++i) {
        if (i < metrics.size() - 1) { // For all but the last metric, use 2 decimal places
            cout << fixed << setprecision(2) << metrics[i] << " ";
        } else { // For the last metric (throughput), use 3 decimal places
            cout << fixed << setprecision(3) << metrics[i];
        }
    }
    cout << "\n";

}


void read_input_file(string input_file_path)
{
    ifstream input_file(input_file_path);
    string temp, temp2;
    int AT, TC, CB, IO;
    while(getline(input_file, temp)) {
        int curr_ind = 0;
        int i=4;
        while(i!=0)
        {
            int start = temp.find_first_not_of(' ',curr_ind);
            curr_ind=start;
            int end = temp.find_first_of(' ',curr_ind);
            curr_ind=end;
            if (end == -1 && i==4){
                break;
            }
            temp2 = temp.substr(start, end - start);
            if(i==4)
            {
                AT = stoi(temp2);
            }
            else if (i==3)
            {
                TC = stoi(temp2);
            }
            else if(i==2)
            {
                CB = stoi(temp2);
            }
            else
            {
                IO = stoi(temp2);
            }
            i=i-1;
        }
        
        
        Process* process = new Process(AT,TC,CB,IO,pid, get_random_num(max_priority));
        
        Event* event = new Event(AT,process,event_counter,TRANS_TO_READY);
        event_counter++;
        add_event1(event);
        all_process_map.insert(make_pair(pid, process));
        pid++;
        
    }
    

}

int get_random_num(int burst)
{
    if(ofs==size_of_random_values)
    {
        ofs=0;
    }
    int t=1+(randomvalues[ofs]%burst);
    ofs=ofs+1;
    return(t);
}


void read_random_num_file(string rf){
    ifstream input_file(rf);
    string temp;
    int index=0;
    bool flag = true;
    while(getline(input_file, temp)){
        if(flag){
            size_of_random_values = stoi(temp);
            flag = false;
            randomvalues = new int[size_of_random_values];
        }
        else{
            if(index==size_of_random_values){
                break;
            }
            randomvalues[index] = stoi(temp);
            index++;
        }
    }

}
