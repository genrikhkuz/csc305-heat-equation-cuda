#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <cmath>
#include <string>

using namespace std;

// Convert 2D coordinates (i, j) into a 1D array index
int index2D(int i, int j, int N) {
    return i * N + j;
}

// Write the grid to a CSV file
void write_csv(const vector<double>& grid, int N, const char* filename) {
    ofstream file(filename);

    if (!file) {
        cout << "FILE ERROR " << filename << endl;
        return;
    }

    file << "x,y,temperature\n";

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            file << i << "," << j << "," << grid[index2D(i, j, N)] << "\n";
        }
    }

    file.close();
}

// Put a hot square in the centre of the grid
void initialise_hot_square(vector<double>& grid, int N, int radius) {
    int centre = N / 2;

    for (int i = centre - radius; i <= centre + radius; i++) {
        for (int j = centre - radius; j <= centre + radius; j++) {
            if (i >= 0 && i < N && j >= 0 && j < N) {
                grid[index2D(i, j, N)] = 100.0;
            }
        }
    }
}

// Minimum temperature in the grid
double min_temperature(const vector<double>& grid) {
    double result = grid[0];

    for (int i = 0; i < (int)grid.size(); i++) {
        if (grid[i] < result) {
            result = grid[i];
        }
    }

    return result;
}

// Maximum temperature in the grid
double max_temperature(const vector<double>& grid) {
    double result = grid[0];

    for (int i = 0; i < (int)grid.size(); i++) {
        if (grid[i] > result) {
            result = grid[i];
        }
    }

    return result;
}

// Total heat in the grid
double total_heat(const vector<double>& grid) {
    double result = 0.0;

    for (int i = 0; i < (int)grid.size(); i++) {
        result = result + grid[i];
    }

    return result;
}

// Simple CUDA error checker
void check_cuda(cudaError_t result, const char* message) {
    if (result != cudaSuccess) {
        cout << "CUDA error: " << message << endl;
        cout << cudaGetErrorString(result) << endl;
        exit(1);
    }
}

// CUDA kernel: one thread updates one grid cell
__global__
void heat_step(double* current, double* next, int N, double lambda) {
    int j = blockIdx.x * blockDim.x + threadIdx.x;
    int i = blockIdx.y * blockDim.y + threadIdx.y;

    if (i >= N || j >= N) {
        return;
    }

    int position = i * N + j;

    // Fixed boundary conditions with boundary staying at zero
    if (i == 0 || i == N - 1 || j == 0 || j == N - 1) {
        next[position] = 0.0;
        return;
    }

    double centre_value = current[position];
    double up = current[(i - 1) * N + j];
    double down = current[(i + 1) * N + j];
    double left = current[i * N + (j - 1)];
    double right = current[i * N + (j + 1)];

    next[position] =
        (1.0 - 4.0 * lambda) * centre_value
        + lambda * (up + down + left + right);
}

int main(int argc, char* argv[]) {
    int N = 1000;
    int steps = 5000;
    double lambda = 0.1;
    int block_x = 16;
    int block_y = 16;
    int radius = 50;
    int save_output = 1;


    if (argc >= 2) {
        N = stoi(argv[1]);
    }

    if (argc >= 3) {
        steps = stoi(argv[2]);
    }

    if (argc >= 4) {
        block_x = stoi(argv[3]);
    }

    if (argc >= 5) {
        block_y = stoi(argv[4]);
    }

    if (argc >= 6) {
        radius = stoi(argv[5]);
    } else {
        radius = N / 20;
    }

    if (argc >= 7) {
        save_output = stoi(argv[6]);
    }

    int size = N * N;
    int bytes = size * sizeof(double);

    vector<double> current(size, 0.0);
    vector<double> final_grid(size, 0.0);

    initialise_hot_square(current, N, radius);

    if (save_output == 1) {
        write_csv(current, N, "output/initial_cuda.csv");
    }

    double* d_current;
    double* d_next;

    check_cuda(cudaMalloc((void**)&d_current, bytes), "could not allocate d_current");
    check_cuda(cudaMalloc((void**)&d_next, bytes), "could not allocate d_next");

    check_cuda(cudaMemcpy(d_current, current.data(), bytes, cudaMemcpyHostToDevice),
               "could not copy current grid to GPU");

    check_cuda(cudaMemset(d_next, 0, bytes),
               "could not initialise next grid on GPU");

    dim3 block(block_x, block_y);
    dim3 grid((N + block_x - 1) / block_x,
              (N + block_y - 1) / block_y);

    auto start = chrono::high_resolution_clock::now();

    for (int t = 0; t < steps; t++) {
        heat_step<<<grid, block>>>(d_current, d_next, N, lambda);

        double* temp = d_current;
        d_current = d_next;
        d_next = temp;
    }

    check_cuda(cudaGetLastError(), "kernel launch failed");
    check_cuda(cudaDeviceSynchronize(), "kernel execution failed");

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    check_cuda(cudaMemcpy(final_grid.data(), d_current, bytes, cudaMemcpyDeviceToHost),
               "could not copy final grid back to CPU");

    if (save_output == 1) {
        write_csv(final_grid, N, "output/final_cuda.csv");
    }

    cudaFree(d_current);
    cudaFree(d_next);

    cout << "-------------------------------------------------------" << endl;
    cout << "CUDA SIMULATION END" << endl;
    cout << "-------------------------------------------------------" << endl;
    cout << "Grid size: " << N << " x " << N << endl;
    cout << "Number of steps: " << steps << endl;
    cout << "Lambda: " << lambda << endl;
    cout << "Hot region radius: " << radius << endl;
    cout << "Block size: " << block_x << " x " << block_y << endl;
    cout << "CUDA grid size: " << grid.x << " x " << grid.y << endl;
    cout << "Runtime: " << elapsed.count() << " seconds" << endl;
    cout << "Minimum temperature: " << min_temperature(final_grid) << endl;
    cout << "Maximum temperature: " << max_temperature(final_grid) << endl;
    cout << "Total heat: " << total_heat(final_grid) << endl;
    cout << "-------------------------------------------------------" << endl;

    return 0;
}