

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <vector>
#include <chrono>


//      Struct Process
struct process {
  int id;
  float arrivalTime;
  float serviceTime;
  float remainingServiceTime;
  //int priority;
};
//      Struct Event
struct event {
  std::string type; //ARR or DEP
  float time;
  event* nextEvent;
  process* p;

  //constructor
  event(std::string e_type, int t) {
    type = e_type;
    time = t;
  }
};

//      Globals
bool isIdle = true;
float currTime;
int readyQueueCount;
event* eq_head = NULL;
int numProcesses = 0;
float avgTurnaround = 0;
float totThroughput;
float avgCpuUtil = 0;
float avgNumProcs = 0;
double serviceTime;
int lambda;
float x;
std::vector<float> arrTimes;
std::vector<float> serTimes;
std::vector<int> rq;


//      Schedule an Event
void Schedule_event(std::string e_type, float t, event* head) {
  event* e = new event(e_type,t);//create event
  std::cout << "Event Scheduled : " << numProcesses << std::endl;//debug
  e->type = e_type;e->time = t;
  if (eq_head == NULL) { //queue is empty, put on front
    eq_head = e;
  } else {
    event* temp = eq_head;
    while (temp->nextEvent != NULL) { //traverse to end
      temp = temp->nextEvent;
    }
    temp->nextEvent = e; //place new node in back of queue
  }
}
//      Initialize Simulation
void Init(int lambda) {
  std::cout << "Initializing..." << std::endl;
  x = rand()/RAND_MAX; //generate a number b/w 0 and 1
  float t = (-(1/lambda)*log(x)); //Generate 1st Interarrival time
  Schedule_event("ARR",currTime + t,eq_head);
  readyQueueCount = 0;
}
//      Arrival Event Handler
void Arr_handler(event* e) {
  if (isIdle == true) { //CPU available
    std::cout << "CPU is idle" << std::endl;
    isIdle = false;
    float s = (-serviceTime) * log(((double)rand()/(double)RAND_MAX));
    avgTurnaround = avgTurnaround + s;
    Schedule_event("DEP",currTime + s,eq_head); //add to Event Queue
    currTime = currTime + s;
    float a = (-(1/lambda)*log(x));
    //arrTimes.push_back(a);
    Schedule_event("ARR",currTime + a,eq_head); //Create next Arrival
    //rq.push_back(readyQueueCount);
  } else { //CPU not available
    readyQueueCount++;
    std::cout << "CPU is not available" << std::endl;
    float a = (-(1/lambda)*log(x));
    //arrTimes.push_back(a);
    Schedule_event("ARR",currTime + a,eq_head); //add to Event Queue
    currTime = currTime + a;
    //avgNumProcs = avgNumProcs + readyQueueCount;
  } 
}
//      Departure Event Handler
void Dep_handler(event* e) {
  numProcesses++;
  if (readyQueueCount == 0) {//No new procs
    isIdle = true;
  } else {//New proc(s) avail
    readyQueueCount--;
    float s = (-serviceTime) * log(((double)rand()/(double)RAND_MAX));
    avgTurnaround = avgTurnaround + s;
    Schedule_event("DEP",currTime + s,eq_head);
    currTime = currTime + s;
  }
}
//      Get Next Event From Queue
event* Get_next_event () {
  return eq_head;
}
//      Delete Event From Queue
void Delete_event (event* e) {
  event* temp = eq_head;
  eq_head = temp->nextEvent;
}



//      Run the Simulation
void Run() { //run through all procs
  std::cout << "Running" << std::endl;
  while (numProcesses < 10000) {
    event* e = Get_next_event();
    std::cout << "Current Time: "<< currTime<< std::endl;
    //record the time
    //record interval length
    //record states
    //currTime = currTime + (e->time);
    if (e->type == "ARR") {
      std::cout << "arrival event" << std::endl;
      //arrTimes.push_back(e->p->arrivalTime);//record arrival time
      Arr_handler(e);
    } else if (e->type == "DEP") {
      std::cout << "departure event" << std::endl;
      //record arrival time
      Dep_handler(e);
    }
    Delete_event(e);
  }
}

  int main(int argc, char*argv[]) {
    //grabs first argument, lambda, and stores it as an int
    lambda = std::atoi(argv[1]); //Average Arrival Rate
    //grabs second argument, average service time, and stores it as a double
    serviceTime = std::atof(argv[2]); //Average Service Time
    srand((unsigned)time(NULL)); //allows for rng
    //currTime = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    Init(lambda);
    Run();
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);

    avgTurnaround = avgTurnaround/10000;
    totThroughput = 10000/currTime;
    float idleTime = std::chrono::duration<float>(duration).count() - currTime;
    avgCpuUtil = -(currTime/idleTime);
    //avgNumProcs = avgNumProcs/10000;
    std::cout << "Discrete-Time Event Simulator (FCFS) for 10,000 processes with arrival rate of lambda = "  + std::to_string(lambda) + " and average service time of " +  std::to_string(serviceTime) + "\n";
  
    std::cout << "Average turnaround time: " + std::to_string(avgTurnaround) << std::endl;
    std::cout << "Total throughput: " + std::to_string(totThroughput) << std::endl;
    std::cout << "Average CPU utilization: " + std::to_string(avgCpuUtil) << std::endl;
    std::cout << "Average number of processes in the Ready Queue: " + std::to_string(readyQueueCount) << std::endl;
    return 0;
  }
