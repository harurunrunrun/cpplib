// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/segtree/rollback_segtree_2d.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AddMonoid<long long> rollback_sum_2d{};

long long rollback_naive_prod(
    const std::vector<std::vector<long long>>& values,
    int row_begin,
    int col_begin,
    int row_end,
    int col_end
){
    long long result = 0;
    for(int row = row_begin; row < row_end; ++row){
        for(int col = col_begin; col < col_end; ++col){
            result += values[static_cast<std::size_t>(row)]
                            [static_cast<std::size_t>(col)];
        }
    }
    return result;
}

void test_rollback(){
    RollbackSegtree2D<rollback_sum_2d, 4, 4, 8> seg(
        std::vector<std::vector<long long>>{{1, 2, 3}, {4, 5, 6}}
    );
    const int initial = seg.snapshot();
    seg.set(0, 1, 20);
    const int first = seg.snapshot();
    seg.apply(1, 2, 10);
    assert(initial == 0 && first == 1 && seg.snapshot() == 2);
    assert(seg.all_prod() == 49);
    seg.undo();
    assert(seg.all_prod() == 39 && seg.get(1, 2) == 6);
    seg.set(1, 2, -4);
    assert(seg.all_prod() == 29);
    seg.rollback(initial);
    assert(seg.all_prod() == 21 && !seg.can_undo());
}

void test_random(){
    std::mt19937 random(20260715);
    for(int iteration = 0; iteration < 80; ++iteration){
        const int height = 1 + static_cast<int>(random() % 4U);
        const int width = 1 + static_cast<int>(random() % 5U);
        std::vector<std::vector<long long>> initial(
            static_cast<std::size_t>(height),
            std::vector<long long>(static_cast<std::size_t>(width))
        );
        for(auto& row : initial){
            for(long long& value : row){
                value = static_cast<int>(random() % 31U) - 15;
            }
        }

        RollbackSegtree2D<rollback_sum_2d, 4, 5, 80> seg(initial);
        std::vector<std::vector<std::vector<long long>>> states = {initial};
        for(int operation = 0; operation < 300; ++operation){
            const unsigned choice = random() % 10U;
            if(choice < 6 || states.size() == 1){
                const int row = static_cast<int>(random() % static_cast<unsigned>(height));
                const int col = static_cast<int>(random() % static_cast<unsigned>(width));
                const long long value = static_cast<int>(random() % 41U) - 20;
                auto next = states.back();
                next[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] = value;
                seg.set(row, col, value);
                states.push_back(std::move(next));
            }else if(choice < 8){
                seg.undo();
                states.pop_back();
            }else{
                const int target = static_cast<int>(
                    random() % static_cast<unsigned>(states.size())
                );
                seg.rollback(target);
                states.resize(static_cast<std::size_t>(target + 1));
            }
            assert(seg.snapshot() == static_cast<int>(states.size()) - 1);
            int top = static_cast<int>(random() % static_cast<unsigned>(height + 1));
            int bottom = static_cast<int>(random() % static_cast<unsigned>(height + 1));
            int left = static_cast<int>(random() % static_cast<unsigned>(width + 1));
            int right = static_cast<int>(random() % static_cast<unsigned>(width + 1));
            if(top > bottom) std::swap(top, bottom);
            if(left > right) std::swap(left, right);
            assert(seg.prod(top, left, bottom, right) ==
                rollback_naive_prod(states.back(), top, left, bottom, right));
        }
    }
}

void test_capacity_and_boundaries(){
    RollbackSegtree2D<rollback_sum_2d, 2, 2, 1> limited(2, 2);
    limited.set(0, 0, 3);
    bool thrown = false;
    try{
        limited.set(1, 1, 5);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    assert(limited.history_size() == 1 && limited.all_prod() == 3);
    limited.undo();
    limited.set(1, 1, 5);
    assert(limited.all_prod() == 5);

    thrown = false;
    try{
        limited.rollback(2);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    RollbackSegtree2D<rollback_sum_2d, 2, 2, 0> empty;
    assert(empty.empty() && empty.all_prod() == 0);
    thrown = false;
    try{
        empty.undo();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void self_test(){
    test_rollback();
    test_random();
    test_capacity_and_boundaries();
}

int main(){
    int height, width, query_count;
    if(!(std::cin >> height >> width >> query_count)){
        self_test();
        return 0;
    }
    std::vector<std::vector<long long>> values(
        static_cast<std::size_t>(height),
        std::vector<long long>(static_cast<std::size_t>(width))
    );
    for(auto& row : values){
        for(long long& value : row) std::cin >> value;
    }
    RollbackSegtree2D<rollback_sum_2d, 8, 8, 512> seg(values);
    while(query_count--){
        std::string type;
        std::cin >> type;
        if(type == "SET" || type == "ADD"){
            int row, col;
            long long value;
            std::cin >> row >> col >> value;
            if(type == "SET") seg.set(row, col, value);
            else seg.apply(row, col, value);
        }else if(type == "GET"){
            int row, col;
            std::cin >> row >> col;
            std::cout << seg.get(row, col) << '\n';
        }else if(type == "PROD"){
            int top, left, bottom, right;
            std::cin >> top >> left >> bottom >> right;
            std::cout << seg.prod(top, left, bottom, right) << '\n';
        }else if(type == "ALL"){
            std::cout << seg.all_prod() << '\n';
        }else if(type == "SNAPSHOT"){
            std::cout << seg.snapshot() << '\n';
        }else if(type == "UNDO"){
            seg.undo();
        }else if(type == "ROLLBACK"){
            int snapshot;
            std::cin >> snapshot;
            seg.rollback(snapshot);
        }
    }
}
