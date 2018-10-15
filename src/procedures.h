#pragma once

#include "problem.h"
#include "utils.h"


bool ProcedureTwoOpt(Problem& problem, Route& route, size_t index1, size_t index2, double& gain, bool revert = false);

bool ProcedureOrOpt(Problem& problem, Route& route, size_t index1, size_t index2, double& gain, bool revert = false);

bool ProcedureRelocate(Problem& problem, Route& route1, Route& route2, size_t index1, size_t index2, double& gain, bool forced = false, bool revert = false);

bool ProcedureExchange(Problem& problem, Route& route1, Route& route2, size_t index1, size_t index2, double& gain, bool forced = false, bool revert = false);

bool ProcedureCrossExchange(Problem& problem, Route& route1, Route& route2, size_t index1, size_t index2, size_t segment_size1, size_t segment_size2, double& gain, bool revert = false);
