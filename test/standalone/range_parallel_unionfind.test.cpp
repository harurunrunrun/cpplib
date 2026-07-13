// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/structure/dsu/range_parallel_unionfind.hpp"

struct NaiveDSU{
    std::vector<int> parent;
    std::vector<int> size;

    explicit NaiveDSU(int n): parent(static_cast<std::size_t>(n)), size(static_cast<std::size_t>(n), 1){
        std::iota(parent.begin(), parent.end(), 0);
    }
    int leader(int vertex){
        if(parent[static_cast<std::size_t>(vertex)] == vertex) return vertex;
        return parent[static_cast<std::size_t>(vertex)] = leader(parent[static_cast<std::size_t>(vertex)]);
    }
    bool merge(int left, int right){
        left = leader(left);
        right = leader(right);
        if(left == right) return false;
        if(size[static_cast<std::size_t>(left)] < size[static_cast<std::size_t>(right)]) std::swap(left, right);
        parent[static_cast<std::size_t>(right)] = left;
        size[static_cast<std::size_t>(left)] += size[static_cast<std::size_t>(right)];
        return true;
    }
};

void test_random(){
    constexpr int n = 80;
    RangeParallelUnionFind<128> dsu(n);
    NaiveDSU naive(n);
    std::vector<long long> sum(static_cast<std::size_t>(n));
    std::iota(sum.begin(), sum.end(), 1LL);
    std::mt19937 rng(2026071402);
    for(int step = 0; step < 3000; ++step){
        const int length = static_cast<int>(rng() % (n + 1));
        const int first = static_cast<int>(rng() % (n - length + 1));
        const int second = static_cast<int>(rng() % (n - length + 1));
        int callback_count = 0;
        const int merged = dsu.merge_ranges(
            first, first + length, second, second + length,
            [&](int root, int absorbed){
                sum[static_cast<std::size_t>(root)] += sum[static_cast<std::size_t>(absorbed)];
                ++callback_count;
            }
        );
        int expected_merged = 0;
        for(int offset = 0; offset < length; ++offset){
            expected_merged += naive.merge(first + offset, second + offset);
        }
        assert(merged == expected_merged);
        assert(callback_count == expected_merged);
        for(int left = 0; left < n; ++left){
            assert(dsu.component_size(left) == naive.size[static_cast<std::size_t>(naive.leader(left))]);
            for(int right = 0; right < n; ++right){
                assert(dsu.same(left, right) == (naive.leader(left) == naive.leader(right)));
            }
        }
    }
    assert(dsu.groups() == 1);
    assert(sum[static_cast<std::size_t>(dsu.leader(0))] == n * (n + 1LL) / 2);
}

void test_errors(){
    RangeParallelUnionFind<16> dsu(10);
    bool thrown = false;
    try{ dsu.merge_ranges(0, 4, 2, 5); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ dsu.merge_ranges(-1, 2, 0, 3); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    assert(dsu.merge_ranges(3, 3, 8, 8) == 0);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, query_count;
    if(!(std::cin >> n >> query_count)){
        test_random();
        test_errors();
        return 0;
    }
    std::vector<long long> component_sum(static_cast<std::size_t>(n));
    for(long long& value: component_sum) std::cin >> value;
    RangeParallelUnionFind<200000> dsu(n);
    long long pair_sum = 0;
    while(query_count-- > 0){
        int length, first, second;
        std::cin >> length >> first >> second;
        const int merged = dsu.merge_ranges(
            first, first + length, second, second + length,
            [&](int root, int absorbed){
                pair_sum += component_sum[static_cast<std::size_t>(root)] *
                    component_sum[static_cast<std::size_t>(absorbed)];
                component_sum[static_cast<std::size_t>(root)] +=
                    component_sum[static_cast<std::size_t>(absorbed)];
            }
        );
        std::cout << merged << ' ' << dsu.groups() << ' ' << pair_sum << '\n';
    }
}
