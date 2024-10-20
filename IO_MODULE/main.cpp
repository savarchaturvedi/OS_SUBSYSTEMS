//
//  main.cpp
//  os_lab_4
//
//  Created by Savar Chaturvedi on 24/04/24.
//


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
#include<map>
#include <algorithm>
#include <list>
#include <iomanip>

using namespace std;
int hand=0;

struct IO_request {
    int arrival_time;
    int track;
    int start_time;
    int end_time;
};

class IO_scheduler {
public:
    //deque<IO_request*> IO_QUEUE;
    list<IO_request*> IO_QUEUE;
    virtual IO_request* get_next_io_request() = 0;
    virtual void add_io_request_to_queue(IO_request* req)=0;
};

class FIFO_scheduler : public IO_scheduler {
public:
    IO_request* get_next_io_request() override
    {
        if(IO_QUEUE.size()!=0)
        {
            IO_request* ob=IO_QUEUE.front();
            IO_QUEUE.pop_front();
            return(ob);
        }
        return(nullptr);
    }
    
    void  add_io_request_to_queue(IO_request* req) override
    {
        IO_QUEUE.push_back(req);
    }
    
};

class SRTF : public IO_scheduler {
public:
    IO_request* get_next_io_request() override
    {
        
        if(IO_QUEUE.size()!=0)
        {
            IO_request* shortest_seq_req=nullptr;
            int min_seek_time=INT32_MAX;
            for (const auto& request_ptr : IO_QUEUE)
            {
                if(abs(request_ptr->track-hand)>=min_seek_time)
                {
                    continue;
                }
                else
                {
                    shortest_seq_req=request_ptr;
                    min_seek_time=abs(request_ptr->track-hand);
                }
            }
            IO_QUEUE.remove(shortest_seq_req);
            return(shortest_seq_req);
        }
        
        return(nullptr);
    }
    
    void  add_io_request_to_queue(IO_request* req) override
    {
        IO_QUEUE.push_back(req);
    }
    
};


class Look : public IO_scheduler {
public:
    int traversal=1;
    IO_request* travel()
    {
        IO_request* shortest_seq_req=nullptr;
        int min_seek_time=INT32_MAX;
        if(traversal==1)
        {
            for (const auto& request_ptr : IO_QUEUE)
            {
                if(request_ptr->track>=hand)
                {
                    if(request_ptr->track-hand<min_seek_time)
                    {
                        shortest_seq_req=request_ptr;
                        min_seek_time=request_ptr->track-hand;
                    }
                    else if(request_ptr->track-hand==min_seek_time and request_ptr->arrival_time<shortest_seq_req->arrival_time)
                    {
                        shortest_seq_req=request_ptr;
                    }
                }
            }
        }
        else
        {
            for (const auto& request_ptr : IO_QUEUE)
            {
                if(request_ptr->track<=hand)
                {
                    if(hand-request_ptr->track<min_seek_time)
                    {
                        shortest_seq_req=request_ptr;
                        min_seek_time=hand-request_ptr->track;
                    }
                    else if(hand-request_ptr->track==min_seek_time and request_ptr->arrival_time<shortest_seq_req->arrival_time)
                    {
                        shortest_seq_req=request_ptr;
                    }
                }
            }
        }
        return(shortest_seq_req);
    }
    IO_request* get_next_io_request() override
    {
        if(IO_QUEUE.size()!=0)
        {
            IO_request* io_to_s=travel();
            if(io_to_s==nullptr)
            {
                traversal=traversal*(-1);
                io_to_s=travel();
            }
            IO_QUEUE.remove(io_to_s);
            return(io_to_s);
        }
        return(nullptr);
    }
    void  add_io_request_to_queue(IO_request* req) override
    {
        IO_QUEUE.push_back(req);
    }
    
    
};


class Clook : public IO_scheduler
{
    int traversal=1;
    void  add_io_request_to_queue(IO_request* req) override
    {
        IO_QUEUE.push_back(req);
    }
    IO_request* travel()
    {
        IO_request* shortest_seq_req=nullptr;
        int min_seek_time=INT32_MAX;
        if(traversal==1)
        {
            for (const auto& request_ptr : IO_QUEUE)
            {
                if(request_ptr->track>=hand)
                {
                    if(request_ptr->track-hand<min_seek_time)
                    {
                        shortest_seq_req=request_ptr;
                        min_seek_time=request_ptr->track-hand;
                    }
                    else if(request_ptr->track-hand==min_seek_time and request_ptr->arrival_time<shortest_seq_req->arrival_time)
                    {
                        shortest_seq_req=request_ptr;
                    }
                }
            }
        }
        return(shortest_seq_req);
    }
    IO_request* get_next_io_request() override
    {
        if(IO_QUEUE.size()!=0)
        {
            IO_request* io_to_s=travel();
            if(io_to_s==nullptr)
            {
                int temp1=hand;
                hand=0;
                io_to_s=travel();
                hand=temp1;
            }
            IO_QUEUE.remove(io_to_s);
            return(io_to_s);
        }
        return(nullptr);
    }
};

class Flook : public IO_scheduler
{
public:
    list<IO_request*> active_queue;
    list<IO_request*> add_queue;
    int traversal=1;
    
    void  add_io_request_to_queue(IO_request* req) override
    {
        add_queue.push_back(req);
    }
    IO_request* travel()
    {
        IO_request* shortest_seq_req=nullptr;
        int min_seek_time=INT32_MAX;
        if(traversal==1)
        {
            for (const auto& request_ptr : active_queue)
            {
                if(request_ptr->track>=hand)
                {
                    if(request_ptr->track-hand<min_seek_time)
                    {
                        shortest_seq_req=request_ptr;
                        min_seek_time=request_ptr->track-hand;
                    }
                    else if(request_ptr->track-hand==min_seek_time and request_ptr->arrival_time<shortest_seq_req->arrival_time)
                    {
                        shortest_seq_req=request_ptr;
                    }
                }
            }
        }
        else
        {
            for (const auto& request_ptr : active_queue)
            {
                if(request_ptr->track<=hand)
                {
                    if(hand-request_ptr->track<min_seek_time)
                    {
                        shortest_seq_req=request_ptr;
                        min_seek_time=hand-request_ptr->track;
                    }
                    else if(hand-request_ptr->track==min_seek_time and request_ptr->arrival_time<shortest_seq_req->arrival_time)
                    {
                        shortest_seq_req=request_ptr;
                    }
                }
            }
        }
        return(shortest_seq_req);
    }
    
    IO_request* get_next_io_request() override
    {
        if(active_queue.size()==0 && add_queue.size()==0)
        {
            return(nullptr);
        }
        if(active_queue.size()==0)
        {
            list<IO_request*> temp=add_queue;
            add_queue=active_queue;
            active_queue=temp;
        }
        IO_request* io_to_p=travel();
        if(io_to_p==nullptr)
        {
            traversal=traversal*(-1);
            io_to_p=travel();
        }
        active_queue.remove(io_to_p);
        return(io_to_p);
    }
    
    
    
};


 IO_request simulation_queue[100001];

int total_requests=0;
void read_input_file(string path)
{
    
        ifstream file(path);
        if (!file.is_open()) {
            cerr << "Error: Unable to open file " << path << endl;
            
        }
        
        string line;
        while (getline(file, line)) {
            // Skip lines starting with '#'
            if (line[0] == '#') {
                continue;
            }
            
            istringstream iss(line);
            IO_request req;
            if (!(iss >> req.arrival_time >> req.track)) {
                cerr << "Error: Invalid input format in line: " << line << endl;
                continue;
            }
            simulation_queue[total_requests]=req;
            //simulation_queue.push_back(req); // Store IO_request object
            total_requests=total_requests+1;
        }
        file.close();
}

int current_time=0;

int total_waiting_time=0;
int max_wait_time=0;
int IO_BUSY_TIME=0;
int tot_movement=0;
int previous=0;

int total_turn_around_time=0;
map<int, IO_request*> myMap;
int counter=0;
//deque<IO_request*> IO_QUEUE;
IO_request *current_IO_processing=nullptr;
IO_scheduler* scheduler;
struct IO_request simmulation_queue[10000];
void scheduler_selector(char s)
{
    if(s=='N')
    {
        scheduler=new FIFO_scheduler();
    }
    else if(s=='S')
    {
        scheduler=new SRTF();
    }
    else if(s=='L')
    {
        scheduler=new Look();
    }
    else if(s=='C')
    {
        scheduler=new Clook();
    }
    else if(s=='F')
    {
        scheduler=new Flook();
    }
}

char algo = '\0';
int main(int argc, const char * argv[]) {
    string input_file = "";
        
        
        // Parse command line options
        int opt;
        while ((opt = getopt(argc, (char * const *)argv, "s:")) != -1) {
            switch (opt) {
                case 's':
                    algo = optarg[0];
                    break;
                default:
                    cerr << "Usage: " << argv[0] << " -s <algorithm> <input_file>" << endl;
                    exit(EXIT_FAILURE);
            }
        }
        
        // Check if input file name is provided
        if (optind >= argc) {
            cerr << "Error: Input file name not provided." << endl;
            exit(EXIT_FAILURE);
        }
        
        // Assign input file name
        input_file = argv[optind];

        // Now you have algo and input_file, you can use them as needed
    
    //string input_file_path="/Users/savarchaturvedi/Desktop/os_lab_4/os_lab_4/lab4_assign/input0";
    string input_file_path=input_file;
    read_input_file(input_file_path);
    scheduler_selector(algo);
    
    
    while(true)
    {
        
        if(current_time==simulation_queue[counter].arrival_time)
        {
            //IO_request* io_r=&simulation_queue.front();
            //simulation_queue.pop_front();
            scheduler->add_io_request_to_queue(&simulation_queue[counter]);
            //myMap[counter]=io_r;
            counter=counter+1;
            
        }
        if(current_IO_processing!=nullptr && current_IO_processing->track==hand)
        {
            current_IO_processing->end_time=current_time;
            current_IO_processing=nullptr;
            tot_movement=tot_movement+abs(previous-hand);
            previous=hand;
            
            
        }
        if(current_IO_processing==nullptr)
        {
            current_IO_processing=scheduler->get_next_io_request();
            //current_IO_processing=next_req;
            if(current_IO_processing!=nullptr)
            {
                current_IO_processing->start_time=current_time;
                total_waiting_time=total_waiting_time+(current_IO_processing->start_time-current_IO_processing->arrival_time);
                if(max_wait_time<current_IO_processing->start_time-current_IO_processing->arrival_time)
                {
                    max_wait_time=current_IO_processing->start_time-current_IO_processing->arrival_time;
                }
                
            }
            else if(counter==total_requests)
            {
                break;
            }
        }
        if(current_IO_processing!=nullptr)
        {
            if(current_IO_processing->track>hand)
            {
                hand=hand+1;
            }
            else if(current_IO_processing->track<hand)
            {
                hand=hand-1;
            }
            else{
                current_time=current_time-1;
            }
        }
        current_time=current_time+1;
    }
    
    for(int i = 0; i < counter; ++i) {
            total_turn_around_time=total_turn_around_time+(simulation_queue[i].end_time-simulation_queue[i].arrival_time);
            IO_BUSY_TIME=IO_BUSY_TIME+(simulation_queue[i].end_time-simulation_queue[i].start_time);
            std::cout << std::setw(5) << i << ": "
                      << std::setw(5) << simulation_queue[i].arrival_time << " "
                      << std::setw(5) << simulation_queue[i].start_time << " "
                      << std::setw(5) << simulation_queue[i].end_time << std::endl;
    }
    
    
    double avg_turn=total_turn_around_time*1.0/(counter);
    double avg_wait=total_waiting_time*1.0/(counter);
    double io_utal=(IO_BUSY_TIME*1.0)/current_time;
    std::cout << "SUM: "
                  << current_time << " "
                  << tot_movement<< " "
                  << std::fixed << std::setprecision(4) <<io_utal  << " "
                  << std::fixed << std::setprecision(2) <<avg_turn  << " "
                  << std::fixed << std::setprecision(2) << avg_wait << " "
                  << max_wait_time << std::endl;
    
    
    
}

