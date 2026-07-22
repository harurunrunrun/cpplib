// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/matching/bipartite/assignment.hpp"

long long brute_assignment(const std::vector<std::vector<long long>>& cost){
    const int rows = static_cast<int>(cost.size());
    const int columns = rows == 0 ? 0 : static_cast<int>(cost[0].size());
    std::vector<int> selected(static_cast<std::size_t>(columns), 0);
    std::fill(selected.begin(), selected.begin() + rows, 1);
    std::sort(selected.begin(), selected.end(), std::greater<int>());
    long long answer = std::numeric_limits<long long>::max();
    do{
        std::vector<int> chosen;
        for(int column = 0; column < columns; ++column){
            if(selected[static_cast<std::size_t>(column)]) chosen.push_back(column);
        }
        std::sort(chosen.begin(), chosen.end());
        do{
            long long current = 0;
            for(int row = 0; row < rows; ++row){
                current += cost[static_cast<std::size_t>(row)]
                    [static_cast<std::size_t>(chosen[static_cast<std::size_t>(row)])];
            }
            answer = std::min(answer, current);
        }while(std::next_permutation(chosen.begin(), chosen.end()));
    }while(std::prev_permutation(selected.begin(), selected.end()));
    return rows == 0 ? 0 : answer;
}

void verify_result(
    const std::vector<std::vector<long long>>& cost,
    const AssignmentResult<long long>& result
){
    const int rows = static_cast<int>(cost.size());
    const int columns = rows == 0 ? 0 : static_cast<int>(cost[0].size());
    std::vector<int> used(static_cast<std::size_t>(columns), 0);
    long long total = 0;
    for(int row = 0; row < rows; ++row){
        const int column = result.row_to_column[static_cast<std::size_t>(row)];
        assert(column >= 0 && column < columns);
        assert(!used[static_cast<std::size_t>(column)]++);
        assert(result.column_to_row[static_cast<std::size_t>(column)] == row);
        total += cost[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)];
        assert(result.row_potential[static_cast<std::size_t>(row)] +
                   result.column_potential[static_cast<std::size_t>(column)] ==
               cost[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)]);
    }
    for(int row = 0; row < rows; ++row){
        for(int column = 0; column < columns; ++column){
            assert(result.row_potential[static_cast<std::size_t>(row)] +
                       result.column_potential[static_cast<std::size_t>(column)] <=
                   cost[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)]);
        }
    }
    assert(total == result.cost);
}

void test_random(){
    std::mt19937 rng(2026071404);
    for(int iteration = 0; iteration < 700; ++iteration){
        const int rows = static_cast<int>(rng() % 7);
        const int columns = rows + static_cast<int>(rng() % (8 - rows));
        std::vector<std::vector<long long>> cost(
            static_cast<std::size_t>(rows),
            std::vector<long long>(static_cast<std::size_t>(columns))
        );
        for(auto& row: cost){
            for(long long& value: row) value = static_cast<int>(rng() % 41) - 20;
        }
        const auto result = minimum_cost_assignment(cost);
        verify_result(cost, result);
        assert(result.cost == brute_assignment(cost));
    }
}

void test_large_and_errors(){
    constexpr int n = 180;
    std::vector<std::vector<long long>> cost(
        n, std::vector<long long>(n)
    );
    for(int row = 0; row < n; ++row){
        for(int column = 0; column < n; ++column){
            const long long difference = row - column;
            cost[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)] =
                difference * difference - 1000000000LL;
        }
    }
    const auto result = minimum_cost_assignment(cost);
    verify_result(cost, result);
    assert(result.cost == -1000000000LL * n);

    bool thrown = false;
    try{
        (void)minimum_cost_assignment(std::vector<std::vector<int>>{{1}, {2}});
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{
        (void)minimum_cost_assignment(std::vector<std::vector<int>>{{1, 2}, {3}});
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int rows, columns;
    if(!(std::cin >> rows >> columns)){
        test_random();
        test_large_and_errors();
        return 0;
    }
    std::vector<std::vector<long long>> cost(
        static_cast<std::size_t>(rows),
        std::vector<long long>(static_cast<std::size_t>(columns))
    );
    for(auto& row: cost){
        for(long long& value: row) std::cin >> value;
    }
    std::cout << minimum_cost_assignment(cost).cost << '\n';
}
