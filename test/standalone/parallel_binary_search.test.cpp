// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/other/parallel_binary_search.hpp"

std::vector<int> solve_thresholds(
    const std::vector<long long>& targets,
    const std::vector<long long>& increments
){
    long long current = 0;
    return parallel_binary_search(
        static_cast<int>(targets.size()),
        static_cast<int>(increments.size()),
        [&]{ current = 0; },
        [&](int operation){
            current += increments[static_cast<std::size_t>(operation)];
        },
        [&](int query){
            return current >= targets[static_cast<std::size_t>(query)];
        }
    );
}

std::vector<int> naive_thresholds(
    const std::vector<long long>& targets,
    const std::vector<long long>& increments
){
    std::vector<long long> prefix(increments.size() + 1, 0);
    for(std::size_t i = 0; i < increments.size(); ++i){
        prefix[i + 1] = prefix[i] + increments[i];
    }
    std::vector<int> result;
    result.reserve(targets.size());
    for(long long target: targets){
        const auto iterator = std::lower_bound(prefix.begin(), prefix.end(), target);
        if(iterator == prefix.end()){
            result.push_back(static_cast<int>(increments.size()) + 1);
        }else{
            result.push_back(static_cast<int>(iterator - prefix.begin()));
        }
    }
    return result;
}

void self_test(){
    assert((solve_thresholds({-5, 0, 1, 3, 6, 7}, {1, 2, 3})
        == std::vector<int>{0, 0, 1, 2, 3, 4}));
    assert((solve_thresholds({-1, 0, 1}, {}) == std::vector<int>{0, 0, 1}));

    int callback_count = 0;
    const auto empty = parallel_binary_search(
        0,
        1000000000,
        [&]{ ++callback_count; },
        [&](int){ ++callback_count; },
        [&](int){ ++callback_count; return false; }
    );
    assert(empty.empty());
    assert(callback_count == 0);

    bool reset_seen = false;
    int applied_count = 0;
    int last_predicate_query = -1;
    const std::vector<int> first_true = {0, 3, 5, 8, 9};
    const auto ordered = parallel_binary_search(
        static_cast<int>(first_true.size()),
        8,
        [&]{
            reset_seen = true;
            applied_count = 0;
            last_predicate_query = -1;
        },
        [&](int operation){
            assert(reset_seen);
            assert(operation == applied_count);
            ++applied_count;
            last_predicate_query = -1;
        },
        [&](int query){
            assert(reset_seen);
            assert(last_predicate_query < query);
            last_predicate_query = query;
            return applied_count >= first_true[static_cast<std::size_t>(query)];
        }
    );
    assert(ordered == first_true);

    bool invalid_called = false;
    for(const auto [queries, operations]: std::vector<std::pair<int, int>>{
        {-1, 0}, {0, -1}, {1, std::numeric_limits<int>::max()}
    }){
        bool thrown = false;
        try{
            (void)parallel_binary_search(
                queries,
                operations,
                [&]{ invalid_called = true; },
                [&](int){ invalid_called = true; },
                [&](int){ invalid_called = true; return false; }
            );
        }catch(const std::invalid_argument&){
            thrown = true;
        }
        assert(thrown);
    }
    assert(!invalid_called);

    bool propagated = false;
    try{
        (void)parallel_binary_search(
            1,
            1,
            []{ throw std::runtime_error("reset failure"); },
            [](int){},
            [](int){ return false; }
        );
    }catch(const std::runtime_error&){
        propagated = true;
    }
    assert(propagated);

    std::mt19937_64 random(2026071401);
    for(int iteration = 0; iteration < 2000; ++iteration){
        const int query_count = static_cast<int>(random() % 50);
        const int operation_count = static_cast<int>(random() % 50);
        std::vector<long long> increments(static_cast<std::size_t>(operation_count));
        long long total = 0;
        for(long long& value: increments){
            value = static_cast<long long>(random() % 20);
            total += value;
        }
        std::vector<long long> targets(static_cast<std::size_t>(query_count));
        for(long long& target: targets){
            target = static_cast<long long>(random() % static_cast<unsigned long long>(total + 31)) - 10;
        }
        assert(solve_thresholds(targets, increments)
            == naive_thresholds(targets, increments));
    }

    constexpr int large_size = 300000;
    std::vector<long long> increments(static_cast<std::size_t>(large_size), 1);
    std::vector<long long> targets(static_cast<std::size_t>(large_size));
    for(int i = 0; i < large_size; ++i) targets[static_cast<std::size_t>(i)] = i;
    targets.front() = -1;
    targets.back() = large_size + 1LL;
    const auto large = solve_thresholds(targets, increments);
    assert(large.size() == targets.size());
    for(int i = 0; i + 1 < large_size; ++i){
        assert(large[static_cast<std::size_t>(i)] == (i == 0 ? 0 : i));
    }
    assert(large.back() == large_size + 1);
}

int main(){
    int query_count;
    int operation_count;
    if(!(std::cin >> query_count >> operation_count)){
        self_test();
        return 0;
    }
    std::vector<long long> targets(static_cast<std::size_t>(query_count));
    std::vector<long long> increments(static_cast<std::size_t>(operation_count));
    for(long long& target: targets) std::cin >> target;
    for(long long& increment: increments) std::cin >> increment;

    const auto answer = solve_thresholds(targets, increments);
    for(int query = 0; query < query_count; ++query){
        if(query != 0) std::cout << ' ';
        std::cout << answer[static_cast<std::size_t>(query)];
    }
    std::cout << '\n';
}
