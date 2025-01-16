#pragma once
#include <concepts>
#include <chrono>
#include <vector>
#include <algorithm>
#include <thread>
#include <numeric>
#include <iostream>

void set_num_threads(unsigned T);
unsigned get_num_threads();