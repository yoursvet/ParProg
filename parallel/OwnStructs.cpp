#include <iostream>

struct partial_t {
	alignas(std::hardware_destructive_interference_size) // = 40
		unsigned value;
};

struct scalability_result {
	unsigned result; // Это должна быть сумма для отслеживания результата значения функции
	double t, s, e; // Ускорение и эффективность
};