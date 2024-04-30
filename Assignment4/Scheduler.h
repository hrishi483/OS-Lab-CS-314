#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

typedef pair<int, int> pi;

class Process {
private:
    queue<int> prc;
    int arrival_time = -1;

public:
    int p_id = -1;
    int response_time = -1;
    int turnaround_time = 0;
    float penalty_ratio = 0;
    int waiting_time = 0;
    int totalburst_time = 0;
    int totalp_time = 0;
    int i_quesize = 0;
    int c_quesize = 0;
    int c_btime = 0;

    void addSegment(int burst);
    void setArrivalTime(int time);
    int getBurst();
    void popBurst();
    int getArrivalTime();
};

class Scheduler {
private:
    int time = 0;
    int wait_processIsGoingOn = 0;
    int iowait_processIsGoingOn = 0;
    int ioprocessgoingon = 0;
    int currentCPUpid = -1;
    int currentIOpid = -1;
    queue<pi> ioQueue;

public:
    bool processgoingon = false;
    priority_queue<pi, vector<pi>, greater<pi>> readyQueue;
    vector<Process> process;
    unordered_map<int, vector<int>> arrival;

    void readdata(string file);
    void nextClock();
    void startEvent_sjf(string file);
    void startEvent_srtf(string file);
    void midPrint(int pid);
    void printOutput(string file);
};

#endif
