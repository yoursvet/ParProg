#include "own_barrier_latch.h"

void Latch::arrive_and_wait() {
	std::unique_lock l(this->mtx);
	if (--T == 0) { 
		// If after change of threads count they are equal 0,
		// then all threads are done, job of "Latch" is completed
		cv.notify_all();
	}
	else {
		do
			cv.wait(l);
		while (T > 0);
	}
}

void Barrier::arrive_and_wait() {
	std::unique_lock l(this->mtx);
	if (--T == 0) {
		T = T0; // Reset T parameter
		this->barrierGeneration = !this->barrierGeneration; // Next generation of barrier could be created (current isn't actual)
		cv.notify_all();
	}
	else {
		bool myBarrier = this->barrierGeneration;
		while (myBarrier == this->barrierGeneration)
			cv.wait(l);
	}
}