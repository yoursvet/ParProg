#include <iostream>
#include <chrono>
#include <vector>
#include "num_threads.h"

struct scalability_result {
    unsigned result;
    double t, s, e; 
};

template <std::invocable<unsigned*, size_t> Fun>
std::vector<scalability_result> run_experiment(Fun f, unsigned v_size_min, unsigned v_size_max) {
    unsigned* v = new unsigned[v_size_max - v_size_min + 1];
    for (unsigned i = v_size_min, j = 0; i < v_size_max; i++, j++) {
        v[j] = i;
    }
    unsigned P = get_num_threads();
    std::vector<scalability_result> scale(P);

    for (unsigned T = 1; T <= P; ++T) {
        set_num_threads(T);
        auto t_start = std::chrono::steady_clock::now();
        scale.at(T - 1).result = f(v, v_size_max-v_size_min +1);
        auto t_end = std::chrono::steady_clock::now();
        scale.at(T - 1).t = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count());
        scale.at(T - 1).s = static_cast<double>(scale.at(0).t) / static_cast<double>(scale.at(T - 1).t);
        scale.at(T - 1).e = static_cast<double>(scale.at(T - 1).s) / T;
    }
    delete[]v;

    return scale;
}


template <std::invocable<const std::vector<unsigned>&, size_t> Functor>
std::vector<scalability_result> run_experiment(Functor f, unsigned v_size_min, unsigned v_size_max) {
    std::vector<unsigned> v(v_size_max - v_size_min);
    std::iota(v.begin(), v.end(), v_size_min);
    unsigned P = get_num_threads();

    std::vector<scalability_result> scale(P);

    for (unsigned T = 1; T <= P; ++T) {
        set_num_threads(T);
        auto t_start = std::chrono::steady_clock::now();
        scale.at(T - 1).result = f(v, v.size());
        auto t_end = std::chrono::steady_clock::now();
        scale.at(T - 1).t = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count());
        scale.at(T - 1).s = static_cast<double>(scale.at(0).t) / static_cast<double>(scale.at(T - 1).t);
        scale.at(T - 1).e = static_cast<double>(scale.at(T - 1).s) / T;
    }
    v.clear();

    return scale;
}

template <std::invocable<unsigned*, size_t, unsigned, unsigned, unsigned> Func>
std::vector<scalability_result> run_experiment_rand(Func f, unsigned arraySize, unsigned x_min, unsigned x_max, unsigned entropy) {
    unsigned* v = new unsigned[arraySize];
    for (unsigned i = 0; i < arraySize; i++) {
        v[i] = 0;
    }
    unsigned P = get_num_threads();
    std::vector<scalability_result> scale(P);

    for (unsigned T = 1; T <= P; ++T) {
        set_num_threads(T);
        auto t_start = std::chrono::steady_clock::now();
        f(v, arraySize, x_min, x_max, entropy);
        auto t_end = std::chrono::steady_clock::now();
        scale.at(T - 1).result = 0;
        scale.at(T - 1).t = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count());
        scale.at(T - 1).s = static_cast<double>(scale.at(0).t) / static_cast<double>(scale.at(T - 1).t);
        scale.at(T - 1).e = static_cast<double>(scale.at(T - 1).s) / T;
    }
    delete[]v;
    return scale;
}

