// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <queue>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/structure/tree/dynamic_monochromatic_tree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

using Value = long long;

template<int MAX_SIZE>
using MaximumTree = DynamicMonochromaticTree<MaxMonoid<Value>{}, MAX_SIZE>;

template<int MAX_SIZE>
using SizeTree = DynamicMonochromaticTree<AddMonoid<int>{}, MAX_SIZE>;

std::vector<int> brute_component(
    int source,
    const std::vector<std::vector<int>>& graph,
    const std::vector<bool>& color
){
    std::vector<int> result;
    std::vector<bool> visited(graph.size());
    std::vector<int> stack = {source};
    visited[static_cast<std::size_t>(source)] = true;
    while(!stack.empty()){
        const int vertex = stack.back();
        stack.pop_back();
        result.push_back(vertex);
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            if(!visited[static_cast<std::size_t>(to)]
                && color[static_cast<std::size_t>(to)]
                    == color[static_cast<std::size_t>(vertex)]){
                visited[static_cast<std::size_t>(to)] = true;
                stack.push_back(to);
            }
        }
    }
    return result;
}

void test_contract(){
    MaximumTree<16> tree(
        std::vector<Value>{-8, -3, -11, -5, -9},
        std::vector<bool>{false, false, true, true, false}
    );
    assert(tree.size() == 5);
    tree.add_edge(0, 1);
    tree.add_edge(1, 2);
    tree.add_edge(1, 3);
    tree.add_edge(3, 4);

    assert(tree.set_color(4, true));
    assert(!tree.set_color(4, true));
    assert(!tree.toggle(4));
    tree.set(0, -2);
    assert(tree.get(0) == -2);

    bool thrown = false;
    try{
        (void)tree.component_prod(0);
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    tree.build(2);
    assert(tree.root() == 2);
    assert(tree.component_prod(0) == -2);
    assert(tree.component_prod(2) == -11);
    assert(tree.toggle(1));
    assert(tree.component_prod(1) == -3);
    assert(tree.component_prod(0) == -2);
    tree.set(2, -1);
    assert(tree.component_prod(2) == -1);

    tree.build(0);
    assert(tree.root() == 0);
    assert(tree.component_prod(2) == -1);

    MaximumTree<4> single(std::vector<Value>{-17});
    single.build();
    assert(single.component_prod(0) == -17);
    single.set(0, std::numeric_limits<Value>::lowest() + 1);
    assert(single.component_prod(0) == std::numeric_limits<Value>::lowest() + 1);

    DynamicMonochromaticTree<MaxMonoid<Value>{}, 4> empty(0);
    empty.build();
    assert(empty.size() == 0);
    assert(empty.root() == -1);
}

void test_exceptions(){
    bool thrown = false;
    try{
        MaximumTree<2> tree(3);
        (void)tree;
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        MaximumTree<4> tree(
            std::vector<Value>{1, 2}, std::vector<bool>{true}
        );
        (void)tree;
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        MaximumTree<4> tree(2);
        tree.add_edge(0, 2);
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        MaximumTree<4> tree(3);
        tree.add_edge(0, 1);
        tree.build();
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        MaximumTree<4> tree(3);
        tree.add_edge(0, 1);
        tree.add_edge(1, 0);
        tree.build();
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    MaximumTree<4> tree(2);
    tree.add_edge(0, 1);
    tree.build();
    thrown = false;
    try{
        tree.add_edge(0, 1);
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        volatile int invalid_vertex = -1;
        (void)tree.color(invalid_vertex);
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        tree.set(2, 0);
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        tree.build(2);
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

void test_random(){
    std::mt19937 generator(20260714U);
    for(int repetition = 0; repetition < 30; ++repetition){
        const int size = 1 + static_cast<int>(generator() % 45U);
        std::vector<Value> value(static_cast<std::size_t>(size));
        std::vector<bool> color(static_cast<std::size_t>(size));
        for(int vertex = 0; vertex < size; ++vertex){
            value[static_cast<std::size_t>(vertex)]
                = static_cast<Value>(generator() % 201U) - 100;
            color[static_cast<std::size_t>(vertex)] = (generator() & 1U) != 0;
        }
        MaximumTree<64> maximum(value, color);
        SizeTree<64> component_size(
            std::vector<int>(static_cast<std::size_t>(size), 1), color
        );
        std::vector<std::vector<int>> graph(static_cast<std::size_t>(size));
        for(int vertex = 1; vertex < size; ++vertex){
            const int parent = static_cast<int>(
                generator() % static_cast<unsigned int>(vertex)
            );
            maximum.add_edge(vertex, parent);
            component_size.add_edge(vertex, parent);
            graph[static_cast<std::size_t>(vertex)].push_back(parent);
            graph[static_cast<std::size_t>(parent)].push_back(vertex);
        }
        const int root = static_cast<int>(
            generator() % static_cast<unsigned int>(size)
        );
        maximum.build(root);
        component_size.build(root);
        for(int operation = 0; operation < 500; ++operation){
            const int vertex = static_cast<int>(
                generator() % static_cast<unsigned int>(size)
            );
            switch(generator() % 5U){
                case 0:{
                    color[static_cast<std::size_t>(vertex)]
                        = !color[static_cast<std::size_t>(vertex)];
                    assert(maximum.toggle(vertex)
                        == color[static_cast<std::size_t>(vertex)]);
                    (void)component_size.set_color(
                        vertex, color[static_cast<std::size_t>(vertex)]
                    );
                    break;
                }
                case 1:{
                    const bool next = (generator() & 1U) != 0;
                    const bool changed = color[static_cast<std::size_t>(vertex)] != next;
                    color[static_cast<std::size_t>(vertex)] = next;
                    assert(maximum.set_color(vertex, next) == changed);
                    assert(component_size.set_color(vertex, next) == changed);
                    break;
                }
                case 2:{
                    const Value next = static_cast<Value>(generator() % 401U) - 200;
                    value[static_cast<std::size_t>(vertex)] = next;
                    maximum.set(vertex, next);
                    break;
                }
                default:
                    break;
            }
            const auto component = brute_component(vertex, graph, color);
            Value expected_maximum = std::numeric_limits<Value>::lowest();
            for(int member: component){
                expected_maximum = std::max(
                    expected_maximum, value[static_cast<std::size_t>(member)]
                );
            }
            assert(maximum.component_prod(vertex) == expected_maximum);
            assert(component_size.component_prod(vertex)
                == static_cast<int>(component.size()));
            assert(maximum.color(vertex) == color[static_cast<std::size_t>(vertex)]);
            assert(maximum.get(vertex) == value[static_cast<std::size_t>(vertex)]);
        }
    }
}

template<int MAX_SIZE>
void run_case(int size, int operation_count){
    std::vector<bool> color(static_cast<std::size_t>(size));
    std::vector<Value> value(static_cast<std::size_t>(size));
    for(int vertex = 0; vertex < size; ++vertex){
        int source_color;
        std::cin >> source_color;
        color[static_cast<std::size_t>(vertex)] = source_color != 0;
    }
    for(Value& element: value) std::cin >> element;

    MaximumTree<MAX_SIZE> maximum(value, color);
    SizeTree<MAX_SIZE> component_size(
        std::vector<int>(static_cast<std::size_t>(size), 1), color
    );
    for(int edge = 1; edge < size; ++edge){
        int left, right;
        std::cin >> left >> right;
        maximum.add_edge(left, right);
        component_size.add_edge(left, right);
    }
    maximum.build();
    component_size.build();

    while(operation_count-- != 0){
        char operation;
        int vertex;
        std::cin >> operation >> vertex;
        if(operation == 'Q'){
            std::cout << maximum.component_prod(vertex) << ' '
                << component_size.component_prod(vertex) << '\n';
        }else if(operation == 'T'){
            const bool next = maximum.toggle(vertex);
            (void)component_size.set_color(vertex, next);
        }else if(operation == 'C'){
            int next;
            std::cin >> next;
            (void)maximum.set_color(vertex, next != 0);
            (void)component_size.set_color(vertex, next != 0);
        }else if(operation == 'V'){
            Value next;
            std::cin >> next;
            maximum.set(vertex, next);
        }else{
            std::cout << static_cast<int>(maximum.color(vertex)) << ' '
                << maximum.get(vertex) << '\n';
        }
    }
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    test_contract();
    test_exceptions();
    test_random();

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- != 0){
        int size, operation_count;
        std::cin >> size >> operation_count;
        if(size <= 256) run_case<256>(size, operation_count);
        else run_case<100000>(size, operation_count);
    }
}
