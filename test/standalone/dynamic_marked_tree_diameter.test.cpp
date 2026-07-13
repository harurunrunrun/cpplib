// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "../../src/structure/tree/centroid_distance_index.hpp"
#include "../../src/structure/tree/dynamic_marked_tree_diameter.hpp"

using Distance = long long;

std::vector<std::vector<Distance>> all_distances(
    const std::vector<std::vector<std::pair<int, Distance>>>& graph
){
    const int size = static_cast<int>(graph.size());
    std::vector<std::vector<Distance>> result(
        static_cast<std::size_t>(size),
        std::vector<Distance>(static_cast<std::size_t>(size))
    );
    for(int source = 0; source < size; ++source){
        std::vector<std::tuple<int, int, Distance>> stack = {{source, -1, 0}};
        while(!stack.empty()){
            const auto [vertex, parent, distance] = stack.back();
            stack.pop_back();
            result[static_cast<std::size_t>(source)][static_cast<std::size_t>(vertex)] = distance;
            for(const auto& [to, weight]: graph[static_cast<std::size_t>(vertex)]){
                if(to != parent) stack.push_back({to, vertex, distance + weight});
            }
        }
    }
    return result;
}

std::optional<Distance> brute_diameter(
    const std::vector<unsigned char>& marked,
    const std::vector<std::vector<Distance>>& distance
){
    bool any = false;
    Distance answer = 0;
    for(int left = 0; left < static_cast<int>(marked.size()); ++left){
        if(marked[static_cast<std::size_t>(left)] == 0) continue;
        any = true;
        for(int right = 0; right < static_cast<int>(marked.size()); ++right){
            if(marked[static_cast<std::size_t>(right)] != 0){
                answer = std::max(
                    answer,
                    distance[static_cast<std::size_t>(left)][static_cast<std::size_t>(right)]
                );
            }
        }
    }
    if(!any) return std::nullopt;
    return answer;
}

void test_index(){
    CentroidDistanceIndex<Distance, 16> index(5);
    const std::vector<std::tuple<int, int, Distance>> edges = {
        {0, 1, 4}, {1, 2, -7}, {2, 3, 2}, {3, 4, 6}
    };
    std::vector<std::vector<std::pair<int, Distance>>> graph(5);
    for(const auto& [left, right, weight]: edges){
        index.add_edge(left, right, weight);
        graph[static_cast<std::size_t>(left)].push_back({right, weight});
        graph[static_cast<std::size_t>(right)].push_back({left, weight});
    }
    index.build();
    const auto distance = all_distances(graph);
    assert(index.size() == 5);
    assert(index.branch_count() == 9);
    assert(index.parent(index.root()) == -1);
    assert(index.depth(index.root()) == 0);
    for(int vertex = 0; vertex < 5; ++vertex){
        const auto entries = index.entries(vertex);
        assert(!entries.empty());
        assert(entries.front().centroid == index.root());
        bool found_self = false;
        for(const auto& entry: entries){
            assert(index.branch_centroid(entry.branch) == entry.centroid);
            assert(entry.distance == distance[static_cast<std::size_t>(vertex)]
                [static_cast<std::size_t>(entry.centroid)]);
            if(entry.centroid == vertex){
                assert(entry.distance == 0);
                found_self = true;
            }
        }
        assert(found_self);
    }
}

void test_negative_edges_and_duplicates(){
    DynamicMarkedTreeDiameter<Distance, 16> tree(4);
    tree.add_edge(0, 1, -5);
    tree.add_edge(1, 2, -2);
    tree.add_edge(2, 3, -9);
    tree.build(true);
    assert(tree.marked_count() == 4);
    assert(tree.diameter() == std::optional<Distance>(0));
    assert(!tree.mark(2));
    assert(tree.unmark(2));
    assert(!tree.unmark(2));
    assert(tree.diameter() == std::optional<Distance>(0));
    assert(tree.toggle(2));
    assert(tree.is_marked(2));
    assert(!tree.toggle(2));
    assert(!tree.is_marked(2));

    tree.build(false);
    assert(tree.marked_count() == 0);
    assert(!tree.diameter());
    assert(tree.mark(1));
    assert(tree.diameter() == std::optional<Distance>(0));
}

void test_random(){
    std::mt19937 generator(20260714U);
    for(int repetition = 0; repetition < 35; ++repetition){
        const int size = 1 + static_cast<int>(generator() % 35U);
        DynamicMarkedTreeDiameter<Distance, 64> tree(size);
        std::vector<std::vector<std::pair<int, Distance>>> graph(
            static_cast<std::size_t>(size)
        );
        for(int vertex = 1; vertex < size; ++vertex){
            const int parent = static_cast<int>(generator() % static_cast<unsigned int>(vertex));
            const Distance weight = static_cast<Distance>(generator() % 31U) - 15;
            tree.add_edge(vertex, parent, weight);
            graph[static_cast<std::size_t>(vertex)].push_back({parent, weight});
            graph[static_cast<std::size_t>(parent)].push_back({vertex, weight});
        }
        tree.build(false);
        const auto distance = all_distances(graph);
        std::vector<unsigned char> marked(static_cast<std::size_t>(size), 0);
        for(int operation = 0; operation < 1200; ++operation){
            const int vertex = static_cast<int>(
                generator() % static_cast<unsigned int>(size)
            );
            switch(generator() % 4U){
                case 0:
                    assert(tree.mark(vertex) == (marked[static_cast<std::size_t>(vertex)] == 0));
                    marked[static_cast<std::size_t>(vertex)] = 1;
                    break;
                case 1:
                    assert(tree.unmark(vertex) == (marked[static_cast<std::size_t>(vertex)] != 0));
                    marked[static_cast<std::size_t>(vertex)] = 0;
                    break;
                case 2:
                    marked[static_cast<std::size_t>(vertex)] ^= 1U;
                    assert(tree.toggle(vertex)
                        == (marked[static_cast<std::size_t>(vertex)] != 0));
                    break;
                default:
                    break;
            }
            assert(tree.marked_count() == static_cast<int>(std::count(
                marked.begin(), marked.end(), static_cast<unsigned char>(1)
            )));
            assert(tree.diameter() == brute_diameter(marked, distance));
        }
    }
}

void test_exceptions(){
    bool thrown = false;
    try{
        CentroidDistanceIndex<Distance, 4> index(5);
        (void)index;
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        CentroidDistanceIndex<Distance, 4> index(2);
        (void)index.root();
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        CentroidDistanceIndex<Distance, 4> index(2);
        index.add_edge(0, 2, 1);
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        CentroidDistanceIndex<Distance, 4> index(3);
        index.add_edge(0, 1, 1);
        index.add_edge(2, 2, 1);
        index.build();
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        CentroidDistanceIndex<Distance, 4> index(4);
        index.add_edge(0, 1, 0);
        index.add_edge(1, 2, std::numeric_limits<Distance>::max());
        index.add_edge(2, 3, 1);
        index.build();
    }catch(const std::overflow_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        DynamicMarkedTreeDiameter<Distance, 4> tree(2);
        (void)tree.diameter();
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    DynamicMarkedTreeDiameter<Distance, 4> tree(2);
    tree.add_edge(0, 1, 3);
    tree.build();
    thrown = false;
    try{
        (void)tree.mark(2);
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        tree.add_edge(0, 1, 3);
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    test_index();
    test_negative_edges_and_duplicates();
    test_exceptions();

    int test_count;
    if(std::cin >> test_count){
        while(test_count-- != 0){
            int size, initially_marked, operation_count;
            std::cin >> size >> initially_marked >> operation_count;
            DynamicMarkedTreeDiameter<Distance, 256> tree(size);
            for(int edge = 1; edge < size; ++edge){
                int left, right;
                Distance weight;
                std::cin >> left >> right >> weight;
                tree.add_edge(left, right, weight);
            }
            tree.build(initially_marked != 0);
            while(operation_count-- != 0){
                char operation;
                std::cin >> operation;
                if(operation == 'Q'){
                    const auto answer = tree.diameter();
                    if(answer) std::cout << *answer << '\n';
                    else std::cout << "EMPTY\n";
                }else{
                    int vertex;
                    std::cin >> vertex;
                    if(operation == 'M') std::cout << tree.mark(vertex) << '\n';
                    else if(operation == 'U') std::cout << tree.unmark(vertex) << '\n';
                    else std::cout << tree.toggle(vertex) << '\n';
                }
            }
        }
        return 0;
    }

    test_random();
}
