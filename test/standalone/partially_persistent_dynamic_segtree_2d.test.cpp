// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/structure/segtree/partially_persistent_dynamic_segtree_2d.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AddMonoid<long long> partially_persistent_dynamic_2d_sum{};
using Tree = PartiallyPersistentDynamicSegtree2D<
    partially_persistent_dynamic_2d_sum, 1000000000039LL, 1000000000007LL,
    10000, 300000, 512, 300000
>;
static Tree tree;
using Point = std::pair<long long, long long>;
using State = std::map<Point, long long>;

long long rectangle_sum(const State& state, long long rb, long long cb,
                        long long re, long long ce){
    long long result = 0;
    for(const auto& [point, value]: state){
        if(rb <= point.first && point.first < re && cb <= point.second && point.second < ce)
            result += value;
    }
    return result;
}

void test_random(){
    std::mt19937_64 rng(2026071302);
    std::vector<State> versions(1);
    for(int turn = 0; turn < 90; ++turn){
        const long long row = static_cast<long long>(rng() % 1000000);
        const long long col = static_cast<long long>(rng() % 1000000);
        State next = versions.back();
        int version;
        if(turn % 3 == 0){
            const long long delta = static_cast<long long>(rng() % 101) - 50;
            next[{row, col}] += delta;
            version = tree.apply(row, col, delta);
        }else{
            const long long value = static_cast<long long>(rng() % 201) - 100;
            next[{row, col}] = value;
            version = tree.set(row, col, value);
        }
        versions.push_back(next);
        assert(version == static_cast<int>(versions.size()) - 1);
        for(int repeat = 0; repeat < 3; ++repeat){
            const int query_version = static_cast<int>(rng() % versions.size());
            long long rb = static_cast<long long>(rng() % 1000001);
            long long re = static_cast<long long>(rng() % 1000001);
            long long cb = static_cast<long long>(rng() % 1000001);
            long long ce = static_cast<long long>(rng() % 1000001);
            if(rb > re) std::swap(rb, re);
            if(cb > ce) std::swap(cb, ce);
            assert(tree.prod(query_version, rb, cb, re, ce) ==
                   rectangle_sum(versions[static_cast<std::size_t>(query_version)], rb, cb, re, ce));
        }
    }
    assert(tree.all_prod(0) == 0 && tree.get(0, 0, 0) == 0);
    assert(tree.row_nodes_used() < 10000 && tree.column_nodes_used() < 300000);
}

void test_boundaries_and_capacity(){
    static PartiallyPersistentDynamicSegtree2D<
        partially_persistent_dynamic_2d_sum, 2, 2, 1, 4, 2, 4
    > too_small;
    bool thrown = false;
    try{ (void)too_small.set(1, 1, 3); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && too_small.versions() == 1 && too_small.row_nodes_used() == 0);

    static PartiallyPersistentDynamicSegtree2D<
        partially_persistent_dynamic_2d_sum, 1, 1, 2, 2, 1, 2
    > limited;
    const int version = limited.set(0, 0, 7);
    thrown = false;
    try{ (void)limited.set(0, 0, 9); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && limited.all_prod(version) == 7 && limited.all_prod(0) == 0);
    thrown = false;
    try{ (void)limited.get(version, -1, 0); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

int main(){
    int query_count;
    if(std::cin >> query_count){
        while(query_count--){
            std::string type;
            std::cin >> type;
            if(type == "SET" || type == "ADD"){
                long long row, col, value;
                std::cin >> row >> col >> value;
                std::cout << (type == "SET" ? tree.set(row, col, value)
                                             : tree.apply(row, col, value)) << '\n';
            }else if(type == "GET"){
                int version;
                long long row, col;
                std::cin >> version >> row >> col;
                std::cout << tree.get(version, row, col) << '\n';
            }else if(type == "PROD"){
                int version;
                long long rb, cb, re, ce;
                std::cin >> version >> rb >> cb >> re >> ce;
                std::cout << tree.prod(version, rb, cb, re, ce) << '\n';
            }else{
                int version;
                std::cin >> version;
                std::cout << tree.all_prod(version) << '\n';
            }
        }
        return 0;
    }
    test_random();
    test_boundaries_and_capacity();
}
