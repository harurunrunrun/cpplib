// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/structure/segtree/rollback_dynamic_segtree_2d.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AddMonoid<long long> rollback_dynamic_2d_sum{};
using Tree = RollbackDynamicSegtree2D<
    rollback_dynamic_2d_sum, 1000000000039LL, 1000000000007LL,
    10000, 300000, 300000
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
    std::mt19937_64 rng(2026071303);
    std::vector<State> history(1);
    for(int turn = 0; turn < 130; ++turn){
        if(turn % 13 == 0 && tree.can_undo()){
            tree.undo();
            history.pop_back();
        }else if(turn % 17 == 0 && history.size() > 2){
            const int target = static_cast<int>(rng() % history.size());
            tree.rollback(target);
            history.resize(static_cast<std::size_t>(target + 1));
        }else{
            const long long row = static_cast<long long>(rng() % 1000000);
            const long long col = static_cast<long long>(rng() % 1000000);
            State next = history.back();
            if(turn % 3 == 0){
                const long long delta = static_cast<long long>(rng() % 101) - 50;
                next[{row, col}] += delta;
                tree.apply(row, col, delta);
            }else{
                const long long value = static_cast<long long>(rng() % 201) - 100;
                next[{row, col}] = value;
                tree.set(row, col, value);
            }
            history.push_back(next);
        }
        assert(tree.snapshot() == static_cast<int>(history.size()) - 1);
        assert(tree.history_size() == tree.snapshot());
        assert(tree.can_undo() == (history.size() > 1));
        long long rb = static_cast<long long>(rng() % 1000001);
        long long re = static_cast<long long>(rng() % 1000001);
        long long cb = static_cast<long long>(rng() % 1000001);
        long long ce = static_cast<long long>(rng() % 1000001);
        if(rb > re) std::swap(rb, re);
        if(cb > ce) std::swap(cb, ce);
        assert(tree.prod(rb, cb, re, ce) == rectangle_sum(history.back(), rb, cb, re, ce));
    }
    tree.rollback(0);
    assert(tree.all_prod() == 0 && tree.row_nodes_used() == 0 && tree.column_nodes_used() == 0);
    assert(tree.history_size() == 0 && tree.snapshot() == 0);
    assert(!tree.can_undo());
}

void test_boundaries_and_capacity(){
    static RollbackDynamicSegtree2D<rollback_dynamic_2d_sum, 2, 2, 1, 4, 4> too_small;
    bool thrown = false;
    try{ too_small.set(1, 1, 3); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && too_small.snapshot() == 0 && too_small.row_nodes_used() == 0);
    assert(too_small.history_size() == 0 && !too_small.can_undo());
    assert(too_small.column_nodes_used() == 0);

    static RollbackDynamicSegtree2D<rollback_dynamic_2d_sum, 1, 1, 2, 2, 2> small;
    small.set(0, 0, 7);
    const auto snapshot = small.snapshot();
    thrown = false;
    try{ small.set(1, 0, 9); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && small.snapshot() == snapshot && small.all_prod() == 7);
    assert(small.history_size() == snapshot && small.can_undo());
    assert(small.row_nodes_used() > 0 && small.column_nodes_used() > 0);
    small.undo();
    assert(!small.can_undo() && small.all_prod() == 0);
    thrown = false;
    try{ small.undo(); }catch(const std::runtime_error&){ thrown = true; }
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
                if(type == "SET") tree.set(row, col, value);
                else tree.apply(row, col, value);
            }else if(type == "GET"){
                long long row, col;
                std::cin >> row >> col;
                std::cout << tree.get(row, col) << '\n';
            }else if(type == "PROD"){
                long long rb, cb, re, ce;
                std::cin >> rb >> cb >> re >> ce;
                std::cout << tree.prod(rb, cb, re, ce) << '\n';
            }else if(type == "ALL"){
                std::cout << tree.all_prod() << '\n';
            }else if(type == "SNAPSHOT"){
                std::cout << tree.snapshot() << '\n';
            }else if(type == "UNDO"){
                tree.undo();
            }else{
                int target;
                std::cin >> target;
                tree.rollback(target);
            }
        }
        return 0;
    }
    test_random();
    test_boundaries_and_capacity();
}
