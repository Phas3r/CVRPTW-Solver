#pragma once
#include "local_search.h"
#include <random>
#include <algorithm>
#include <iterator>

bool TwoOpt(Problem& problem, Route& route)
{
    if (route.size() < 4)
        return false;
    bool route_changed = false;
    for (size_t iteration = 0; iteration < 2; ++iteration)
    {
    label:
        bool change_made = false;
        Route best_route = route;
        Route old_route = route;
        double best_gain = 0.0;
        double gain = 0.0;
        for (size_t i = 1; i < route.size() - 1; ++i)
        {
            for (size_t j = i + 1; j < route.size() - 1; ++j)
            {
                if (ProcedureTwoOpt(problem, route, i, j, gain))
                {
                    if ((best_gain - gain) > 0.0001 || !problem.IsFeasible(route, false) || (problem.GetDistance(route) - problem.GetDistance(best_route) < -0.00001) )
                    {
                        ProcedureTwoOpt(problem, route, i, j, gain, true);
                    }
                    else
                    {
                        best_gain = gain;
                        change_made = true;
                        route_changed = true;
                        best_route = route;
                        route = old_route;
                    }
                }
            }
        }
        if (change_made)
        {
            route = best_route;
            goto label;
        }
        break;
    }
    return route_changed;
}

bool OrOpt(Problem& problem, Route& route)
{
    if (route.size() < 4)
        return false;
    bool route_changed = false;
    for (size_t iteration = 0; iteration < 2; ++iteration)
    {
    label:
        bool change_made = false;
        Route best_route = route;
        double best_gain = 0.0;
        double gain = 0.0;
        for (size_t i = 1; i < route.size() - 2; ++i)
        {
            for (size_t j = 1; j < route.size() - 1; ++j)
            {
                if (ProcedureOrOpt(problem, best_route, i, j, gain))
                {
                    if ((best_gain - gain) > 0.0001 || !problem.IsFeasible(best_route, false))
                    {
                        ProcedureOrOpt(problem, best_route, i, j, gain, true);
                    }
                    else
                    {
                        best_gain = gain;
                        change_made = true;
                        route_changed = true;
                    }
                }
            }
        }
        if (change_made)
        {
            route = best_route;
            goto label;
        }
        break;
    }
    return route_changed;
}

bool Relocate(Problem& problem, Route& route1, Route& route2)
{
    if (route1.size() < 3)
        return false;
    bool routes_changed = false;
    for (size_t iteration = 0; iteration < 2; ++iteration)
    {
    label:
        bool change_made = false;
        size_t best_index1 = -1;
        size_t best_index2 = -1;
        Route old_route1 = route1;
        Route old_route2 = route2;
        double best_gain = 0.0;
        double gain = 0.0;
        for (size_t i = 1; i < route1.size() - 1; ++i)
        {
            for (size_t j = 0; j < route2.size() - 1; ++j)
            {
                if (ProcedureRelocate(problem, route1, route2, i, j, gain))
                {

                    if ((best_gain - gain) > 0.0001 || !problem.IsFeasible(route1, true) || !problem.IsFeasible(route2, true))
                    {
                        ProcedureRelocate(problem, route1, route2, i, j, gain, false, true);
                    }
                    else
                    {
                        best_gain = gain;
                        best_index1 = i;
                        best_index2 = j;
                        change_made = true;
                        routes_changed = true;
                        route1 = old_route1;
                        route2 = old_route2;
                    }
                }
            }
        }
        if (change_made)
        {
            ProcedureRelocate(problem, route1, route2, best_index1, best_index2, gain);
            goto label;
        }
        break;
    }
    return routes_changed;
}

bool Exchange(Problem& problem, Route& route1, Route& route2)
{
    if (route1.size() < 3 || route2.size() < 3)
        return false;
    bool routes_changed = false;
    for (size_t iteration = 0; iteration < 2; ++iteration)
    {
    label:
        bool change_made = false;
        size_t best_index1 = -1;
        size_t best_index2 = -1;
        Route old_route1 = route1;
        Route old_route2 = route2;
        double best_gain = 0.0;
        double gain = 0.0;
        for (size_t i = 1; i < route1.size() - 1; ++i)
        {
            for (size_t j = 1; j < route2.size() - 1; ++j)
            {
                if (ProcedureExchange(problem, route1, route2, i, j, gain))
                {
                    if ((best_gain - gain) > 0.0001 || !problem.IsFeasible(route1, true) || !problem.IsFeasible(route2, true) || (problem.GetDistance(route1) - problem.GetDistance(old_route1) + problem.GetDistance(route2) - problem.GetDistance(old_route2) < -0.00001))
                    {
                        ProcedureExchange(problem, route1, route2, i, j, gain, false, true);
                    }
                    else
                    {
                        best_gain = gain;
                        best_index1 = i;
                        best_index2 = j;
                        change_made = true;
                        routes_changed = true;
                        route1 = old_route1;
                        route2 = old_route2;
                    }
                }
            }
        }
        if (change_made)
        {
            ProcedureExchange(problem, route1, route2, best_index1, best_index2, gain);
            goto label;
        }
        break;
    }
    return routes_changed;
}

bool CrossExchange(Problem& problem, Route& route1, Route& route2)
{
    if (route1.size() < 3 || route2.size() < 3 || route1.size() < 4 && route2.size() < 4)
        return false;
    bool routes_changed = false;
    for (size_t segment_size2 = 5; segment_size2 >= 2; --segment_size2)
    {
        for (size_t segment_size1 = segment_size2; segment_size1 >= 1; --segment_size1)
        {
        label:
            bool change_made = false;
            size_t best_index1 = -1;
            size_t best_index2 = -1;
            Route old_route1 = route1;
            Route old_route2 = route2;
            double best_gain = 0.0;
            double gain = 0.0;
            for (size_t i = 1; i < route1.size() - 1; ++i)
            {
                for (size_t j = 1; j < route2.size() - 1; ++j)
                {
                    if (ProcedureCrossExchange(problem, route1, route2, i, j, segment_size1, segment_size2, gain))
                    {
                        if ((best_gain - gain) > 0.0001 || !problem.IsFeasible(route1, true) || !problem.IsFeasible(route2, true))
                        {
                            ProcedureCrossExchange(problem, route1, route2, i, j, segment_size1, segment_size2, gain, true);
                        }
                        else
                        {
                            best_gain = gain;
                            best_index1 = i;
                            best_index2 = j;
                            change_made = true;
                            routes_changed = true;
                            route1 = old_route1;
                            route2 = old_route2;
                        }
                    }
                }
            }
            if (change_made)
            {
                ProcedureCrossExchange(problem, route1, route2, best_index1, best_index2, segment_size1, segment_size2, gain);
                goto label;
            }
            break;
        }
    }
    return routes_changed;
}


bool LocalSearchTwoOpt(Problem& problem, Solution & solution)
{
    bool change_made = false;
    for (auto& route : solution)
    {
        change_made |= TwoOpt(problem, route);
    }
    return change_made;
}

bool LocalSearchOrOpt(Problem& problem, Solution & solution)
{
    bool change_made = false;
    for (auto& route : solution)
    {
        change_made |= OrOpt(problem, route);
    }
    return change_made;
}

bool LocalSearchRelocate(Problem& problem, Solution & solution)
{
    size_t interations = 0;
    bool change_made = false;
label:
    interations++;
    bool not_local_optimum = false;
    for (auto& route1 : solution)
    {
        for (auto& route2 : solution)
        {
            not_local_optimum |= Relocate(problem, route1, route2);
        }
    }
    if (not_local_optimum)
    {
        change_made = true;
        if (interations > solution.size())
            goto label;
    }
    return change_made;
}

bool LocalSearchExchange(Problem& problem, Solution & solution)
{

    size_t interations = 0;
    bool change_made = false;
label:
    interations++;
    bool not_local_optimum = false;
    for (auto& route1 : solution)
    {
        for (auto& route2 : solution)
        {
            not_local_optimum |= Exchange(problem, route1, route2);
        }
    }
    if (not_local_optimum)
    {
        change_made = true;
        if (interations > 5)
            goto label;
    }
    return change_made;
}

bool LocalSearchCrossExchange(Problem& problem, Solution & solution)
{

    size_t interations = 0;
    bool change_made = false;
label:
    interations++;
    bool not_local_optimum = false;
    for (auto& route1 : solution)
    {
        for (auto& route2 : solution)
        {
            not_local_optimum |= CrossExchange(problem, route1, route2);
        }
    }
    if (not_local_optimum)
    {
        change_made = true;
        if (interations > 5)
            goto label;
    }
    return change_made;
}

bool MakeLocalSearchWithOperation(LocalSearchOperations operation_type, Problem& problem, Solution & solution)
{
    switch (operation_type)
    {
    case LocalSearchOperations::OperationTwoOpt:
        return LocalSearchTwoOpt(problem, solution);
    case LocalSearchOperations::OperationOrOpt:
        return LocalSearchOrOpt(problem, solution);
    case LocalSearchOperations::OperationRelocate:
        return LocalSearchRelocate(problem, solution);
    case LocalSearchOperations::OperationExchange:
        return LocalSearchExchange(problem, solution);
    case LocalSearchOperations::OperationCrossExchange:
        return LocalSearchCrossExchange(problem, solution);
    default:
        return false;
    }
}

bool LocalSearch(Problem& problem, Solution & solution)
{
    std::vector<LocalSearchOperations> operations;
    operations.push_back(LocalSearchOperations::OperationTwoOpt);
    operations.push_back(LocalSearchOperations::OperationOrOpt);
    operations.push_back(LocalSearchOperations::OperationExchange);
    operations.push_back(LocalSearchOperations::OperationRelocate);
    operations.push_back(LocalSearchOperations::OperationCrossExchange);


    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(operations.begin(), operations.end(), g);

    operations.push_back(LocalSearchOperations::OperationOrOpt);
    operations.push_back(LocalSearchOperations::OperationTwoOpt);
    bool modified = false;
    bool not_local_optimum = true;
    size_t iterations = 0;
    while (not_local_optimum && iterations<10)
    {
        iterations++;
        not_local_optimum = false;
        for (auto operation : operations)
        {
            not_local_optimum |= MakeLocalSearchWithOperation(operation, problem, solution);
            modified |= not_local_optimum;
        }
        std::shuffle(operations.begin(), operations.end(), g);
    }
    return modified;
}