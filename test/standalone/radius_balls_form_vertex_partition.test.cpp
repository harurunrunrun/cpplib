// competitive-verifier: STANDALONE

#include <iostream>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/radius_balls_form_vertex_partition.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int vertex_count, edge_count, ball_count;
        std::cin >> vertex_count >> edge_count >> ball_count;
        std::vector<std::pair<int, int>> edges;
        edges.reserve(static_cast<std::size_t>(edge_count));
        while(edge_count-- > 0){
            int left, right;
            std::cin >> left >> right;
            edges.emplace_back(left, right);
        }
        std::vector<RadiusBall> balls(
            static_cast<std::size_t>(ball_count)
        );
        for(auto& ball: balls){
            std::cin >> ball.center >> ball.radius;
        }
        std::cout << (radius_balls_form_vertex_partition(
            vertex_count, edges, balls
        ) ? "YES\n" : "NO\n");
    }
}
