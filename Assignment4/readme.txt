# Shortest Job First (SJF) and Shortest Remaining Time First (SRTF) Schedulers

This project implements two CPU scheduling algorithms: Shortest Job First (SJF) and Shortest Remaining Time First (SRTF). It includes source code files for the main programs (`main_sjf.cpp` and `main_srtf.cpp`) and the scheduler implementation (`Scheduler.cpp`).

## Building the Executables

To compile the project, you can use the provided `Makefile`. The Makefile contains rules for compiling the source code files and linking them to create the executables.

To compile both the SJF and SRTF programs, run:

make

### Once the executables are built, you can run them directly from the command line to run the sjf algorithm:

./sjf <filename>.dat


### Once the executables are built, you can run them directly from the command line to run the srtf algorithm:

./srtf <filename>.dat 