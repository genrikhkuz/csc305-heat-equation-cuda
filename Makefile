CXX = g++
NVCC = /usr/local/cuda/bin/nvcc

CXXFLAGS = -O2 -std=c++17
NVCCFLAGS = -O2 -arch=sm_60 -std=c++11 -I/usr/local/cuda/include

all: heat_cpu heat_cuda

heat_cpu: src/heat_cpu.cpp
	$(CXX) $(CXXFLAGS) src/heat_cpu.cpp -o heat_cpu

heat_cuda: src/heat_cuda.cu
	$(NVCC) $(NVCCFLAGS) src/heat_cuda.cu -o heat_cuda

run_cpu: heat_cpu
	./heat_cpu

run_cuda: heat_cuda
	./heat_cuda

clean:
	rm -f heat_cpu
	rm -f heat_cuda
	rm -f output/*.csv