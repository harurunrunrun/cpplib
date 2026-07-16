// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/dsu/commutative_monoid_dsu.hpp"
#include "../../src/structure/types/monoid.hpp"

constexpr long long sum_operation(long long first, long long second){
    return first + second;
}

constexpr long long sum_identity(){
    return 0;
}

constexpr Monoid<sum_operation, sum_identity> sum_monoid;
using SumDSU = CommutativeMonoidDSU<sum_monoid, 100000>;

void run_direct_tests(){
    SumDSU dsu(std::vector<long long>{4, -7, 10, 0});
    assert(dsu.size() == 4);
    assert(dsu.groups() == 4);
    assert(dsu.component_product(1) == -7);
    assert(dsu.merge(0, 1));
    assert(!dsu.merge(1, 0));
    assert(dsu.same(0, 1));
    assert(!dsu.same(0, 2));
    assert(dsu.component_size(1) == 2);
    assert(dsu.size(0) == 2);
    assert(dsu.component_product(0) == -3);
    assert(dsu.merge(2, 3));
    assert(dsu.merge(1, 3));
    assert(dsu.groups() == 1);
    assert(dsu.component_product(2) == 7);
    assert(dsu.leader(0) == dsu.leader(3));

    SumDSU identities(3);
    assert(identities.component_product(2) == 0);

    bool constructor_threw = false;
    try{
        SumDSU invalid(100001);
    }catch(const std::runtime_error&){
        constructor_threw = true;
    }
    assert(constructor_threw);

    bool vertex_threw = false;
    try{
        (void) dsu.component_product(4);
    }catch(const std::runtime_error&){
        vertex_threw = true;
    }
    assert(vertex_threw);
}

int main(){
    int vertex_count, query_count;
    if(!(std::cin >> vertex_count >> query_count)){
        run_direct_tests();
        return 0;
    }
    std::vector<long long> values(vertex_count);
    for(auto& value: values) std::cin >> value;
    SumDSU dsu(values);
    while(query_count--){
        std::string type;
        std::cin >> type;
        if(type == "M"){
            int first, second;
            std::cin >> first >> second;
            std::cout << dsu.merge(first, second) << '\n';
        }else if(type == "S"){
            int first, second;
            std::cin >> first >> second;
            std::cout << dsu.same(first, second) << '\n';
        }else if(type == "Z"){
            int vertex;
            std::cin >> vertex;
            std::cout << dsu.component_size(vertex) << '\n';
        }else if(type == "P"){
            int vertex;
            std::cin >> vertex;
            std::cout << dsu.component_product(vertex) << '\n';
        }else if(type == "G"){
            std::cout << dsu.groups() << '\n';
        }else{
            std::cout << dsu.size() << '\n';
        }
    }
}

