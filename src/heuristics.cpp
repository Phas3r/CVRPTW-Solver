#include "heuristics.h"
#include <set>
#include <algorithm>
#include <functional>
#include <numeric>

void CalculateBestPlaceInRoute(Problem& problem, std::vector<size_t>& unrouted_customers, Route& route, std::vector<size_t>& best_places, std::vector<double>& best_costs, double alpha1)
{
    for (auto c : unrouted_customers)
    {
        size_t best_position;
        double best_cost = std::numeric_limits<double>::infinity();
        for (size_t curr_position = 1; curr_position<route.size(); ++curr_position)
        {            
            double cost11 = problem.GetDistance(c, route[curr_position])+ problem.GetDistance(route[curr_position - 1], c)-problem.GetDistance(route[curr_position - 1], route[curr_position]);
            double cost12 = problem.GetBeginTime(route, curr_position, c) - problem.GetBeginTime(route, curr_position);
            double cost = alpha1*cost11 + (1 - alpha1)*cost12;
            if (cost < best_cost)
            {
                best_cost = cost;
                best_position = curr_position;
            }
        }
        best_places[c] = best_position;
        best_costs[c] = best_cost;
    }
}
Solution FindSolutionFromInsertionTypeOneHeuristic(Problem & problem, double alpha, double lambda)
{
    Solution solution;
    std::vector<size_t> unrouted_customers(problem.GetCustomersNum()-1);

    std::iota(unrouted_customers.begin(), unrouted_customers.end(), 1);
    Route current_route = { 0, 0 };
    while (unrouted_customers.size())
    {
        uint32_t capacity_left = problem.GetVehicleCapacity() - problem.GetTotalDemand(current_route);
        bool can_insert_more = false;
        for (auto c : unrouted_customers)
        {
            if (problem.GetDemand(c) <= capacity_left)
            {
                can_insert_more = true;
                break;
            }
        }
        if (!can_insert_more)
        {
            solution.push_back(current_route);
            current_route = { 0, 0 };
        }
        std::vector<size_t> best_places(problem.GetCustomersNum(), -1);
        std::vector<double> best_costs(problem.GetCustomersNum(), std::numeric_limits<double>::infinity());
        CalculateBestPlaceInRoute(problem, unrouted_customers, current_route, best_places, best_costs, alpha);
        auto compare_savings = [&](size_t customer1, size_t customer2)
        {
            return (lambda*problem.GetDistance(0, customer1) - best_costs[customer1]) > (lambda*problem.GetDistance(0, customer2) - best_costs[customer2]);
        };
        std::sort(unrouted_customers.begin(), unrouted_customers.end(), compare_savings);
        Route old_route = current_route;
        bool changed = false;
        for (size_t i = 0; i < unrouted_customers.size(); ++i)
        {
            DEBUG_CHECK(best_places[unrouted_customers[i]] != -1);
            current_route.insert(current_route.begin() + best_places[unrouted_customers[i]], unrouted_customers[i]);
            if (problem.IsFeasible(current_route))
            {
                changed = true;
                unrouted_customers.erase(unrouted_customers.begin() + i);
                break;
            }
            else
            {
                current_route = old_route;
            }
        }
        if (!changed)
        {
            solution.push_back(current_route);
            current_route = { 0, 0 };
        }
    }
    solution.push_back(current_route);
    return solution;
}

Solution FindSolutionFromInsertionTypeThreeHeuristic(Problem & problem)
{
    Solution solution;
    /*std::vector<size_t> unrouted_customers(problem.GetCustomersNum() - 1);

    std::iota(unrouted_customers.begin(), unrouted_customers.end(), 1);
    Route current_route = { 0, 0 };
    while (unrouted_customers.size())
    {
        uint32_t capacity_left = problem.GetVehicleCapacity() - problem.GetTotalDemand(current_route);
        bool can_insert_more = false;
        for (auto c : unrouted_customers)
        {
            if (problem.GetDemand(c) <= capacity_left)
            {
                can_insert_more = true;
                break;
            }
        }
        if (!can_insert_more)
        {
            solution.push_back(current_route);
            current_route = { 0, 0 };
        }
        std::vector<size_t> best_places(problem.GetCustomersNum(), -1);
        std::vector<double> best_costs(problem.GetCustomersNum(), std::numeric_limits<double>::infinity());
        CalculateBestPlaceInRoute(problem, unrouted_customers, current_route, best_places, best_costs, 0.9);
        auto compare_savings = [&](size_t customer1, size_t customer2)
        {
            return best_costs[customer1] < best_costs[customer2];
        };
        std::sort(unrouted_customers.begin(), unrouted_customers.end(), compare_savings);
        Route old_route = current_route;
        bool changed = false;
        for (size_t i = 0; i < unrouted_customers.size(); ++i)
        {
            DEBUG_CHECK(best_places[unrouted_customers[i]] != -1);
            current_route.insert(current_route.begin() + best_places[unrouted_customers[i]], unrouted_customers[i]);
            if (problem.IsFeasible(current_route))
            {
                changed = true;
                unrouted_customers.erase(unrouted_customers.begin() + i);
                break;
            }
            else
            {
                current_route = old_route;
            }
        }
        if (!changed)
        {
            solution.push_back(current_route);
            current_route = { 0, 0 };
        }
    }
    solution.push_back(current_route);*/
    return solution;
}

void CalculateCosts(Problem& problem, std::vector<size_t>& unrouted_customers, Route& route, std::vector<double>& costs, double delta1, double delta2, double delta3)
{
    size_t last_customer_index = route.size() - 2;
    size_t last_customer = route[last_customer_index];
    for (auto c : unrouted_customers)
    {
        double Bj = problem.GetBeginTime(route, last_customer_index) + problem.GetDistance(last_customer, c);
        if (problem.GetReadyTime(c) > Bj)
        {
            Bj = problem.GetReadyTime(c);
        }
        costs[c] = delta1 * problem.GetDistance(last_customer, c)
            + delta2*(Bj - problem.GetBeginTime(route, last_customer_index) - problem.GetServiceTime(last_customer))
            + delta3*(problem.GetDueTime(c) - problem.GetBeginTime(route, last_customer_index) - problem.GetServiceTime(last_customer) - problem.GetDistance(last_customer, c));
    }
}

Solution FindSolutionFromTimeOrientedNearestNeighborHeuristic(Problem & problem, double delta1, double delta2, double delta3)
{
    Solution solution;
    std::vector<size_t> unrouted_customers(problem.GetCustomersNum() - 1);

    std::iota(unrouted_customers.begin(), unrouted_customers.end(), 1);
    Route current_route = { 0, 0 };
    while (unrouted_customers.size())
    {
        uint32_t capacity_left = problem.GetVehicleCapacity() - problem.GetTotalDemand(current_route);
        bool can_insert_more = false;
        for (auto c : unrouted_customers)
        {
            if (problem.GetDemand(c) <= capacity_left)
            {
                can_insert_more = true;
                break;
            }
        }
        if (!can_insert_more)
        {
            solution.push_back(current_route);
            current_route = { 0, 0 };
        }

        std::vector<double> costs(problem.GetCustomersNum(), std::numeric_limits<double>::infinity());
        CalculateCosts(problem, unrouted_customers, current_route, costs, delta1, delta2, delta3);
        auto compare_cost2 = [&](size_t customer1, size_t customer2)
        {
            return  costs[customer1] < costs[customer2];
        };
        std::sort(unrouted_customers.begin(), unrouted_customers.end(), compare_cost2);
        if (unrouted_customers.size() > 1)
        {
            DEBUG_CHECK(costs[unrouted_customers[0]] < costs[unrouted_customers[1]]);
        }
        Route old_route = current_route;
        bool changed = false;
        for (size_t i = 0; i < unrouted_customers.size(); ++i)
        {
            current_route.insert(current_route.end() - 1, unrouted_customers[i]);
            if (problem.IsFeasible(current_route))
            {
                changed = true;
                unrouted_customers.erase(unrouted_customers.begin() + i);
                break;
            }
            else
            {
                current_route = old_route;
            }
        }
        if (!changed)
        {
            solution.push_back(current_route);
            current_route = { 0, 0 };
        }
    }
    solution.push_back(current_route);
    return solution;
}
