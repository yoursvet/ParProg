#include "num_threads.h"
#include <thread>
#include <omp.h>

static unsigned thread_no = std::thread::hardware_concurrency();

void set_num_threads(unsigned T) {
	if (T <= std::thread::hardware_concurrency() && T > 0)
	{
		thread_no = T;
		omp_set_num_threads(T);
	}
}

unsigned get_num_threads() {
	return thread_no;
}