CXX = g++
CXXFLAGS = -O2 -std=c++17

all: heat_cpu

heat_cpu: src/heat_cpu.cpp
	$(CXX) $(CXXFLAGS) src/heat_cpu.cpp -o heat_cpu

run: heat_cpu
	./heat_cpu
