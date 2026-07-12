// competitive-verifier: STANDALONE

#include <cassert>
#include <stdexcept>
#include <vector>

#include "../../src/structure/other/fenwick_tree.hpp"

void test_basic(){
    FenwickTree<long long, 8> fw(std::vector<long long>{1, 2, 3, 4, 5});
    assert(fw.size() == 5);
    assert(!fw.empty());
    assert(fw.prefix_sum(0) == 0);
    assert(fw.prefix_sum(5) == 15);
    assert(fw.sum(1, 4) == 9);
    assert(fw.get(2) == 3);

    fw.add(2, 7);
    assert(fw.get(2) == 10);
    assert(fw.sum(0, 5) == 22);

    fw.set(4, -5);
    assert(fw.get(4) == -5);
    assert(fw.sum(3, 5) == -1);
}

void test_empty(){
    FenwickTree<int, 4> fw(0);
    assert(fw.size() == 0);
    assert(fw.empty());
    assert(fw.prefix_sum(0) == 0);
}

void test_exceptions(){
    bool thrown = false;
    try{
        FenwickTree<int, 3> fw(4);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    FenwickTree<int, 3> fw(3);
    thrown = false;
    try{
        fw.add(3, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)fw.sum(2, 1);
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
