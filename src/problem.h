#pragma once

#include <stdint.h>
#include <vector>
#include <cstdint>
#include <cstddef>

typedef std::vector<size_t> Route;
typedef std::vector<Route>  Solution;
typedef std::pair<size_t, size_t> Feature;

#define DEPOT 0

class Problem
{
public:
    size_t     GetCustomersNum()		       const { return m_demands.size(); }
    uint32_t   GetVehiclesNum()			       const { return m_max_vehicles; }
    uint32_t   GetVehicleCapacity()		       const { return m_capacity; }
    uint32_t   GetDemand(size_t customer)      const { return m_demands[customer]; }
    uint32_t   GetReadyTime(size_t customer)   const { return m_ready_times[customer]; }
    uint32_t   GetDueTime(size_t customer)     const { return m_due_times[customer]; }
    uint32_t   GetServiceTime(size_t customer) const { return m_service_times[customer]; }
    uint32_t   GetTotalDemand(const Route& route) const;
    double     GetDistance(const Route& route) const;
    double     GetBeginTime(const Route& route, size_t index, size_t added_customer = -1) const;
    void ResetMatrix()
    {
        m_distance_matrix_guided = m_distance_matrix;
    }
    double GetDistance(size_t customer1, size_t customer2) const
    {
        return m_distance_matrix_guided[customer1][customer2];
    }
    double Get“NormalDistance(size_t customer1, size_t customer2) const
    {
        return m_distance_matrix[customer1][customer2];
    }
    void UpdateMatrix(Feature feature)
    {
        m_distance_matrix_guided[feature.first][feature.second] += lambda*m_distance_matrix[feature.first][feature.second];
    }

    const std::string GetInstanceName() const { return m_instance_name; }
    void SetInstanceName(std::string instance_name) { m_instance_name = instance_name; }
    void SetMaxVehicles(uint32_t num_vehicles) { m_max_vehicles = num_vehicles; }
    void SetVehicleCapacity(uint32_t vehicleCapacity) { m_capacity = vehicleCapacity; }

    void AddCustomer(size_t index, double x, double y, uint32_t demand, uint32_t ready_time, uint32_t due_time, uint32_t service_time);
    void ComputeDistances();

    struct Coord
    {
        double x;
        double y;
    };

    struct Customer
    {
        size_t index;
        Coord coords;
        double distance_from_depot;
        uint32_t demand;
        uint32_t ready_time;
        uint32_t due_time;
        uint32_t service_time;
    };

    bool IsFeasible(const Route& route, bool need_to_check_capacity = true) const;
    bool ValidateSolution(const Solution& solution) const;


    struct SolutionStatus
    {
        bool feasible = false;
        double cost = 0.0;
        std::vector<double> route_costs;
    };
    SolutionStatus GetSolutionStatus(const Solution& solution) const;

protected:
    std::string m_instance_name;
    uint32_t m_max_vehicles = 0;
    uint32_t m_capacity = 0;

    double lambda = 0.2;
    std::vector<std::vector<double>> m_distance_matrix_guided;
    std::vector<std::vector<double>> m_distance_matrix;
    std::vector<Customer> m_customers;
    
    std::vector<Coord>    m_coords;
    std::vector<uint32_t> m_demands;
    std::vector<uint32_t> m_ready_times;
    std::vector<uint32_t> m_due_times;
    std::vector<uint32_t> m_service_times;
};
