#include "procedures.h"
#include <numeric>
#include <algorithm>

// INTRA ROUTE MOVES


//The edges (i, i+1) and (j, j+1) are replaced by edges (i, j) and (i+1, j + 1), 
//thus reversing the direction of customers between i + 1 and j
bool ProcedureTwoOpt(Problem& problem, Route& route, size_t index1, size_t index2, double& gain, bool revert)
{
    size_t min_index = std::min(index1, index2);
    size_t max_index = std::max(index1, index2);
    if (!revert)
    {
        gain = 0.0;


        size_t customer_before_reverse = route[min_index - 1];
        size_t reverse_start = route[min_index];

        size_t reverse_end = route[max_index];
        size_t customer_after_reverse = route[max_index + 1];

        gain += problem.GetDistance(customer_before_reverse, reverse_start);
        gain += problem.GetDistance(reverse_end, customer_after_reverse);

        gain -= problem.GetDistance(customer_before_reverse, reverse_end);
        gain -= problem.GetDistance(reverse_start, customer_after_reverse);


        if (gain < 0.0001)
        {
            return false;
        }
    }
    std::reverse(route.begin() + min_index, route.begin() + max_index + 1);
    return true;
}


//Customers i and i+1 are relocated to be served between two customers j and j + 1 instead of customers i - 1 and i + 2
bool ProcedureOrOpt(Problem & problem, Route & route, size_t index1, size_t index2, double & gain, bool revert)
{
    if (revert)
    {
        if (index1 < index2)
        {
            index2 -= 2;
        }

        size_t customer1 = route[index2 + 1];
        size_t succ_customer1 = route[index2 + 2];

        route.erase(route.begin() + index2 + 1, route.begin() + index2 + 3);

        route.insert(route.begin() + index1, { customer1, succ_customer1 });
        return true;
    }
    if (index1 == index2 || index1 == (index2+1) || index1 == (index2 - 1))
    {
        return false;
    }
    gain = 0.0;

    size_t pred_customer1 = route[index1 - 1];
    size_t customer1 = route[index1];
    size_t succ_customer1 = route[index1 + 1];
    size_t succ_succ_customer1 = route[index1 + 2];

    size_t customer2 = route[index2];
    size_t succ_customer2 = route[index2 + 1];

    gain += problem.GetDistance(pred_customer1, customer1);
    gain += problem.GetDistance(succ_customer1, succ_succ_customer1);

    gain -= problem.GetDistance(pred_customer1, succ_succ_customer1);

    gain += problem.GetDistance(customer2, succ_customer2);

    gain -= problem.GetDistance(customer2, customer1);
    gain -= problem.GetDistance(succ_customer1, succ_customer2);


    if (gain < 0.0001)
    {
        return false;
    }
    if (index1 < index2) {
        index2 -= 2;
    }

    route.erase(route.begin() + index1, route.begin() + index1 + 2);

    route.insert(route.begin() + index2 + 1, { customer1, succ_customer1 });
    return true;
}

// INTER ROUTE MOVES

bool ProcedureRelocate(Problem & problem, Route & route1, Route & route2, size_t index1, size_t index2, double & gain, bool forced, bool revert)
{
    if (revert)
    {
        if (&route1 == &route2 && index1 < index2) {
            --index2;
        }
        size_t customer1 = route2[index2 + 1];
        route2.erase(route2.begin() + index2 + 1);
        route1.insert(route1.begin() + index1, customer1);
        return true;
    }
    if (&route1 == &route2 && (index1 == index2 || index1 == (index2 + 1) || index1 == (index2 - 1)))
    {
        return false;
    }

    size_t customer1 = route1[index1];
    if (!forced)
    {
        gain = 0.0;

        size_t pred_customer1 = route1[index1 - 1];
        size_t succ_customer1 = route1[index1 + 1];


        size_t customer2 = route2[index2];
        size_t succ_customer2 = route2[index2 + 1];

        gain += problem.GetDistance(pred_customer1, customer1);
        gain += problem.GetDistance(customer1, succ_customer1);

        gain -= problem.GetDistance(pred_customer1, succ_customer1);

        gain += problem.GetDistance(customer2, succ_customer2);

        gain -= problem.GetDistance(customer2, customer1);
        gain -= problem.GetDistance(customer1, succ_customer2);


        if (gain < 0.0001)
        {
            return false;
        }
    }
    if (&route1 == &route2 && index1 < index2) {
        --index2;
    }
    route1.erase(route1.begin() + index1);
    route2.insert(route2.begin() + index2 + 1, customer1);
    return true;
}

bool ProcedureExchange(Problem & problem, Route & route1, Route & route2, size_t index1, size_t index2, double & gain, bool forced, bool revert)
{
    if (revert)
    {
        std::swap(route1[index1], route2[index2]);
        return true;
    }
    if (&route1 == &route2 && index1 == index2)
    {
        return false;
    }
    if (!forced)
    {
        size_t min_index = std::min(index1, index2);
        size_t max_index = std::max(index1, index2);
        gain = 0.0;
        if (&route1 == &route2 && (max_index - min_index) == 1)
        {
            size_t pred_customer1 = route1[min_index - 1];
            size_t customer1 = route1[min_index];
            size_t customer2 = route2[max_index];
            size_t succ_customer2 = route2[max_index + 1];
            gain += problem.GetDistance(pred_customer1, customer1);
            gain += problem.GetDistance(customer2, succ_customer2);
            gain -= problem.GetDistance(pred_customer1, customer2);
            gain -= problem.GetDistance(customer1, succ_customer2);
        }
        else
        {
            size_t pred_customer1 = route1[index1 - 1];
            size_t customer1 = route1[index1];
            size_t succ_customer1 = route1[index1 + 1];
            size_t pred_customer2 = route2[index2 - 1];
            size_t customer2 = route2[index2];
            size_t succ_customer2 = route2[index2 + 1];

            gain += problem.GetDistance(pred_customer1, customer1);
            gain += problem.GetDistance(customer1, succ_customer1);
            gain += problem.GetDistance(pred_customer2, customer2);
            gain += problem.GetDistance(customer2, succ_customer2);
            gain -= problem.GetDistance(pred_customer1, customer2);
            gain -= problem.GetDistance(customer2, succ_customer1);
            gain -= problem.GetDistance(pred_customer2, customer1);
            gain -= problem.GetDistance(customer1, succ_customer2);
        }

        if (gain < 0.0001 || forced)
        {
            return false;
        }
    }
    std::swap(route1[index1], route2[index2]);
    return true;
}

//Segments (i, k) on the upper route and (j, l) on the lower route are
//simultaneously reinserted into the lower and upper routes, respectively.
bool ProcedureCrossExchange(Problem & problem, Route & route1, Route & route2, size_t index1, size_t index2, size_t segment_size1, size_t segment_size2, double & gain, bool revert)
{
    if (revert)
    {
        auto segment1_start = route2.begin() + index2;
        auto segment1_end = route2.begin() + index2 + segment_size1;
        auto segment2_start = route1.begin() + index1;

        std::swap_ranges(segment1_start, segment1_end, segment2_start);

        if (segment_size1 == segment_size2)
        {
            return true;
        }

        auto segment1_start_in_route2 = route1.begin() + index1 + segment_size1;
        auto segment1_end_in_route2 = route1.begin() + index1 + segment_size2;

        route2.insert(segment1_end, segment1_start_in_route2, segment1_end_in_route2);
        route1.erase(segment1_start_in_route2, segment1_end_in_route2);

        return true;
    }

    if (&route1 == &route2)
    {
        return false;
    }

    if (segment_size2 < segment_size1)
    {
        return false;
    }

    if (segment_size1 < 2 && segment_size2 < 2)
    {
        return false;
    }

    if ((index1 + segment_size1 == route1.size()) != (index2 + segment_size2 == route2.size()))
    {
        return false;
    }

    if (index1 + segment_size1 > route1.size())
    {
        return false;
    }

    if (index2 + segment_size2 > route2.size())
    {
        return false;
    }

    gain = 0.0;
    size_t prev_customer1 = route1[index1 - 1];
    size_t customer1 = route1[index1];

    size_t prev_customer2 = route2[index2 - 1];
    size_t customer2 = route2[index2];

    gain += problem.GetDistance(prev_customer1, customer1);
    gain += problem.GetDistance(prev_customer2, customer2);

    gain -= problem.GetDistance(prev_customer1, customer2);
    gain -= problem.GetDistance(prev_customer2, customer1);

    if (index1 + segment_size1 != route1.size()) {
        size_t segment1_end = route1[index1 + segment_size1 - 1];
        size_t after_segment1 = route1[index1 + segment_size1];

        size_t segment2_end = route2[index2 + segment_size2 - 1];
        size_t after_segment2 = route2[index2 + segment_size2];


        gain += problem.GetDistance(segment1_end, after_segment1);
        gain += problem.GetDistance(segment2_end, after_segment2);

        gain -= problem.GetDistance(segment1_end, after_segment2);
        gain -= problem.GetDistance(segment2_end, after_segment1);
    }
    if (gain < 0.0001)
    {
        return false;
    }
    auto segment1_start = route1.begin() + index1;
    auto segment1_end = route1.begin() + index1 + segment_size1;
    auto segment2_start = route2.begin() + index2;

    std::swap_ranges(segment1_start, segment1_end, segment2_start);

    if (segment_size1 == segment_size2) {
        return true;
    }

    auto segment1_start_in_route2 = route2.begin() + index2 + segment_size1;
    auto segment1_end_in_route2 = route2.begin() + index2 + segment_size2;

    route1.insert(segment1_end, segment1_start_in_route2, segment1_end_in_route2);
    route2.erase(segment1_start_in_route2, segment1_end_in_route2);

    return true;
}
