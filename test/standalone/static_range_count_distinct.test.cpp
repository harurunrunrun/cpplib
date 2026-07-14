// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <set>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/range/static_range_count_distinct.hpp"

struct ConstantHash{
    std::size_t operator()(int) const{ return 0; }
};

int naive_count(const std::vector<int>& values, int left, int right){
    return static_cast<int>(std::set<int>(
        values.begin() + left, values.begin() + right
    ).size());
}

void test_random(){
    std::mt19937 rng(2026071452);
    for(int iteration = 0; iteration < 500; ++iteration){
        const int n = static_cast<int>(rng() % 120);
        std::vector<int> values(static_cast<std::size_t>(n));
        for(int& value: values) value = static_cast<int>(rng() % 31) - 15;
        StaticRangeCountDistinct<int, 128, ConstantHash> structure(values);
        assert(structure.size() == n);
        for(int query = 0; query < 500; ++query){
            int left = static_cast<int>(rng() % (n + 1));
            int right = static_cast<int>(rng() % (n + 1));
            if(left > right) std::swap(left, right);
            assert(structure.count(left, right) == naive_count(values, left, right));
            assert(structure.count_distinct(left, right) == naive_count(values, left, right));
        }
    }
}

void test_large_and_errors(){
    constexpr int n = 200000;
    std::vector<int> values(static_cast<std::size_t>(n));
    for(int index = 0; index < n; ++index) values[static_cast<std::size_t>(index)] = index % 12345;
    StaticRangeCountDistinct<int, n> structure(values);
    assert(structure.count(0, n) == 12345);
    assert(structure.count(1000, 1000) == 0);
    assert(structure.count(n - 100, n) == 100);

    bool thrown = false;
    try{ (void)structure.count(-1, 3); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)structure.count(4, 3); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)structure.count(0, n + 1); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        [[maybe_unused]] StaticRangeCountDistinct<int, 2> too_large({1, 2, 3});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, query_count;
    if(!(std::cin >> n >> query_count)){
        test_random();
        test_large_and_errors();
        return 0;
    }
    std::vector<int> values(static_cast<std::size_t>(n));
    for(int& value: values) std::cin >> value;
    StaticRangeCountDistinct<int, 200000> structure(values);
    while(query_count-- > 0){
        int left, right;
        std::cin >> left >> right;
        std::cout << structure.count(left, right) << '\n';
    }
}
