#include "ils.h"
#include "gls.h"
#include "utils.h"
#include <algorithm>
#include "heuristics.h"
#include <thread>


int main(int argc, char* argv[])
{

    if (argc < 2) {
        std::cout << "No instance provided";
        return 1;
    }
    

    Problem problem;
    bool result = ReadFile(argv[1], problem);
    if (!result)
    {
        std::cout << "Failed to read file:" << argv[1];
        return 2;
    }


    std::vector<std::pair<Solution, double>> solutions;

    Problem::SolutionStatus stat, stat2, stat3, stat4;
    Solution sol;
    Solution sol2;
	Solution sol3;
	Solution sol4;
    auto thread_func1 = [&]()
    {
        Solution sol_init = FindSolutionFromInsertionTypeOneHeuristic(problem, 0.9, 2.5);
        stat = problem.GetSolutionStatus(sol_init);
		sol = GuidedLocalSearch(problem, sol_init);
    };
    auto thread_func2 = [&]()
    {
        Solution sol_init2 = FindSolutionFromTimeOrientedNearestNeighborHeuristic(problem, 0.95, 0, 0.05);
		stat2 = problem.GetSolutionStatus(sol_init2);
		sol2 = GuidedLocalSearch(problem, sol_init2);
    };
	auto thread_func3 = [&]()
	{
		Solution sol_init3 = FindSolutionFromInsertionTypeOneHeuristic(problem, 0.8, 1.5);
		stat3 = problem.GetSolutionStatus(sol_init3);
		sol3 = GuidedLocalSearch(problem, sol_init3);
	};
	auto thread_func4 = [&]()
	{
		Solution sol_init4 = FindSolutionFromTimeOrientedNearestNeighborHeuristic(problem, 0.6, 0.1, 0.3);
		stat4 = problem.GetSolutionStatus(sol_init4);
		sol4 = GuidedLocalSearch(problem, sol_init4);
	};
    std::thread thread1(thread_func1);
    std::thread thread2(thread_func2);
	std::thread thread3(thread_func3);
	std::thread thread4(thread_func4);
    thread1.join();
    thread2.join();
	thread3.join();
	thread4.join();
	stat = problem.GetSolutionStatus(sol);
	stat2 = problem.GetSolutionStatus(sol2);
	stat3 = problem.GetSolutionStatus(sol3);
	stat4 = problem.GetSolutionStatus(sol4);
	if (stat.feasible)
		solutions.push_back(std::make_pair(sol, stat.cost));
	if (stat2.feasible)
		solutions.push_back(std::make_pair(sol2, stat2.cost));
	if (stat3.feasible)
		solutions.push_back(std::make_pair(sol3, stat3.cost));
	if (stat4.feasible)
		solutions.push_back(std::make_pair(sol4, stat4.cost));
    
    std::pair<Solution, double> best_solution = *std::min_element(solutions.begin(), solutions.end(), [](std::pair<Solution, double>& sol1, std::pair<Solution, double>& sol2)
    {
        return sol1.second < sol2.second;
    });
    
    if (!problem.ValidateSolution(best_solution.first))
    {
        DEBUG_CHECK(false);
        std::cout << "Invalid final solution!";
    }
    std::cout << "FINAL COST:" << best_solution.second << std::endl;

    WriteSolutionToFile(best_solution.first, problem);
    WriteResultToFile(best_solution.first, problem);
    return 0;
}
