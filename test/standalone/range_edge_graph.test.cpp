// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <queue>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/range_edge_graph.hpp"

namespace{

using Weight = long long;
using CompressedGraph = RangeEdgeGraph<Weight>;
using NaiveGraph = std::vector<std::vector<std::pair<int, Weight>>>;

constexpr Weight infinity = std::numeric_limits<Weight>::max() / 4;

std::vector<Weight> compressed_distances(
    const CompressedGraph& graph,
    int source
){
    std::vector<Weight> distance(
        static_cast<std::size_t>(graph.vertex_count()),
        infinity
    );
    using QueueEntry = std::pair<Weight, int>;
    std::priority_queue<
        QueueEntry,
        std::vector<QueueEntry>,
        std::greater<QueueEntry>
    > queue;
    distance[static_cast<std::size_t>(source)] = 0;
    queue.emplace(0, source);
    while(!queue.empty()){
        const auto [current, vertex] = queue.top();
        queue.pop();
        if(current != distance[static_cast<std::size_t>(vertex)]) continue;
        for(const auto& edge: graph.view()[static_cast<std::size_t>(vertex)]){
            if(distance[static_cast<std::size_t>(edge.to)]
               > current + edge.weight){
                distance[static_cast<std::size_t>(edge.to)] =
                    current + edge.weight;
                queue.emplace(
                    distance[static_cast<std::size_t>(edge.to)],
                    edge.to
                );
            }
        }
    }
    distance.resize(static_cast<std::size_t>(graph.original_size()));
    return distance;
}

std::vector<Weight> naive_distances(
    const NaiveGraph& graph,
    int source
){
    std::vector<Weight> distance(graph.size(), infinity);
    using QueueEntry = std::pair<Weight, int>;
    std::priority_queue<
        QueueEntry,
        std::vector<QueueEntry>,
        std::greater<QueueEntry>
    > queue;
    distance[static_cast<std::size_t>(source)] = 0;
    queue.emplace(0, source);
    while(!queue.empty()){
        const auto [current, vertex] = queue.top();
        queue.pop();
        if(current != distance[static_cast<std::size_t>(vertex)]) continue;
        for(const auto& [to, weight]: graph[static_cast<std::size_t>(vertex)]){
            if(distance[static_cast<std::size_t>(to)] > current + weight){
                distance[static_cast<std::size_t>(to)] = current + weight;
                queue.emplace(distance[static_cast<std::size_t>(to)], to);
            }
        }
    }
    return distance;
}

void add_naive_point_to_range(
    NaiveGraph& graph,
    int from,
    int left,
    int right,
    Weight weight
){
    for(int to = left; to < right; ++to){
        graph[static_cast<std::size_t>(from)].emplace_back(to, weight);
    }
}

void add_naive_range_to_point(
    NaiveGraph& graph,
    int left,
    int right,
    int to,
    Weight weight
){
    for(int from = left; from < right; ++from){
        graph[static_cast<std::size_t>(from)].emplace_back(to, weight);
    }
}

void add_naive_range_to_range(
    NaiveGraph& graph,
    int source_left,
    int source_right,
    int destination_left,
    int destination_right,
    Weight weight
){
    for(int from = source_left; from < source_right; ++from){
        add_naive_point_to_range(
            graph,
            from,
            destination_left,
            destination_right,
            weight
        );
    }
}

void self_test(){
    bool thrown = false;
    try{
        (void)CompressedGraph(0);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);

    CompressedGraph singleton(1);
    assert(singleton.original_size() == 1);
    assert(singleton.vertex_count() == 3);
    assert(singleton.edge_count() == 2);
    const auto edges_before = singleton.edge_count();
    const auto vertices_before = singleton.vertex_count();
    singleton.add_point_to_range(0, 0, 0, 7);
    singleton.add_range_to_point(0, 0, 0, 7);
    singleton.add_range_to_range(0, 0, 0, 1, 7);
    assert(singleton.edge_count() == edges_before);
    assert(singleton.vertex_count() == vertices_before);

    thrown = false;
    try{
        singleton.add_edge(-1, 0, 1);
    }catch(const std::out_of_range&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        singleton.add_point_to_range(0, 1, 0, 1);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        singleton.add_range_to_point(-1, 1, 0, 1);
    }catch(const std::out_of_range&){
        thrown = true;
    }
    assert(thrown);

    std::mt19937_64 engine(2026071604ULL);
    for(int round = 0; round < 30; ++round){
        const int size = 1 + static_cast<int>(engine() % 8);
        CompressedGraph graph(size);
        NaiveGraph naive(static_cast<std::size_t>(size));
        for(int query = 0; query < 120; ++query){
            const int operation = static_cast<int>(engine() % 4);
            const Weight weight = static_cast<Weight>(engine() % 30);
            const int first = static_cast<int>(
                engine() % static_cast<unsigned long long>(size + 1)
            );
            const int second = static_cast<int>(
                engine() % static_cast<unsigned long long>(size + 1)
            );
            const int left = std::min(first, second);
            const int right = std::max(first, second);
            if(operation == 0){
                const int from = static_cast<int>(
                    engine() % static_cast<unsigned long long>(size)
                );
                const int to = static_cast<int>(
                    engine() % static_cast<unsigned long long>(size)
                );
                graph.add_edge(from, to, weight);
                naive[static_cast<std::size_t>(from)].emplace_back(to, weight);
            }else if(operation == 1){
                const int from = static_cast<int>(
                    engine() % static_cast<unsigned long long>(size)
                );
                graph.add_point_to_range(from, left, right, weight);
                add_naive_point_to_range(
                    naive,
                    from,
                    left,
                    right,
                    weight
                );
            }else if(operation == 2){
                const int to = static_cast<int>(
                    engine() % static_cast<unsigned long long>(size)
                );
                graph.add_range_to_point(left, right, to, weight);
                add_naive_range_to_point(
                    naive,
                    left,
                    right,
                    to,
                    weight
                );
            }else{
                const int third = static_cast<int>(
                    engine() % static_cast<unsigned long long>(size + 1)
                );
                const int fourth = static_cast<int>(
                    engine() % static_cast<unsigned long long>(size + 1)
                );
                const int destination_left = std::min(third, fourth);
                const int destination_right = std::max(third, fourth);
                graph.add_range_to_range(
                    left,
                    right,
                    destination_left,
                    destination_right,
                    weight
                );
                add_naive_range_to_range(
                    naive,
                    left,
                    right,
                    destination_left,
                    destination_right,
                    weight
                );
            }
        }
        for(int source = 0; source < size; ++source){
            assert(
                compressed_distances(graph, source)
                == naive_distances(naive, source)
            );
        }
    }

    constexpr int large_size = 20000;
    CompressedGraph large(large_size);
    const int base_vertices = large.vertex_count();
    const std::size_t base_edges = large.edge_count();
    int range_to_range_count = 0;
    for(int query = 0; query < 5000; ++query){
        const int vertex = query % large_size;
        const int left = (query * 37) % large_size;
        const int right = std::min(large_size, left + 1 + query % 100);
        if(query % 4 == 0){
            large.add_edge(vertex, (vertex + 1) % large_size, query);
        }else if(query % 4 == 1){
            large.add_point_to_range(vertex, left, right, query);
        }else if(query % 4 == 2){
            large.add_range_to_point(left, right, vertex, query);
        }else{
            const int destination = (query * 97) % large_size;
            large.add_range_to_range(
                left,
                right,
                destination,
                std::min(large_size, destination + 80),
                query
            );
            ++range_to_range_count;
        }
    }
    assert(large.vertex_count() == base_vertices + range_to_range_count);
    assert(large.edge_count() > base_edges);
    assert(large.edge_count() < base_edges + 5000U * 64U);
    assert(large.build().size() == large.view().size());
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    int query_count;
    if(!(std::cin >> vertex_count >> query_count)){
        self_test();
        return 0;
    }

    CompressedGraph graph(vertex_count);
    while(query_count-- != 0){
        std::string operation;
        std::cin >> operation;
        if(operation == "E"){
            int from, to;
            Weight weight;
            std::cin >> from >> to >> weight;
            graph.add_edge(from, to, weight);
        }else if(operation == "PR"){
            int from, left, right;
            Weight weight;
            std::cin >> from >> left >> right >> weight;
            graph.add_point_to_range(from, left, right, weight);
        }else if(operation == "RP"){
            int left, right, to;
            Weight weight;
            std::cin >> left >> right >> to >> weight;
            graph.add_range_to_point(left, right, to, weight);
        }else if(operation == "RR"){
            int source_left, source_right;
            int destination_left, destination_right;
            Weight weight;
            std::cin
                >> source_left >> source_right
                >> destination_left >> destination_right
                >> weight;
            graph.add_range_to_range(
                source_left,
                source_right,
                destination_left,
                destination_right,
                weight
            );
        }else{
            throw std::invalid_argument("unknown operation");
        }
    }

    int source;
    std::cin >> source;
    const auto distance = compressed_distances(graph, source);
    for(std::size_t vertex = 0; vertex < distance.size(); ++vertex){
        if(vertex != 0) std::cout << ' ';
        if(distance[vertex] == infinity) std::cout << -1;
        else std::cout << distance[vertex];
    }
    std::cout << '\n';
}
