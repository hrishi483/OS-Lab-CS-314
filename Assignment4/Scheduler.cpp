#include "Scheduler.h"

void Process::addSegment(int burst) {
    prc.push(burst);
    i_quesize++;
    c_quesize++;
}

void Process::setArrivalTime(int time) {
    arrival_time = time;
}

int Process::getBurst() {
    return prc.front();
}

void Process::popBurst() {
    prc.pop();
    c_quesize--;
}

int Process::getArrivalTime() {
    return arrival_time;
}

void Scheduler::readdata(string file) {
     string myText;

        ifstream MyReadFile(file);
        int j = 0;
        int sum = 0;
        while (getline(MyReadFile, myText)) {
            if (myText[0] == '<') continue;
            stringstream ss(myText);
            string num;
            Process new_p;
            queue<int> q;
            int i = 0;
            while (getline(ss, num, ' ')) {
                stringstream ssnm(num);
                int x = 0;
                ssnm >> x;
                if (i == 0) {
                    new_p.p_id = j;
                    new_p.setArrivalTime(x);
                    arrival[x].push_back(j);
                } else {
                    if (x != -1 && x != 0) {
                        if (i % 2 == 1) {
                            sum += x; // ***
                            new_p.totalburst_time += x;
                        }
                        new_p.totalp_time += x;
                        // cout << x << ",";
                        new_p.addSegment(x);
                    }
                }
                i++;
            }
            j++;
            process.push_back(new_p);
        }
        MyReadFile.close();
}
void Scheduler::nextClock() {
  time++;
}

void Scheduler::startEvent_sjf(string file) {
        while (true) {
            if (arrival[time].size() > 0) {
                for (int prc = 0; prc < arrival[time].size(); prc++) {
                    readyQueue.push(make_pair(process[arrival[time][prc]].getBurst(), process[arrival[time][prc]].p_id));
                    process[arrival[time][prc]].popBurst();
                }
            }

            if (!processgoingon && readyQueue.size() > 0) {
                wait_processIsGoingOn = readyQueue.top().first;
                currentCPUpid = readyQueue.top().second;
                if (process[currentCPUpid].response_time == -1) {
                    process[currentCPUpid].response_time = time - process[currentCPUpid].getArrivalTime();
                }

                readyQueue.pop();
                processgoingon = true;
            }

            if (!ioprocessgoingon && ioQueue.size() > 0) {
                iowait_processIsGoingOn = ioQueue.front().first;
                currentIOpid = ioQueue.front().second;
                ioQueue.pop();
                ioprocessgoingon = true;
            }

            if (wait_processIsGoingOn >= 1) wait_processIsGoingOn--;
            if (iowait_processIsGoingOn >= 1) iowait_processIsGoingOn--;

            if (ioprocessgoingon && iowait_processIsGoingOn == 0) {
                if (process[currentIOpid].c_quesize == 0) {
                    midPrint(currentIOpid);
                } else {
                    if (process[currentIOpid].c_quesize > 0) {
                        readyQueue.push(make_pair(process[currentIOpid].getBurst(), process[currentIOpid].p_id));
                        process[currentIOpid].popBurst();
                    }
                }
                if (readyQueue.empty() && ioQueue.empty()) break;
                ioprocessgoingon = false;
            }

            if (processgoingon && wait_processIsGoingOn == 0) {
                if (process[currentCPUpid].c_quesize > 0) {
                    ioQueue.push(make_pair(process[currentCPUpid].getBurst(), currentCPUpid));
                    process[currentCPUpid].popBurst();
                } else {
                    midPrint(currentCPUpid);
                }
                if (readyQueue.empty() && ioQueue.empty()) break;
                processgoingon = false;
            }
            nextClock();
        }
    }


void Scheduler::startEvent_srtf(string file) {
 while (true) {
            if (arrival[time].size() > 0) {
                if (processgoingon)
                    readyQueue.push(make_pair(wait_processIsGoingOn, currentCPUpid));
                for (int prc = 0; prc < arrival[time].size(); prc++) {
                    readyQueue.push(make_pair(process[arrival[time][prc]].getBurst(), process[arrival[time][prc]].p_id));
                    // cout << time << "->"
                    //      << "Process arrived= " << arrival[time][prc] 
                    //      << " Burst = " << process[arrival[time][prc]].getBurst() << " " << endl;
                    process[arrival[time][prc]].popBurst();
                }
                processgoingon = false;
            }
            if (!processgoingon && readyQueue.size() > 0){
                wait_processIsGoingOn = readyQueue.top().first;
                currentCPUpid = readyQueue.top().second;
                if (process[currentCPUpid].response_time == -1) {
                    process[currentCPUpid].response_time = time - process[currentCPUpid].getArrivalTime();
                }


                readyQueue.pop();
                processgoingon = true;
            }

            if (!ioprocessgoingon && ioQueue.size() > 0) {
                iowait_processIsGoingOn = ioQueue.front().first;
                currentIOpid = ioQueue.front().second;
                // cout << time << ") "
                //      << "ioProcess Scheduled= " << ioQueue.front().second << " timetake= " << ioQueue.front().first << " " << endl;
                ioQueue.pop();
                ioprocessgoingon = true;
            }

            if (wait_processIsGoingOn >= 1) wait_processIsGoingOn--;
            if (iowait_processIsGoingOn >= 1) iowait_processIsGoingOn--;

            if (ioprocessgoingon && iowait_processIsGoingOn == 0) {
                if (process[currentIOpid].c_quesize == 0) {
                    midPrint(currentIOpid);
                } else {
                    if (process[currentIOpid].c_quesize > 0) {
                        readyQueue.push(make_pair(process[currentIOpid].getBurst(), process[currentIOpid].p_id));
                        process[currentIOpid].popBurst();
                    }
                }
                if (readyQueue.empty() && ioQueue.empty()) break;
                ioprocessgoingon = false;
            }

            if (processgoingon && wait_processIsGoingOn == 0) {
                if (process[currentCPUpid].c_quesize > 0) {
                    ioQueue.push(make_pair(process[currentCPUpid].getBurst(), currentCPUpid));
                    process[currentCPUpid].popBurst();
                } else {
                    midPrint(currentCPUpid);
                }
                if (readyQueue.empty() && ioQueue.empty()) {
                    break;
                }
                processgoingon = false;
            }
            nextClock();
            // cout<<"-----------------------"<<time;

        }   
        // cout<<"-----------------------"<<time;

}





void Scheduler::midPrint(int pid) {
    process[pid].turnaround_time = time - process[pid].getArrivalTime() + 1;
    process[pid].waiting_time = process[pid].turnaround_time - process[pid].totalburst_time;
    process[pid].penalty_ratio = ((process[pid].turnaround_time)-(process[pid].waiting_time)) / float(process[pid].turnaround_time);

}

void Scheduler::printOutput(string file) {
    float sum_responseTime = 0, sum_burstTime = 0, sun_turnAroundTime = 0, sum_waitTime = 0, sum_penaltyRatio = 0, numOfProcesses = process.size();
        cout << "_________________________Individual Process____________________________" << endl;
        for (int i = 0; i < numOfProcesses; i++)
        {
            cout << "Process ID =" << i 
                 << " Response Time = " << process[i].response_time 
                 << " Wait Time = " << process[i].waiting_time 
                 << " Burst Time = " << process[i].totalburst_time 
                 << " Turn Around Time = " << process[i].turnaround_time 
                 << " Penalty Ratio = " << process[i].penalty_ratio 
                 << endl;
            sum_responseTime += process[i].response_time;
            sum_burstTime += process[i].totalburst_time;
            sun_turnAroundTime += process[i].turnaround_time;
            sum_waitTime += process[i].waiting_time;
            sum_penaltyRatio += process[i].penalty_ratio;
        }
        cout << "_________________________System Average____________________________" << "\n";
        cout << "Response Time = " << sum_responseTime / numOfProcesses << endl
             << " Wait Time = " << sum_waitTime / numOfProcesses << endl
             << " Burst Time = " << sum_burstTime / numOfProcesses << endl
             << " Turn Around Time = " << sun_turnAroundTime / numOfProcesses << endl
             << " Penalty Ratio = " << sum_penaltyRatio / numOfProcesses << endl
             << " System throughput = " << numOfProcesses * 1.0 / (time - 1) << endl;
}

