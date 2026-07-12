// competitive-verifier: STANDALONE

#include <cassert>
#include <stdexcept>
#include <vector>

#include "../../src/structure/other/fenwick_tree_2d.hpp"

void test_basic(){
    FenwickTree2D<long long, 4, 5> fw(std::vector<std::vector<long long>>{
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
    });

    assert(fw.height() == 3);
    assert(fw.width() == 4);
    assert(!fw.empty());
    assert(fw.prefix_sum(0, 0) == 0);
    assert(fw.prefix_sum(3, 4) == 78);
    assert(fw.sum(0, 0, 3, 4) == 78);
    assert(fw.sum(1, 1, 3, 3) == 34);
    assert(fw.get(2, 3) == 12);

    fw.add(1, 2, 10);
    assert(fw.get(1, 2) == 17);
    assert(fw.sum(1, 1, 3, 3) == 44);

    fw.set(0, 0, -5);
    assert(fw.get(0, 0) == -5);
    assert(fw.sum(0, 0, 1, 1) == -5);
    assert(fw.sum(0, 0, 3, 4) == 82);
}

void test_empty(){
    FenwickTree2D<int, 3, 4> fw(0, 4);
    assert(fw.height() == 0);
    assert(fw.width() == 4);
    assert(fw.empty());
    assert(fw.prefix_sum(0, 0) == 0);
}

void test_exceptions(){
    bool thrown = false;
    try{
        FenwickTree2D<int, 2, 2> fw(3, 2);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        FenwickTree2D<int, 3, 3> fw(std::vector<std::vector<int>>{{1, 2}, {3}});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    FenwickTree2D<int, 2, 2> fw(2, 2);
    thrown = false;
    try{
        fw.add(2, 0, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)fw.sum(1, 0, 0, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    test_basic();
    test_empty();
    test_exceptions();
}
