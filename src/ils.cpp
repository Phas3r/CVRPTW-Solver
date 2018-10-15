#include "ils.h"
#include "procedures.h"

static bool TryApplyRandomExchange(Problem& problem, Solution& solution, Route& route, size_t index)
{
    if (route.size() < 3 || route.size() <= index + 1)
        return false;
    size_t num_tries = 100;
    for (size_t i = 0; i < num_tries; ++i)
    {
        Route& r = *select_randomly(solution.begin(), solution.end());
        if (r.size() < 3)
            continue;
        for (size_t j = 0; j < 10; ++j)
        {
            size_t index2 = r.size() > 3? get_random_index(r.begin(), r.end() - 2) + 1 : 1;
            double gain = 0;
            ProcedureExchange(problem, route, r, index, index2, gain, true);
            if (problem.IsFeasible(route) && problem.IsFeasible(r))
            {
                return true;
            }
            else
            {
                ProcedureExchange(problem, route, r, index, index2, gain, false, true);
            }
        }
    }
    return false;
}

static bool TryApplyRandomRelocate(Problem& problem, Solution& solution, Route& route, size_t index)
{
    if (route.size() < 3 || route.size() <= index + 1)
        return false;
    size_t num_tries = 10;
    for (size_t i = 0; i < num_tries; ++i)
    {
        Route& r = *select_randomly(solution.begin(), solution.end());
        for (size_t j = 0; j < 10; ++j)
        {
            size_t index2 = r.size() > 2 ? get_random_index(r.begin() + 1, r.end() - 1) : 0;
            double gain = 0;
            ProcedureRelocate(problem, route, r, index, index2, gain, true);
            if (problem.IsFeasible(route) && problem.IsFeasible(r))
            {
                return true;
            }
            else
            {
                ProcedureRelocate(problem, route, r, index, index2, gain, false, true);
            }
        }
    }
    return false;
}

void Perturbate(Problem& problem, Solution & solution, bool strong)
{
    size_t max_customers = problem.GetCustomersNum() / 10;
    std::vector<std::pair<std::pair<size_t, size_t>,double>> customers_costs;
    for (size_t route_i = 0; route_i < solution.size(); ++route_i)
    {
        for (size_t iter = 1; iter < solution[route_i].size() - 1; ++iter)
        {
            double time_cost = (iter == solution[route_i].size() - 2) ? 0 : 
                0.1*(problem.GetBeginTime(solution[route_i], iter + 1)
                - problem.GetBeginTime(solution[route_i], iter));
            double cost = problem.GetDistance(solution[route_i][iter], solution[route_i][iter - 1])
                + problem.GetDistance(solution[route_i][iter + 1], solution[route_i][iter])
                + time_cost;
            customers_costs.push_back({ {route_i, iter}, cost });
        }
    }
    std::vector<std::pair<size_t, size_t>> worst_customers;
    auto func = [&](std::pair<std::pair<size_t, size_t>, double>& it1, std::pair<std::pair<size_t, size_t>, double>& it2)
    {
        return it1.second > it2.second;
    };
    std::sort(customers_costs.begin(), customers_costs.end(), func);
    for (size_t iter = 0; iter < max_customers; ++iter)
    {
        worst_customers.push_back(customers_costs[iter].first);
    }
    size_t num_relocates = strong ? 2 : solution.size() / 2;
    size_t num_exchanges = strong ? 1 : solution.size() / 5;
    for (auto& c : worst_customers)
    {
        if (TryApplyRandomRelocate(problem, solution, solution[c.first], c.second))
        {
            num_relocates--;
        }
        if (num_relocates == 0)
            break;
    }
    for (auto& c : worst_customers)
    {
        if (TryApplyRandomExchange(problem, solution, solution[c.first], c.second))
        {
            num_exchanges--;
        }
        if (num_exchanges == 0)
            break;
    }
}

Solution IteratedLocalSearch(Problem& problem, Solution solution)
{
    solution.erase(std::remove_if(solution.begin(), solution.end(), [](Route& r) { return r.size() == 2; }),
        solution.end());
    LocalSearch(problem, solution);
	auto stat = problem.GetSolutionStatus(solution);
	size_t num_iterations;
	if (stat.feasible)
	{
		num_iterations = 2000;
	}
	else
	{
		num_iterations = 5000;
	}
    double best_cost = stat.cost;
    Solution best_solution = solution;
    size_t iters_without_improvement = 0;
    for (size_t i = 0; i < num_iterations; ++i)
    {
        Perturbate(problem, solution, iters_without_improvement>=5);
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
    return best_solution;
}
