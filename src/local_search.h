#pragma once
#include "problem.h"
#include "procedures.h"

enum LocalSearchOperations
{
    OperationTwoOpt = 0,
    OperationOrOpt = 1,
    OperationRelocate = 2,
    OperationExchange = 3,
    OperationCrossExchange = 4
};

bool LocalSearchTwoOpt(Problem& problem, Solution& solution);
bool LocalSearchOrOpt(Problem& problem, Solution& solution);
bool LocalSearchRelocate(Problem& problem, Solution& solution);
bool LocalSearchExchange(Problem& problem, Solution& solution);
bool LocalSearchCrossExchange(Problem& problem, Solution& solution);
bool MakeLocalSearchWithOperation(LocalSearchOperations operation_type, Problem& problem, Solution & solution);
bool LocalSearch(Problem& problem, Solution& solution);