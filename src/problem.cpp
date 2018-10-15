#include "problem.h"
#include "utils.h"
#include <math.h>
#include <set>
#include <iostream>

void Problem::AddCustomer(size_t index, double x, double y, uint32_t demand, uint32_t ready_time, uint32_t due_time, uint32_t service_time)
{
    Customer c = { index, { x, y }, 0, demand, ready_time, due_time, service_time };
    m_customers.push_back(c);
    m_coords.push_back({x, y});
    m_demands.push_back(demand);
    m_ready_times.push_back(ready_time);
    m_due_times.push_back(due_time);
    m_service_times.push_back(service_time);
}

void Problem::ComputeDistances()
{
    m_distance_matrix.resize(GetCustomersNum());
    for (auto& d : m_distance_matrix) 
    {
        d.resize(GetCustomersNum());
    }

    for (size_t c1 = 0; c1 < GetCustomersNum(); ++c1) {
        for (size_t c2 = 0; c2 < GetCustomersNum(); ++c2) {
            const auto& coord1 = m_coords[c1];
            const auto& coord2 = m_coords[c2];
            auto dx = coord1.x - coord2.x;
            auto dy = coord1.y - coord2.y;

            m_distance_matrix[c1][c2] = sqrt(dx * dx + dy * dy);
        }
    }
    for (size_t c = 0; c < m_customers.size(); ++c)
    {
        m_customers[c].distance_from_depot = m_distance_matrix[0][c];
    }
    m_distance_matrix_guided = m_distance_matrix;
}

bool Problem::IsFeasible(const Route& route, bool need_to_check_capacity) const
{
    if (route.empty())
    {
        return true;
    }
    double time = 0.0;
    size_t prev = 0;
    uint32_t capacityLeft = GetVehicleCapacity();

    DEBUG_CHECK(route.front() == 0);
    DEBUG_CHECK(route.back() == 0);

    for (size_t customer : route) 
    {
        if (need_to_check_capacity)
        {
            if (GetDemand(customer) > capacityLeft)
            {
                return false;
            }
            else
            {
                capacityLeft -= GetDemand(customer);
            }
        }
        double curr_dist = m_distance_matrix[prev][customer];
        time += curr_dist;

        prev = customer;

        if (time > GetDueTime(customer)) 
        {
            return false;
        }

        if (time < GetReadyTime(customer)) 
        {
            time = GetReadyTime(customer);
        }
        time += GetServiceTime(customer);
    }
    return true;
}

uint32_t Problem::GetTotalDemand(const Route & route) const
{
    uint32_t demand = 0;
    for (auto c : route)
    {
        demand += GetDemand(c);
    }
    return demand;
}

double Problem::GetDistance(const Route & route) const
{
    double distance = 0.0;
    size_t prev = 0;
    for (size_t customer : route)
    {
        distance += m_distance_matrix[prev][customer];
        prev = customer;
    }
    return distance;
}

double Problem::GetBeginTime(const Route & route, size_t index, size_t added_customer) const
{
    double time = 0.0;
    size_t prev = 0;
    size_t i = 0;
    for (; i < index; ++i)
    {
        double curr_dist = m_distance_matrix[prev][route[i]];
        time += curr_dist;
        if (GetReadyTime(route[i]) > time)
        {
            time = GetReadyTime(route[i]);
        }
        time += GetServiceTime(prev);
    }
    if (added_customer != -1)
    {
        time += m_distance_matrix[route[i]][added_customer];
        if (GetReadyTime(added_customer) > time)
        {
            time = GetReadyTime(added_customer);
        }
    }
    if (GetReadyTime(route[index]) > time)
    {
        time = GetReadyTime(route[index]);
    }
    return time;
}


Problem::SolutionStatus Problem::GetSolutionStatus(const Solution& solution) const
{
    SolutionStatus status;
    if (solution.size() >= m_max_vehicles)
    {
        return{};
    }
    for (const Route& route : solution) 
    {
        if (!IsFeasible(route))
        {
            return {};
        }
        double route_cost = GetDistance(route);
        status.cost += route_cost;
        status.route_costs.push_back(route_cost);
    }
    status.feasible = true;
    return status;
}

bool Problem::ValidateSolution(const Solution& solution) const
{
    std::set<size_t> visited_customers; 

    for (const Route& route : solution) 
    {
        if (route.empty()) {
            continue;
        }
        if (route.front() != 0)
        {
            DEBUG_CHECK(false);
            std::cout << "Starting customer is not depot!!!";
            return false;
        }
        if (route.back() != 0) 
        {
            DEBUG_CHECK(false);
            std::cout << "The last customer is not depot!!!";
            return false;
        }

        for (size_t customer = 0; customer < route.size(); ++customer) 
        {
            size_t c = route[customer];

            if (c!=DEPOT && visited_customers.count(c)) 
            {
                DEBUG_CHECK(false);
                std::cout << "Customer already visited:" << c;
                return false;
            }

            visited_customers.insert(c);

            if (c == DEPOT && customer != DEPOT && customer != route.size() - 1) 
            {
                DEBUG_CHECK(false);
                std::cout << "Depot is inside of the route";
                return false;
            }
        }
    }

    if (!GetSolutionStatus(solution).feasible) 
    {
        DEBUG_CHECK(false);
        std::cout << "Infeasible solution";
        return false;
    }

    if (visited_customers.size() != m_customers.size())
    {
        DEBUG_CHECK(false);
        std::cout << "There are unvisited customers";
        return false;
    }

    return true;
}
