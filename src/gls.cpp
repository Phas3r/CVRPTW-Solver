#include "gls.h"
#pragma once


Features ChoosePenaltyFeatures(Problem & problem, Penalties & penalties, Solution & solution)
{
    Features features;
    size_t max_features = 3;
    std::vector<std::pair<Feature, double>> feature_costs;
    for (auto& route: solution)
    {
        for (size_t j = 0; j < route.size() - 1; ++j)
        {
            double cost = problem.GetÒNormalDistance(route[j], route[j+1]) / (double(penalties[route[j]][route[j + 1]] + 1));
            feature_costs.push_back({ { route[j], route[j + 1] },  cost });
        }
    }

    auto func = [&](std::pair<std::pair<size_t, size_t>, double>& it1, std::pair<std::pair<size_t, size_t>, double>& it2)
    {
        return it1.second > it2.second;
    };
    std::sort(feature_costs.begin(), feature_costs.end(), func);
    for (size_t i = 0; i < max_features; ++i)
    {
        features.push_back(feature_costs[i].first);
    }
    return features;
}

Solution GuidedLocalSearch(Problem & problem, Solution solution)
{
    solution.erase(std::remove_if(solution.begin(), solution.end(), [](Route& r) { return r.size() == 2; }),
        solution.end());
    LocalSearch(problem, solution);
    Penalties penalties;
    penalties.resize(problem.GetCustomersNum());
    for (auto& d : penalties)
    {
        d.resize(problem.GetCustomersNum(), 0);
    }
	auto stat = problem.GetSolutionStatus(solution);
    double best_cost = stat.cost;
    Solution best_solution = solution;

	size_t num_iterations;
	if (stat.feasible)
	{
		num_iterations = 2000;
	}
	else
	{
		num_iterations = 5000;
	}
    size_t iters_without_improvement = 0;
    for (size_t i = 0; i < num_iterations; ++i)
    {
        Features features = ChoosePenaltyFeatures(problem, penalties, solution);
        for (auto feature : features)
        {
            penalties[feature.first][feature.second] += 1;
            problem.UpdateMatrix(feature);
        }
        LocalSearch(problem, solution);
        solution.erase(std::remove_if(solution.begin(), solution.end(), [](Route& r) { return r.size() == 2; }),
            solution.end());
        if (problem.ValidateSolution(solution))
        {
            Problem::SolutionStatus status = problem.GetSolutionStatus(solution);
            if (best_cost > status.cost)
            {
                iters_without_improvement = 0;
                best_cost = status.cost;
                best_solution = solution;
            }
            else
            {
                iters_without_improvement++;
            }
        }
        else
        {
            iters_without_improvement++;
        }
    }
    problem.ResetMatrix();
    LocalSearch(problem, best_solution);
    return best_solution;
}
