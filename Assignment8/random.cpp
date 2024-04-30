#include <iostream>
#include <fstream>
#include <cstdlib>
#include <bits/stdc++.h>  

using namespace std;

void showMemory(vector<int> mem)
{
    for(auto m: mem) {
        cout << m << " ";
    }
    cout << endl;
}


int main(int argc, char* argv[]) {
    if(argc != 5){
        cout << "ERROR: Expected 5 arguments. Only " << argc << " were given!" << endl;
        exit(0);
    }
    // Initializing the constants 
    int virtualPages = atoi(argv[1]), physicalFrames = atoi(argv[2]), swapSpace = atoi(argv[3]);
    int pageFault = 0;
    if (virtualPages > swapSpace+physicalFrames) {
        cout << "ERROR: There cannot be more virtual pages than the swap space!" << endl;
        return 0;
    }
    //cout << virtualPages << endl << physicalFrames << endl << swapSpace << endl;
    vector<int> requests;
    vector<int> memory;
    vector<int>::iterator it;

    ifstream inputFile;
    string line, token;
    inputFile.open(argv[4]);
    while (getline(inputFile, line)) {
        // cout << "requests: " << line << endl;
        stringstream ss(line);
        while(getline(ss, token, ' ')){
            requests.push_back(stoi(token));
        }
    }

    // showMemory(memory);
    int cycle = 1;
    for(auto req: requests){
        it = find(memory.begin(), memory.end(), req);
        if(it == memory.end() ) {
            // not found in memory
            // add to memory and increase pageFault
            if (memory.size() >= physicalFrames) {
                srand(time(0));
                it = find(memory.begin(), memory.end(), memory[rand()% physicalFrames]);
                memory.erase(it);
            } 
            memory.push_back(req);
            pageFault++;
        }
        // cout << "Cycle " << cycle << " ==== ";
        // showMemory(memory);
        // cycle++;
    }
    // cout << "Total Page Faults: " << pageFault << endl;
    cout << pageFault << endl;

    return 0;
}