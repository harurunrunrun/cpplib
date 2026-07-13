// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/segtree/persistent_segtree_2d.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AddMonoid<long long> persistent_sum_2d{};

long long naive_prod(
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

void test_branching(){
    PersistentSegtree2D<persistent_sum_2d, 4, 4, 8> seg(
        std::vector<std::vector<long long>>{{1, 2, 3}, {4, 5, 6}}
    );
    const int first = seg.set(0, 0, 1, 20);
    const int second = seg.apply(0, 1, 2, 10);
    const int copied = seg.fork(first);
    assert(first == 1 && second == 2 && copied == 3);
    assert(seg.all_prod(0) == 21);
    assert(seg.all_prod(first) == 39);
    assert(seg.all_prod(second) == 31);
    assert(seg.all_prod(copied) == 39);
    assert(seg.get(0, 0, 1) == 2);
    assert(seg.get(first, 0, 1) == 20);
    assert(seg.prod(second, 1, 1, 2, 3) == 21);
}

void test_random(){
    std::mt19937 random(20260713);
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

        PersistentSegtree2D<persistent_sum_2d, 4, 5, 100> seg(initial);
        std::vector<std::vector<std::vector<long long>>> versions = {initial};
        for(int operation = 0; operation < 80; ++operation){
            const int source = static_cast<int>(
                random() % static_cast<unsigned>(versions.size())
            );
            if(random() % 5U == 0){
                const int version = seg.fork(source);
                versions.push_back(versions[static_cast<std::size_t>(source)]);
                assert(version == static_cast<int>(versions.size()) - 1);
            }else{
                const int row = static_cast<int>(random() % static_cast<unsigned>(height));
                const int col = static_cast<int>(random() % static_cast<unsigned>(width));
                const long long value = static_cast<int>(random() % 41U) - 20;
                auto next = versions[static_cast<std::size_t>(source)];
                next[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] = value;
                const int version = seg.set(source, row, col, value);
                versions.push_back(std::move(next));
                assert(version == static_cast<int>(versions.size()) - 1);
            }

            const int version = static_cast<int>(
                random() % static_cast<unsigned>(versions.size())
            );
            int top = static_cast<int>(random() % static_cast<unsigned>(height + 1));
            int bottom = static_cast<int>(random() % static_cast<unsigned>(height + 1));
            int left = static_cast<int>(random() % static_cast<unsigned>(width + 1));
            int right = static_cast<int>(random() % static_cast<unsigned>(width + 1));
            if(top > bottom) std::swap(top, bottom);
            if(left > right) std::swap(left, right);
            assert(seg.prod(version, top, left, bottom, right) ==
                naive_prod(
                    versions[static_cast<std::size_t>(version)],
                    top,
                    left,
                    bottom,
                    right
                ));
            const int row = static_cast<int>(random() % static_cast<unsigned>(height));
            const int col = static_cast<int>(random() % static_cast<unsigned>(width));
            assert(seg.get(version, row, col) ==
                versions[static_cast<std::size_t>(version)]
                        [static_cast<std::size_t>(row)]
                        [static_cast<std::size_t>(col)]);
        }
    }
}

void test_capacity_and_boundaries(){
    PersistentSegtree2D<persistent_sum_2d, 2, 2, 2> limited(2, 2);
    assert(limited.set(0, 0, 0, 1) == 1);
    assert(limited.fork(0) == 2);
    bool thrown = false;
    try{
        (void)limited.set(1, 1, 1, 2);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    assert(limited.versions() == 3);
    assert(limited.all_prod(1) == 1);

    thrown = false;
    try{
        (void)limited.prod(0, -1, 0, 1, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    PersistentSegtree2D<persistent_sum_2d, 2, 2, 0> no_update;
    assert(no_update.empty() && no_update.all_prod(0) == 0);
    thrown = false;
    try{
        (void)no_update.set(0, 0, 0, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void self_test(){
    test_branching();
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
    PersistentSegtree2D<persistent_sum_2d, 8, 8, 512> seg(values);
    while(query_count--){
        std::string type;
        std::cin >> type;
        if(type == "SET" || type == "ADD"){
            int version, row, col;
            long long value;
            std::cin >> version >> row >> col >> value;
            std::cout << (type == "SET" ?
                seg.set(version, row, col, value) :
                seg.apply(version, row, col, value)) << '\n';
        }else if(type == "FORK"){
            int version;
            std::cin >> version;
            std::cout << seg.fork(version) << '\n';
        }else if(type == "GET"){
            int version, row, col;
            std::cin >> version >> row >> col;
            std::cout << seg.get(version, row, col) << '\n';
        }else if(type == "PROD"){
            int version, top, left, bottom, right;
            std::cin >> version >> top >> left >> bottom >> right;
            std::cout << seg.prod(version, top, left, bottom, right) << '\n';
        }else if(type == "ALL"){
            int version;
            std::cin >> version;
            std::cout << seg.all_prod(version) << '\n';
        }else if(type == "VERSIONS"){
            std::cout << seg.versions() << ' ' << seg.latest_version() << '\n';
        }
    }
}
