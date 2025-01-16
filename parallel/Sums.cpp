#include "Sums.h"
#include "num_threads.h"
#include "own_barrier_latch.h"
#include <omp.h>

struct partial_t {
	alignas(std::hardware_destructive_interference_size) // = 40
		unsigned value;
};

/*
	1) Без выравнивания сумма (т.е. просто распараллелили и получили сумму)
	2) С выравниванием partial_t
	3) С хорошей локальностью чтения
	4, 5) С мьютексом и автоматической редукцией (ещё 2)
	6) Ручная редукция
*/

// Tree summarization
unsigned localization_sum(unsigned* v, unsigned n) {
	unsigned T = get_num_threads();
	auto part_sum = std::make_unique<partial_t[]>(T);
	unsigned sum = 0; Barrier br(T);
	auto worker_proc = [&part_sum, T, v, n, &br](unsigned t) {
		unsigned s, b, e;
		s = n / T;
		b = n % T;
		part_sum[t].value = 0;
		if (t < b) {
			b = (s + 1) * t;
			e = b + s + 1;
		}
		else {
			b = b + s * t;
			e = b + s;
		}
		unsigned m = 0;
		for (unsigned i = b; i < e; i++) {
			m += v[i];
		}
		part_sum[t].value = m;
		br.arrive_and_wait();
		for (size_t neighbor = 1, next = 2; neighbor < T; neighbor = next, next += next) {
			if (t % next == 0 && t + neighbor < T) {
				part_sum[t].value += part_sum[t + neighbor].value;
			}
			br.arrive_and_wait();
		}
	};
	std::vector <std::thread> w(get_num_threads() - 1);
	for (unsigned t = 1; t < get_num_threads(); t++) {
		w.at(t - 1) = std::thread(worker_proc, t);
	}
	worker_proc(0);
	for (auto& thr : w) {
		if (thr.joinable())
		{
			thr.join();
		}
	}
	return part_sum[0].value;
}

//Sum, using local_sums (then summarize them to get general sum)
unsigned sum_c_mutex(unsigned* v, unsigned v_size) {
	unsigned sum = 0;
	std::mutex mtx;
	auto worker = [&v, &sum, &mtx, &v_size](unsigned t) {
		unsigned local_sum = 0;
		const unsigned threadsCount = get_num_threads();
		size_t s = v_size / threadsCount,
			b = v_size % threadsCount;
		t < b ? b = ++s * t : b += s * t;
		const size_t e = b + s;
		for (size_t i = b; i < e; ++i) {
			local_sum += v[i];
		}
		{
			std::scoped_lock(mtx);
			sum += local_sum;
		}
		};
	std::vector <std::thread> w(get_num_threads() - 1);
	for (unsigned t = 1; t < get_num_threads(); t++) {
		w.at(t - 1) = std::thread(worker, t);
	}
	worker(0);
	for (auto& thr : w) {
		if (thr.joinable()) 
		{
			thr.join();
		}
	}
	return sum;
}

// Same method to uppon, input parameter is vector for cpp calls
// ЭТО НЕ НАДО
unsigned sum_cpp_mutex(std::vector <unsigned> v, unsigned v_size) {
	unsigned sum = 0;
	std::mutex mtx;
	auto worker = [&v, &sum, &mtx, &v_size](unsigned t) {
		unsigned local_sum = 0;
		const unsigned threadsCount = get_num_threads();
		size_t s = v_size / threadsCount,
			b = v_size % threadsCount;
		t < b ? b = ++s * t : b += s * t;
		const size_t e = b + s;
		for (size_t i = b; i < e; ++i) {
			local_sum += v.at(i);
		}

		{
			std::scoped_lock(mtx);
			sum += local_sum;
		}
		};

	std::vector <std::thread> w(get_num_threads() - 1);
	for (unsigned t = 1; t < get_num_threads(); t++) {
		w.at(t - 1) = std::thread(worker, t);
	}
	worker(0);
	for (auto& thr : w) {
		if (thr.joinable())
		{
			thr.join();
		}
	}
	return sum;
}

unsigned sum_omp_reduce(unsigned* v, unsigned n) {
	unsigned sum = 0;
#pragma omp parallel for reduction (+ : sum)
	for (int i = 0; i < n; i++) {
		sum += v[i];
	}
	return sum;
}

unsigned sum(unsigned* v, unsigned n) {
	unsigned sum = 0;
	for (int i = 0; i < n; i++) {
		sum += v[i];
	}
	return sum;
}

//round_robin algorithm
unsigned sum_round_robin(unsigned* v, unsigned n) {
	unsigned sum = 0;
	unsigned T = 0;
	// Введём массив сумм каждого потока
	partial_t* part_sum;

#pragma omp parallel
	{
		unsigned t = omp_get_thread_num();
		// T - count of threads
		// t - index of thread
		#pragma omp single 
		{
			T = omp_get_num_threads();
			part_sum = (partial_t*)malloc(sizeof(partial_t) * T);
		}
		part_sum[t].value = 0;
		for (int i = t; i < n; i += T)
		{
			part_sum[t].value += v[i];
		}
	}
	for (int i = 0; i < T; ++i) {
		sum += part_sum[i].value;
	}
	free(part_sum);
	return sum;
}

unsigned round_robin(unsigned *v, unsigned n) {
	partial_t* part_sum;
	int T;
	int sum = 0;
	#pragma omp parallel 
	{
		unsigned t = omp_get_thread_num();
		#pragma omp single 
		{
			T = omp_get_num_threads();
			part_sum = (partial_t*)malloc(sizeof(partial_t) * T);
		}
		part_sum[t].value = 0;
		int s = n / T;
		int b = n % T;

		if (t < b)
			b = (s + 1) * t;
		else
			b += s * t;

		int e = b + s;

		for (int i = b; i < e; i++)
		{
			part_sum[t].value += v[i];
		}
	}
	for (int j = 0; j < T; j++)
	{
		sum += part_sum[j].value;
	}
	free(part_sum);
	return sum;
}

unsigned mutex_rb_rr(unsigned* v, unsigned n)
{
	unsigned T;
	unsigned sum = 0;
	#pragma omp parallel 
	{
		unsigned mysum = 0;
		unsigned t = omp_get_thread_num();
		unsigned s, b, e;
		T = omp_get_num_threads();
		s = n / T;
		b = n % T;
		if (t < b)
			b = ++s * t;
		else
			b = b + s * t;
		e = b + s;
		for (unsigned i = b; i < e; i++)
		{
			mysum += v[i];
		}
		#pragma omp critical 
		{
			sum += mysum;
		}
	}
	return sum;
}

unsigned sum_round_robin_No_Localization(unsigned* v, unsigned n) { 
	unsigned sum = 0; 
	unsigned T = 0; 
	// Массив сумм каждого потока 
	unsigned* part_sum; 
	#pragma omp parallel 
	{ 
		unsigned t = omp_get_thread_num(); 
		// T - count of threads 
		// t - index of thread 
		#pragma omp single 
		{ 
			T = omp_get_num_threads(); 
			part_sum = (unsigned*)calloc(sizeof v[0], T); 
		} 
		for (int i = t; i < n; i += T) { 
			part_sum[t] += v[i]; 
		} 
	} 
	sum = part_sum[0]; 
	for (int i = 1; i < T; ++i) { 
	sum += part_sum[i]; 
	} 
	free(part_sum); 
	return sum; 
}