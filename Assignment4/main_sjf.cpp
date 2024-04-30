#include "Scheduler.h"

int main(int argc, char *argv[]) {
    string file = argv[1];
    Scheduler sh;
    sh.readdata(file);
    sh.startEvent_sjf(file);    
    sh.printOutput(file);
    return 0;
}
