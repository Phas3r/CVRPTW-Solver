#pragma once
#include <cassert>
#include <string>
#include <iostream>
#include "problem.h"
#include <random>
#include <algorithm>
#include <iterator>

#if defined(DEBUG)
#define DEBUG_CHECK(cond) assert(cond)
#else
#define DEBUG_CHECK
#endif

bool ReadFile(std::string filename, Problem& problem);

bool WriteSolutionToFile(const Solution& solution, const Problem& problem);

bool WriteResultToFile(const Solution & solution, const Problem & problem);


template<typename Iter, typename RandomGenerator>
size_t get_random_index(Iter start, Iter end, RandomGenerator& g)
{
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    return dis(g);
}
template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g)
{
    std::advance(start, get_random_index(start, end, g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

template<typename Iter>
size_t get_random_index(Iter start, Iter end)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return get_random_index(start, end, gen);
}
