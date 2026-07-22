// competitive-verifier: PROBLEM https://www.spoj.com/problems/AKBAR/

#include <utility>
#include <vector>

#include "../../src/algorithm/graph/specialized/radius_balls_form_vertex_partition.hpp"
#include "../../src/structure/other/fastio.hpp"

namespace{
fastio io;
}

int main(){
    int test_count;
    io.readint(test_count);
    while(test_count--){
        int vertex_count, edge_count, ball_count;
        io.readint(vertex_count);
        io.readint(edge_count);
        io.readint(ball_count);

        std::vector<std::pair<int, int>> edges;
        edges.reserve(static_cast<std::size_t>(edge_count));
        while(edge_count--){
            int left, right;
            io.readint(left);
            io.readint(right);
            edges.emplace_back(left - 1, right - 1);
        }

        std::vector<RadiusBall> balls;
        balls.reserve(static_cast<std::size_t>(ball_count));
        while(ball_count--){
            int center, radius;
            io.readint(center);
            io.readint(radius);
            balls.push_back({center - 1, radius});
        }
        io.write(radius_balls_form_vertex_partition(
            vertex_count, edges, balls
        ) ? "Yes\n" : "No\n");
    }
}
