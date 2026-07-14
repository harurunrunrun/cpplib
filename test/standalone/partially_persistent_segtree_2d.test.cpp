// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/segtree/partially_persistent_segtree_2d.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AddMonoid<long long> partial_sum_2d{};

long long partial_naive_prod(
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

void test_versions(){
    PartiallyPersistentSegtree2D<partial_sum_2d, 4, 4, 8> seg(
        std::vector<std::vector<long long>>{{1, 2, 3}, {4, 5, 6}}
    );
    assert(seg.height() == 2 && seg.width() == 3 && !seg.empty());
    assert(seg.versions() == 1 && seg.latest_version() == 0);
    const int first = seg.set(0, 1, 20);
    const int second = seg.apply(1, 2, 10);
    assert(first == 1 && second == 2);
    assert(seg.versions() == 3 && seg.latest_version() == second);
    assert(seg.all_prod(0) == 21);
    assert(seg.all_prod(1) == 39);
    assert(seg.all_prod() == 49);
    assert(seg.get(0, 0, 1) == 2);
    assert(seg.get(1, 0, 1) == 20);
    assert(seg.get(1, 2) == 16);
    assert(seg.prod(0, 0, 1, 2, 3) == 16);
    assert(seg.prod(1, 0, 1, 2, 3) == 34);
}

void test_random(){
    std::mt19937 random(20260714);
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

        PartiallyPersistentSegtree2D<partial_sum_2d, 4, 5, 80> seg(initial);
        std::vector<std::vector<std::vector<long long>>> versions = {initial};
        assert(seg.height() == height && seg.width() == width && !seg.empty());
        assert(seg.versions() == 1 && seg.latest_version() == 0);
        for(int operation = 0; operation < 80; ++operation){
            const int row = static_cast<int>(random() % static_cast<unsigned>(height));
            const int col = static_cast<int>(random() % static_cast<unsigned>(width));
            const long long value = static_cast<int>(random() % 41U) - 20;
            auto next = versions.back();
            next[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)] = value;
            assert(seg.set(row, col, value) == operation + 1);
            versions.push_back(std::move(next));
            assert(seg.versions() == static_cast<int>(versions.size()));
            assert(seg.latest_version() == operation + 1);

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
                partial_naive_prod(
                    versions[static_cast<std::size_t>(version)],
                    top,
                    left,
                    bottom,
                    right
                ));
            assert(seg.prod(top, left, bottom, right) ==
                partial_naive_prod(versions.back(), top, left, bottom, right));
        }
    }
}

void test_capacity_and_boundaries(){
    PartiallyPersistentSegtree2D<partial_sum_2d, 2, 2, 1> limited(2, 2);
    assert(limited.set(0, 0, 3) == 1);
    bool thrown = false;
    try{
        (void)limited.set(1, 1, 5);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    assert(limited.versions() == 2);
    assert(limited.latest_version() == 1 && limited.height() == 2 && limited.width() == 2);
    assert(limited.all_prod(0) == 0 && limited.all_prod(1) == 3);

    thrown = false;
    try{
        (void)limited.get(2, 0, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    PartiallyPersistentSegtree2D<partial_sum_2d, 2, 2, 0> empty;
    assert(empty.empty() && empty.all_prod() == 0);
    assert(empty.height() == 0 && empty.width() == 0);
    assert(empty.versions() == 1 && empty.latest_version() == 0);
}

void self_test(){
    test_versions();
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
    PartiallyPersistentSegtree2D<partial_sum_2d, 8, 8, 512> seg(values);
    while(query_count--){
        std::string type;
        std::cin >> type;
        if(type == "SET" || type == "ADD"){
            int row, col;
            long long value;
            std::cin >> row >> col >> value;
            std::cout << (type == "SET" ?
                seg.set(row, col, value) :
                seg.apply(row, col, value)) << '\n';
        }else if(type == "GET"){
            int version, row, col;
            std::cin >> version >> row >> col;
            std::cout << seg.get(version, row, col) << '\n';
        }else if(type == "LATEST_GET"){
            int row, col;
            std::cin >> row >> col;
            std::cout << seg.get(row, col) << '\n';
        }else if(type == "PROD"){
            int version, top, left, bottom, right;
            std::cin >> version >> top >> left >> bottom >> right;
            std::cout << seg.prod(version, top, left, bottom, right) << '\n';
        }else if(type == "LATEST_PROD"){
            int top, left, bottom, right;
            std::cin >> top >> left >> bottom >> right;
            std::cout << seg.prod(top, left, bottom, right) << '\n';
        }else if(type == "ALL"){
            int version;
            std::cin >> version;
            std::cout << seg.all_prod(version) << '\n';
        }else if(type == "LATEST_ALL"){
            std::cout << seg.all_prod() << '\n';
        }else if(type == "VERSIONS"){
            std::cout << seg.versions() << ' ' << seg.latest_version() << '\n';
        }
    }
}
