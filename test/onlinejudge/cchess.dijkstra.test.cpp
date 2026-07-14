// competitive-verifier: PROBLEM https://www.spoj.com/problems/CCHESS/

#include <array>
#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/dijkstra.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    constexpr int BOARD_SIZE = 8;
    constexpr int VERTEX_COUNT = BOARD_SIZE * BOARD_SIZE;
    constexpr std::array<int, 8> DX = {1, 1, -1, -1, 2, 2, -2, -2};
    constexpr std::array<int, 8> DY = {2, -2, 2, -2, 1, -1, 1, -1};
    auto vertex_id = [](int x, int y){ return x * BOARD_SIZE + y; };

    std::vector<std::vector<DijkstraEdge<int>>> graph(VERTEX_COUNT);
    for(int x = 0; x < BOARD_SIZE; x++){
        for(int y = 0; y < BOARD_SIZE; y++){
            const int from = vertex_id(x, y);
            for(std::size_t direction = 0; direction < DX.size(); direction++){
                const int next_x = x + DX[direction];
                const int next_y = y + DY[direction];
                if(next_x < 0 || BOARD_SIZE <= next_x ||
                   next_y < 0 || BOARD_SIZE <= next_y){
                    continue;
                }
                graph[static_cast<std::size_t>(from)].push_back({
                    vertex_id(next_x, next_y),
                    x * next_x + y * next_y,
                });
            }
        }
    }

    std::vector<std::vector<int>> distance(
        VERTEX_COUNT, std::vector<int>(VERTEX_COUNT)
    );
    std::vector<std::vector<char>> reachable(
        VERTEX_COUNT, std::vector<char>(VERTEX_COUNT)
    );
    for(int source = 0; source < VERTEX_COUNT; source++){
        auto result = dijkstra(graph, source);
        distance[static_cast<std::size_t>(source)] = std::move(result.dist);
        reachable[static_cast<std::size_t>(source)] = std::move(result.reachable);
    }

    int start_x, start_y, target_x, target_y;
    while(std::cin >> start_x >> start_y >> target_x >> target_y){
        const int source = vertex_id(start_x, start_y);
        const int target = vertex_id(target_x, target_y);
        if(!reachable[static_cast<std::size_t>(source)]
                     [static_cast<std::size_t>(target)]){
            std::cout << -1 << '\n';
        }else{
            std::cout << distance[static_cast<std::size_t>(source)]
                                 [static_cast<std::size_t>(target)] << '\n';
        }
    }
}
