#include "utils.h"
#include <sstream>
#include <fstream>
#include <algorithm>

bool ReadFile(std::string filename, Problem& problem)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        DEBUG_CHECK(false);
        std::cout << "Could not open file!!!";
        return false;
    }
    std::string line;
    if (std::getline(file, line))
        problem.SetInstanceName(line);
    else
        return false;
    while (std::getline(file, line)) 
    {
        if (line.find("NUMBER") != std::string::npos) {
            break;
        }
    }

    uint32_t num_vehicles = 0;
    uint32_t vehicle_capacity = 0;
    std::getline(file, line);
    {
        std::stringstream ss(line);
        ss >> num_vehicles >> vehicle_capacity;
        if (ss.fail())
        {
            std::cout << "Invalid header";
            return false;
        }
    }

    problem.SetMaxVehicles(num_vehicles);
    problem.SetVehicleCapacity(vehicle_capacity);

    while (std::getline(file, line))
    {
        if (std::any_of(line.begin(), line.end(), ::isdigit))
        {
            std::stringstream ss(line);
            uint32_t id = 0;
            double x = 0.0;
            double y = 0.0;
            uint32_t demand = 0;
            uint32_t ready_time = 0;
            uint32_t due_time = 0;
            uint32_t service_time;
            ss >> id >> x >> y >> demand >> ready_time >> due_time >> service_time;
            if (ss.fail())
            {
                std::cout << "Error reading file";
                return false;
            }
            problem.AddCustomer(id, x, y, demand, ready_time, due_time, service_time);
        }
        else
        {
            continue;
        }

    }
    problem.ComputeDistances();
    return true;
}

bool WriteSolutionToFile(const Solution & solution, const Problem & problem)
{
    std::string filename = problem.GetInstanceName() + ".sol";
    std::ofstream file(filename);
    for (const Route& route : solution)
    {
        if (route.size() <= 2) 
        {
            continue;
        }
        double time_elapsed = 0.0;
        size_t prev = DEPOT;

        for (auto customer : route)
        {
            file << customer << " ";
            if (customer == DEPOT && prev == DEPOT)
            {
                file << "0 ";
                continue;
            }

            time_elapsed += problem.GetDistance(prev, customer);

            prev = customer;

            if (time_elapsed < problem.GetReadyTime(customer))
            {
                time_elapsed = problem.GetReadyTime(customer);
            }

            file << time_elapsed << " ";

            time_elapsed += problem.GetServiceTime(customer);
        }

        file << std::endl;
    }
    return true;
}

bool WriteResultToFile(const Solution & solution, const Problem & problem)
{
    std::string filename = problem.GetInstanceName() + ".result";
    std::ofstream file(filename);
    for (const Route& route : solution)
    {
        if (route.size() <= 2)
        {
            continue;
        }
        double time_elapsed = 0.0;
        size_t prev = DEPOT;

        for (auto customer : route)
        {
            file << customer << " ";
        }

        file << std::endl;
    }
    auto stat = problem.GetSolutionStatus(solution);
    file << stat.cost<<std::endl;
    return true;
}
