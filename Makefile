
all: output clean

output: main.o
	g++ -std=c++11 -o run main.cpp
main.o: main.cpp 
	g++ -std=c++11 -c main.cpp 
clean:
	rm *.o
