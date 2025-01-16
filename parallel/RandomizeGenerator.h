#pragma once
#include <iostream>
#include <concepts>
#include <type_traits>
#include <thread>
#include <vector>
#include "num_threads.h"

// Линейный конгрдентн. генератор

class af {
private:
	unsigned a, b;
public:
	af() {
		a = 1;
		b = 0;
	}
	af(unsigned aa, unsigned bb) : a(aa), b(bb) {}

	unsigned operator ()(unsigned x) const {
		return a * x + b;
	}

	af& operator *=(af& x) {
		a = a * x.a;
		b = a * x.b + b;
		return *this;
	}
};

template <unsigned n>
class Z {
private:
	unsigned x;
public:
	Z() {
		x = 1;
	}
	Z(unsigned v):x(v) {}

	Z& operator *= (const Z& el) {
		x = (x * el.x)%n;
		return *this;
	}
	unsigned get() const {
		return x;
	}
};

namespace myPow {
	template <class T> concept monoid = requires (T x) { T(); x *= x; };
	template <monoid T, std::unsigned_integral U>
	T pow(T x, U n) {
		auto r = T();
		while (n > 0) {
			if ((n & 1) != 0) {
				r *= x;
			}
			x *= x;
			n >>= 1;
		}
		return r;
	}
}

template <class F> requires std::invocable<F, unsigned>
void affine_transoform(unsigned aa, unsigned bb, F map, unsigned* v, size_t n, unsigned x0) {
	unsigned T = get_num_threads();
	std::vector <std::thread> w(T - 1);

	auto worker = [v, aa, bb, map, n, x0, T](unsigned t) {
		unsigned s, b, e;
		s = n / T;
		b = n % T;
		if (t < b)
			b = 1 + s * t;
		else
			b += s * t;
		e = b + s;
		af gen(aa, bb);
		for (size_t i = b; i < e; i++) {
			v[i] = map(myPow::pow(gen, i)(x0));
		}
	};

	for (unsigned t = 1; t < T; t++) {
		w.emplace_back(worker, t);
	}
	worker(0);
	for (auto& thr : w)
	{
		if (thr.joinable())
		{
			thr.join();
		}
	}
}

void randomize(unsigned* v, size_t n, unsigned x_min, unsigned x_max, unsigned entropy) {
	unsigned a = 0x8088405; // parameter of affine transformation
	unsigned b = 1; // same
	unsigned range = x_max - x_min + 1;

	auto map = [range, x_min](unsigned x) -> unsigned {
		return (x % range) + x_min;
	};

	affine_transoform(a, b, map, v, n, entropy);
}



