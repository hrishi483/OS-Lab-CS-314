all: main_sjf.cpp Scheduler.cpp
	g++ -c main_sjf.cpp -o main_sjf.o
	g++ -c Scheduler.cpp -o Scheduler.o
	g++ main_sjf.o Scheduler.o -o sjf
	g++ -c main_srtf.cpp -o main_srtf.o
	g++ -c Scheduler.cpp -o Scheduler.o
	g++ main_srtf.o Scheduler.o -o srtf