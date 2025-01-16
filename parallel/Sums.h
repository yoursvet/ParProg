#pragma once
#include <vector>
unsigned localization_sum(unsigned* v, unsigned n);
unsigned sum_c_mutex(unsigned* v, unsigned v_size);
unsigned sum_cpp_mutex(std::vector <unsigned> v, unsigned v_size);
unsigned sum_omp_reduce(unsigned* v, unsigned n);
unsigned sum(unsigned* v, unsigned n);
unsigned sum_round_robin(unsigned* v, unsigned n);
unsigned round_robin(unsigned* v, unsigned n);
unsigned mutex_rb_rr(unsigned* v, unsigned n);
unsigned sum_round_robin_No_Localization(unsigned* v, unsigned n);