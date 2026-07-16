// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/dsu/incremental_component_sum.hpp"

using ComponentSum = IncrementalComponentSum<long long, 100000>;

void run_direct_tests(){
    ComponentSum graph(std::vector<long long>{5, -2, 8, 0});
    assert(graph.size() == 4);
    assert(graph.groups() == 4);
    assert(graph.component_sum(1) == -2);
    graph.add_vertex(1, 7);
    assert(graph.component_sum(1) == 5);
    assert(graph.add_edge(0, 1));
    assert(!graph.add_edge(1, 0));
    assert(!graph.add_edge(0, 0));
    assert(graph.same(0, 1));
    assert(graph.component_sum(0) == 10);
    assert(graph.component_size(1) == 2);
    assert(graph.size(0) == 2);
    assert(graph.add_edge(2, 3));
    graph.add_vertex(3, -3);
    assert(graph.component_sum(2) == 5);
    assert(graph.add_edge(1, 3));
    assert(graph.component_sum(2) == 15);
    assert(graph.groups() == 1);
    assert(graph.leader(0) == graph.leader(3));

    ComponentSum zeros(3);
    assert(zeros.component_sum(2) == 0);

    bool constructor_threw = false;
    try{
        ComponentSum invalid(100001);
    }catch(const std::runtime_error&){
        constructor_threw = true;
    }
    assert(constructor_threw);

    bool vertex_threw = false;
    try{
        graph.add_vertex(-1, 1);
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
    ComponentSum graph(values);
    while(query_count--){
        std::string type;
        std::cin >> type;
        if(type == "A"){
            int vertex;
            long long delta;
            std::cin >> vertex >> delta;
            graph.add_vertex(vertex, delta);
        }else if(type == "E"){
            int first, second;
            std::cin >> first >> second;
            std::cout << graph.add_edge(first, second) << '\n';
        }else if(type == "Q"){
            int vertex;
            std::cin >> vertex;
            std::cout << graph.component_sum(vertex) << '\n';
        }else if(type == "S"){
            int first, second;
            std::cin >> first >> second;
            std::cout << graph.same(first, second) << '\n';
        }else if(type == "Z"){
            int vertex;
            std::cin >> vertex;
            std::cout << graph.component_size(vertex) << '\n';
        }else if(type == "G"){
            std::cout << graph.groups() << '\n';
        }else{
            std::cout << graph.size() << '\n';
        }
    }
}

