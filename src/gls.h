#pragma once
#include "problem.h"
#include "utils.h"
#include "local_search.h"

typedef std::vector<std::vector<size_t>> Penalties;
typedef std::pair<size_t, size_t> Feature;
typedef std::vector<Feature> Features;

Features ChoosePenaltyFeatures(Problem& problem, Penalties& penalties, Solution& solution);

Solution GuidedLocalSearch(Problem& problem, Solution solution);