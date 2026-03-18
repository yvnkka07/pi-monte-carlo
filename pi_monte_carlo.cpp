#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono>
#include <fstream>

using namespace std;

void monteCarloWorker(long long points, long long& insideCount) {
    random_device rd;
    mt19937_64 gen(rd());
    uniform_real_distribution<double> dist(0.0, 1.0);

    long long localInside = 0;

    for (long long i = 0; i < points; i++) {
        double x = dist(gen);
        double y = dist(gen);
        if (x * x + y * y <= 1.0)
            localInside++;
    }

    insideCount = localInside;
}

int main() {
    vector<long long> N_values = {
        1000000LL, 10000000LL, 100000000LL,
        1000000000LL, 10000000000LL, 100000000000LL
    };

    vector<int> M_values = { 1, 2, 4, 8, 16, 32, 64, 128 };

    ofstream file("../pi-monte-carlo/pi_monte_carlo_parallel_results.csv");

    // Header
    file << "N/M";
    for (int M : M_values)
        file << "," << M;
    file << "\n";

    for (long long N : N_values) {
        file << N;

        for (int M : M_values) {
            vector<thread> threads;
            vector<long long> results(M, 0);

            long long pointsPerThread = N / M;

            auto start = chrono::high_resolution_clock::now();

            for (int i = 0; i < M; i++) {
                threads.emplace_back(monteCarloWorker,
                    pointsPerThread,
                    ref(results[i]));
            }

            for (auto& t : threads)
                t.join();

            long long totalInside = 0;
            for (auto val : results)
                totalInside += val;

            double pi = 4.0 * totalInside / (pointsPerThread * M);

            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> duration = end - start;

            file << "," << duration.count();

            cout << "N=" << N
                << " M=" << M
                << " pi=" << pi
                << " time=" << duration.count() << "s\n";
        }

        file << "\n";
    }

    file.close();
    return 0;
}