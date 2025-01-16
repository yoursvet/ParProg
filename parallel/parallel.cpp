#include <iostream>
#include <vector>
#include "omp.h"
#include <mutex>
#include "num_threads.h"
#include <fstream>
#include "own_barrier_latch.h"
#include <thread>
#include "ExperementsRunner.h"
#include "Sums.h"
#include "RandomizeGenerator.h"

// Method to create csv file from vector that contains scalability_result objects
void to_csv(std::ostream& io, std::vector<scalability_result> v) {
	io << "Threads,Result,Time,Speedup,Efficiency\n";
	for (unsigned i = 0; i < v.size(); i++) {
		io << i << ',' << v.at(i).result << ',' << v.at(i).t << ',' << v.at(i).s << ',' << v.at(i).e << '\n';
	}
}

void to_csv_wo_result(std::ostream& io, std::vector<scalability_result> v) {
	io << "N,Time,Speedup,Efficiency\n";
	for (unsigned i = 0; i < v.size(); i++) {
		io << i+1 << ',' << v.at(i).t << ',' << v.at(i).s << ',' << v.at(i).e << '\n';
	}
}

int main() {
	auto sr = run_experiment(sum_c_mutex, 1u, 1u<<28);

	std::ofstream os("sum_c_mutex.csv", std::ios_base::out);
	if (os.is_open()) {
		to_csv(os, sr);
		os.close();
	}

	auto sr2 = run_experiment(localization_sum, 1u, 1u << 28);
	std::ofstream os2("localization_sum.csv", std::ios_base::out);
	if (os2.is_open()) {
		to_csv(os2, sr2);
		os2.close();
	}

	auto sr3 = run_experiment(sum_omp_reduce, 1u, 1u << 28);
	std::ofstream os3("sum_omp_reduce.csv", std::ios_base::out);
	if (os3.is_open()) {
		to_csv(os3, sr3);
		os3.close();
	}

	auto sr4 = run_experiment(sum, 1u, 1u << 28);
	std::ofstream os4("sum.csv", std::ios_base::out);
	if (os4.is_open()) {
		to_csv(os4, sr4);
		os4.close();
	}
	auto sr5 = run_experiment(sum_round_robin, 1u, 1u << 28);
	std::ofstream os5("sum_round_robin.csv", std::ios_base::out);
	if (os5.is_open()) {
		to_csv(os5, sr5);
		os5.close();
	}
	auto sr6 = run_experiment(round_robin, 1u, 1u << 28);
	std::ofstream os6("round_robin.csv", std::ios_base::out);
	if (os6.is_open()) {
		to_csv(os6, sr6);
		os6.close();
	}
	auto sr7 = run_experiment(mutex_rb_rr, 1u, 1u << 28);
	std::ofstream os7("mutex_rb_rr.csv", std::ios_base::out);
	if (os7.is_open()) {
		to_csv(os7, sr7);
		os7.close();
	}

	auto sr8 = run_experiment(sum_round_robin_No_Localization, 1u, 1u << 28);
	std::ofstream os8("sum_round_robin_No_Localization.csv", std::ios_base::out);
	if (os8.is_open()) {
		to_csv(os8, sr8);
		os8.close();
	}

	//randomize experiment
	unsigned sz = 2 << 20; // 2^20 elements in array
	unsigned* v = new unsigned[sz];
	auto randomExp = run_experiment_rand(randomize, sz, 0, 1000, 0);
	for (int i = 0; i < randomExp.size(); i++) {
		std::cout << "Cores: " << i + 1 << "\n";
		std::cout << "time (ms): " << randomExp.at(i).t << "\n";
		std::cout << "speedup: " << randomExp.at(i).s << "\n";
		std::cout << "efficiency: " << randomExp.at(i).e << "\n\n";
	}
	std::ofstream randomizerInput("randomize_experiment.csv", std::ios_base::out);
	if (randomizerInput.is_open()) {
		to_csv_wo_result(randomizerInput, randomExp);
		randomizerInput.close();
	}

	return 0;
}

