// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/other/xor_basis.hpp"

void test_basic(){
    XorBasis<unsigned, 8> basis;
    assert(basis.empty());
    assert(basis.insert(3));
    assert(basis.insert(5));
    assert(!basis.insert(6));
    assert(basis.rank() == 2);
    assert(basis.contains(0));
    assert(basis.contains(3));
    assert(basis.contains(5));
    assert(basis.contains(6));
    assert(!basis.contains(1));
    assert(basis.maximize() == 6);
    assert(basis.minimize(7) == 1);
    assert(basis.kth_smallest(0) == 0);
    assert(basis.kth_smallest(1) == 3);
    assert(basis.kth_smallest(2) == 5);
    assert(basis.kth_smallest(3) == 6);

    XorBasis<unsigned, 8> other;
    other.insert(1);
    basis.merge(other);
    assert(basis.rank() == 3);
    assert(basis.maximize() == 7);

    bool thrown = false;
    try{
        (void)basis.kth_smallest(8);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)basis.insert(1U << 8);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    basis.clear();
    assert(basis.empty());
}

int main(){
    int query_count;
    if(std::cin >> query_count){
        XorBasis<unsigned long long, 16> basis;
        while(query_count--){
            std::string type;
            unsigned long long value;
            std::cin >> type;
            if(type == "INSERT"){
                std::cin >> value;
                std::cout << basis.insert(value) << '\n';
            }else if(type == "CONTAINS"){
                std::cin >> value;
                std::cout << basis.contains(value) << '\n';
            }else if(type == "MAX"){
                std::cin >> value;
                std::cout << basis.maximize(value) << '\n';
            }else if(type == "MIN"){
                std::cin >> value;
                std::cout << basis.minimize(value) << '\n';
            }else if(type == "KTH"){
                std::cin >> value;
                std::cout << basis.kth_smallest(value) << '\n';
            }else if(type == "RANK"){
                std::cout << basis.rank() << '\n';
            }else if(type == "CLEAR"){
                basis.clear();
            }
        }
        return 0;
    }
    test_basic();
}
