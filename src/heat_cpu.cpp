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

int main(int argc, char* argv[]) {
    int N = 1000;
    int steps = 5000;
    double lambda = 0.1;
    int radius = 50;
    int save_output = 1;

    if (argc >= 2) {
        N = stoi(argv[1]);
    }

    if (argc >= 3) {
        steps = stoi(argv[2]);
    }

    if (argc >= 4) {
        radius = stoi(argv[3]);
    } else {
        radius = N / 20;
    }

    if (argc >= 5) {
        save_output = stoi(argv[4]);
    }

    vector<double> current(N * N, 0.0);
    vector<double> next(N * N, 0.0);

    initialise_hot_square(current, N, radius);

    if (save_output == 1) {
        write_csv(current, N, "output/initial_cpu.csv");
    }

    auto start = chrono::high_resolution_clock::now();

    for (int t = 0; t < steps; t++) {

        for (int i = 1; i < N - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                double centre_value = current[index2D(i, j, N)];
                double up = current[index2D(i - 1, j, N)];
                double down = current[index2D(i + 1, j, N)];
                double left = current[index2D(i, j - 1, N)];
                double right = current[index2D(i, j + 1, N)];

                next[index2D(i, j, N)] =
                    (1.0 - 4.0 * lambda) * centre_value
                    + lambda * (up + down + left + right);
            }
        }

        // Fixed boundary conditions with boundary staying at zero
        for (int i = 0; i < N; i++) {
            next[index2D(i, 0, N)] = 0.0;
            next[index2D(i, N - 1, N)] = 0.0;
            next[index2D(0, i, N)] = 0.0;
            next[index2D(N - 1, i, N)] = 0.0;
        }

        current.swap(next);

        if (save_output == 1 && t + 1 == 100) {
            write_csv(current, N, "output/step_100_cpu.csv");
        }

        if (save_output == 1 && t + 1 == 500) {
            write_csv(current, N, "output/step_500_cpu.csv");
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    if (save_output == 1) {
        write_csv(current, N, "output/final_cpu.csv");
    }

    cout << "-------------------------------------------------------" << endl;
    cout << "CPU SIMULATION END" << endl;
    cout << "-------------------------------------------------------" << endl;
    cout << "Grid size: " << N << " x " << N << endl;
    cout << "Number of steps: " << steps << endl;
    cout << "Lambda: " << lambda << endl;
    cout << "Hot region radius: " << radius << endl;
    cout << "Runtime: " << elapsed.count() << " seconds" << endl;
    cout << "Minimum temperature: " << min_temperature(current) << endl;
    cout << "Maximum temperature: " << max_temperature(current) << endl;
    cout << "Total heat: " << total_heat(current) << endl;
    cout << "-------------------------------------------------------" << endl;

    return 0;
}