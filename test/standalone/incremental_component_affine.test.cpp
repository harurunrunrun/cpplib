// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/dsu/incremental_component_affine.hpp"
#include "../../src/structure/types/monoid_act_len.hpp"

struct AffineAction{
    long long multiplier = 1;
    long long addend = 0;
};

constexpr long long sum_operation(
    long long first,
    long long,
    long long second,
    long long
){
    return first + second;
}

constexpr long long sum_identity(){ return 0; }

constexpr long long affine_mapping(
    AffineAction action,
    long long sum,
    long long length
){
    return action.multiplier * sum + action.addend * length;
}

constexpr AffineAction affine_composition(
    AffineAction newer,
    AffineAction older
){
    return {
        newer.multiplier * older.multiplier,
        newer.multiplier * older.addend + newer.addend,
    };
}

constexpr AffineAction affine_identity(){ return {}; }

constexpr Monoid_Act_Len<
    sum_operation,
    sum_identity,
    affine_mapping,
    affine_composition,
    affine_identity
> sum_affine_action;

using AffineComponents =
    IncrementalComponentAffine<sum_affine_action, 100000>;

void run_direct_tests(){
    AffineComponents components(std::vector<long long>{1, 2, 3, 4});
    components.apply_component(0, {2, 1});
    assert(components.vertex_value(0) == 3);
    assert(components.vertex_value(1) == 2);
    assert(components.add_edge(0, 1));
    components.apply_component(1, {3, -2});
    assert(components.vertex_value(0) == 7);
    assert(components.vertex_value(1) == 4);
    assert(components.component_sum(0) == 11);
    components.apply_component(2, {-1, 5});
    assert(components.vertex_value(2) == 2);
    assert(components.add_edge(1, 2));
    assert(components.component_sum(2) == 13);
    components.apply_component(2, {0, 8});
    assert(components.vertex_value(0) == 8);
    assert(components.vertex_value(1) == 8);
    assert(components.vertex_value(2) == 8);
    assert(components.component_sum(0) == 24);
    assert(!components.add_edge(0, 2));
    assert(components.component_size(1) == 3);
    assert(components.size(2) == 3);
    assert(components.size() == 4);
    assert(components.groups() == 2);
    assert(components.history_node_count() == 6);

    bool vertex_threw = false;
    try{
        (void) components.vertex_value(4);
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
    AffineComponents components(values);
    while(query_count--){
        std::string type;
        std::cin >> type;
        if(type == "E"){
            int first, second;
            std::cin >> first >> second;
            std::cout << components.add_edge(first, second) << '\n';
        }else if(type == "A"){
            int vertex;
            AffineAction action;
            std::cin >> vertex >> action.multiplier >> action.addend;
            components.apply_component(vertex, action);
        }else if(type == "V"){
            int vertex;
            std::cin >> vertex;
            std::cout << components.vertex_value(vertex) << '\n';
        }else if(type == "Q"){
            int vertex;
            std::cin >> vertex;
            std::cout << components.component_sum(vertex) << '\n';
        }else if(type == "S"){
            int first, second;
            std::cin >> first >> second;
            std::cout << components.same(first, second) << '\n';
        }else if(type == "Z"){
            int vertex;
            std::cin >> vertex;
            std::cout << components.component_size(vertex) << '\n';
        }else if(type == "G"){
            std::cout << components.groups() << '\n';
        }else if(type == "H"){
            std::cout << components.history_node_count() << '\n';
        }else{
            std::cout << components.size() << '\n';
        }
    }
}
