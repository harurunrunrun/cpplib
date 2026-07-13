// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/search/bounded_heuristic_search.hpp"

using Edge = std::pair<int, int>;
using Graph = std::vector<std::vector<Edge>>;

int bfs(const Graph& graph, int source, int target){
    std::vector<int> distance(graph.size(), -1);
    std::queue<int> queue;
    distance[static_cast<std::size_t>(source)] = 0;
    queue.push(source);
    while(!queue.empty()){
        const int vertex = queue.front();
        queue.pop();
        for(const auto [to, cost] : graph[static_cast<std::size_t>(vertex)]){
            assert(cost == 1);
            if(distance[static_cast<std::size_t>(to)] != -1) continue;
            distance[static_cast<std::size_t>(to)]
                = distance[static_cast<std::size_t>(vertex)] + 1;
            queue.push(to);
        }
    }
    return distance[static_cast<std::size_t>(target)];
}

void self_test(){
    constexpr int height = 5;
    constexpr int width = 6;
    Graph graph(static_cast<std::size_t>(height * width));
    for(int row = 0; row < height; ++row){
        for(int column = 0; column < width; ++column){
            const int vertex = row * width + column;
            if(row + 1 < height){
                graph[static_cast<std::size_t>(vertex)].emplace_back(vertex + width, 1);
                graph[static_cast<std::size_t>(vertex + width)].emplace_back(vertex, 1);
            }
            if(column + 1 < width){
                graph[static_cast<std::size_t>(vertex)].emplace_back(vertex + 1, 1);
                graph[static_cast<std::size_t>(vertex + 1)].emplace_back(vertex, 1);
            }
        }
    }
    const int target = height * width - 1;
    const auto neighbors = [&](int vertex){ return graph[static_cast<std::size_t>(vertex)]; };
    const auto goal = [target](int vertex){ return vertex == target; };
    const auto heuristic = [](int vertex){
        const int row = vertex / width;
        const int column = vertex % width;
        return (height - 1 - row) + (width - 1 - column);
    };
    const auto beam = beam_search(0, goal, neighbors, heuristic, 6, 20);
    const auto ida = ida_star_search(0, goal, neighbors, heuristic);
    assert(beam && *beam.cost == height + width - 2);
    assert(ida && *ida.cost == height + width - 2);

    bool threw = false;
    try{
        static_cast<void>(beam_search(0, goal, neighbors, heuristic, 0, 20));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
}

int main(){
    int n;
    int m;
    int source;
    int target;
    if(!(std::cin >> n >> m >> source >> target)){
        self_test();
        return 0;
    }
    Graph graph(static_cast<std::size_t>(n));
    for(int edge_index = 0; edge_index < m; ++edge_index){
        int first;
        int second;
        std::cin >> first >> second;
        graph[static_cast<std::size_t>(first)].emplace_back(second, 1);
        graph[static_cast<std::size_t>(second)].emplace_back(first, 1);
    }
    const auto neighbors = [&](int vertex){ return graph[static_cast<std::size_t>(vertex)]; };
    const auto goal = [target](int vertex){ return vertex == target; };
    const auto zero = [](int){ return 0; };
    const auto beam = beam_search(
        source, goal, neighbors, zero, static_cast<std::size_t>(n),
        static_cast<std::size_t>(n)
    );
    const auto ida = ida_star_search(source, goal, neighbors, zero);
    assert(beam.cost.value_or(-1) == bfs(graph, source, target));
    std::cout << beam.cost.value_or(-1) << ' ' << ida.cost.value_or(-1) << '\n';
}
