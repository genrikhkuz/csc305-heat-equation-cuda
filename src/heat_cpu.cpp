#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;

// Converts 2D coordinates (i, j) into a 1D array index
int index2D(int i, int j, int N) {
    return i * N + j;
}

// Writes the grid to a CSV file
void write_csv(vector<double> grid, int N, const char* filename) {
    ofstream file(filename);

    file << "x,y,temperature\n";

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            file << i << "," << j << "," << grid[index2D(i, j, N)] << "\n";
        }
    }

    file.close();
}

int main() {
    int N = 1000;
    int steps = 5000;
    double lambda = 0.1;

    vector<double> current(N * N, 0.0);
    vector<double> next(N * N, 0.0);

    // Initial condition
    int center = N / 2;
    int radius = 50;

    for (int i = center - radius; i <= center + radius; i++) {
        for (int j = center - radius; j <= center + radius; j++) {
            current[index2D(i, j, N)] = 100.0;
        }
    }

    // Save the initial grid
    write_csv(current, N, "output/initial.csv");

    // Start timing
    auto start = chrono::high_resolution_clock::now();

    // Main simulation loop
    for (int t = 0; t < steps; t++) {

        // Update only interior points
        for (int i = 1; i < N - 1; i++) {
            for (int j = 1; j < N - 1; j++) {

                double center_value = current[index2D(i, j, N)];
                double up = current[index2D(i - 1, j, N)];
                double down = current[index2D(i + 1, j, N)];
                double left = current[index2D(i, j - 1, N)];
                double right = current[index2D(i, j + 1, N)];

                next[index2D(i, j, N)] =
                    (1.0 - 4.0 * lambda) * center_value
                    + lambda * (up + down + left + right);
            }
        }

        // Keep boundary fixed at zero
        for (int i = 0; i < N; i++) {
            next[index2D(i, 0, N)] = 0.0;
            next[index2D(i, N - 1, N)] = 0.0;
            next[index2D(0, i, N)] = 0.0;
            next[index2D(N - 1, i, N)] = 0.0;
        }

        // Copy next grid into current grid
        for (int i = 0; i < N * N; i++) {
            current[i] = next[i];
        }

        // Save intermediate results
        if (t == 100) {
            write_csv(current, N, "output/step_100.csv");
        }

        if (t == 500) {
            write_csv(current, N, "output/step_500.csv");
        }
    }

    // Stop timing
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> elapsed = end - start;

    // Save final result
    write_csv(current, N, "output/final.csv");

    cout << "-------------------------------------------------------" << endl;
    cout << "SIMULATION END" << endl;
    cout << "-------------------------------------------------------" << endl;
    cout << "Grid size: " << N << " x " << N << endl;
    cout << "Number of steps: " << steps << endl;
    cout << "Runtime: " << elapsed.count() << " seconds" << endl;
    cout << "-------------------------------------------------------" << endl;

    return 0;
}