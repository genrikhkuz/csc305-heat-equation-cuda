Compile:
make clean
make

Run CPU:
./heat_cpu

Run CPU with parameters:
./heat_cpu N steps radius save_output

E.g.
./heat_cpu 1000 5000 50 1

Run CUDA:
./heat_cuda

Run CUDA with parameters:
./heat_cuda N steps block_x block_y radius save_output

E.g.
./heat_cuda 1000 5000 16 16 50 1

Benchmark examples:
./heat_cpu 300 1000 15 0
./heat_cuda 300 1000 16 16 15 0

./heat_cpu 500 2000 25 0
./heat_cuda 500 2000 16 16 25 0

./heat_cpu 1000 5000 50 0
./heat_cuda 1000 5000 16 16 50 0

Test CUDA block sizes:
./heat_cuda 1000 5000 8 8 50 0
./heat_cuda 1000 5000 16 16 50 0
./heat_cuda 1000 5000 32 8 50 0

Generate output for plotting:
./heat_cuda 1000 5000 16 16 50 1

Plot:
python3 scripts/plot_heat.py

Clean:
make clean