all:
	g++ -std=c++17 -pthread Singleton.cpp -o Singleton

run:
	./Singleton