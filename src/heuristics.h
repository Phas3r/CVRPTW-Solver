#pragma once
#include "problem.h"
#include "utils.h"

Solution FindSolutionFromInsertionTypeOneHeuristic(Problem& problem, double alpha, double lambda);

Solution FindSolutionFromInsertionTypeThreeHeuristic(Problem& problem);

Solution FindSolutionFromTimeOrientedNearestNeighborHeuristic(Problem& problem, double delta1, double delta2, double delta3);