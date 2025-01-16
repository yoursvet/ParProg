#pragma once
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

class Latch {
private:
	std::condition_variable cv;
	std::mutex mtx;
	unsigned T;

public:
	Latch(const unsigned threads) : T(threads) {}
	void arrive_and_wait();
	
};

class Barrier {
private:
	std::condition_variable cv;
	std::mutex mtx;
	unsigned T;
	bool barrierGeneration = false;
	const unsigned T0;

public:
	Barrier(const unsigned threads) : T(threads),T0(threads) {}
	void arrive_and_wait();
};