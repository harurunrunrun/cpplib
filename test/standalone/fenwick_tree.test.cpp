// competitive-verifier: STANDALONE

#include "../../src/structure/fenwick_tree/fenwick_tree_2d.hpp"
#include <cassert>
#include <iostream>
#include <cassert>
#include "../../src/structure/fenwick_tree/range_add_point_get.hpp"
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/fenwick_tree/fenwick_tree.hpp"

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

    thrown = false;
    try{
        FenwickTree<int, 3> fw(-1);
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

    volatile int out_of_range_prefix = 4;
    thrown = false;
    try{ (void)fw.prefix_sum(out_of_range_prefix); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{ (void)fw.get(-1); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{ fw.set(3, 1); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        FenwickTree<int, 3> too_many(std::vector<int>{1, 2, 3, 4});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    {
        FenwickTree<bool> tree(std::vector<bool>{false, true, false});
        assert(tree.prefix_sum(3)); tree.add(0, true); assert(tree.prefix_sum(1));
        FenwickTree2D<bool> grid(std::vector<std::vector<bool>>{{false, true}, {false, false}});
        assert(grid.prefix_sum(2, 2)); grid.add(1, 0, true); assert(grid.prefix_sum(2, 1));
    }

    // Dynamic wrappers: runtime capacities.
    {
        RangeAddPointGet<long long> values(129); values.add(1, 129, 7); values.add(64, 129, -3);
        assert(values.get(0) == 0 && values.get(63) == 7 && values.get(128) == 4);
    }
    {FenwickTree<long long> f(1001); f.add(1000,7); f.add(0,-2); assert(f.sum(0,1001)==5); assert(f.get(1000)==7); FenwickTree<long long> z(0); assert(z.prefix_sum(0)==0);}

    int n, q;
    if(std::cin >> n >> q){
        std::vector<long long> a(static_cast<std::size_t>(n));
        for(auto& x: a) std::cin >> x;
        FenwickTree<long long, 256> fw(a);
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "ADD"){
                int k;
                long long x;
                std::cin >> k >> x;
                fw.add(k, x);
            }else if(type == "SET"){
                int k;
                long long x;
                std::cin >> k >> x;
                fw.set(k, x);
            }else if(type == "GET"){
                int k;
                std::cin >> k;
                std::cout << fw.get(k) << '\n';
            }else if(type == "SUM"){
                int l, r;
                std::cin >> l >> r;
                std::cout << fw.sum(l, r) << '\n';
            }else if(type == "PREFIX"){
                int r;
                std::cin >> r;
                std::cout << fw.prefix_sum(r) << '\n';
            }
        }
        return 0;
    }

    test_basic();
    test_empty();
    test_exceptions();
}
