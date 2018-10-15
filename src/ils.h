#pragma once
#include "problem.h"
#include "utils.h"
#include "local_search.h"

void Perturbate(Problem& problem, Solution& solution, bool strong);

Solution IteratedLocalSearch(Problem& problem, Solution solution);