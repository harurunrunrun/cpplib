// competitive-verifier: PROBLEM https://www.spoj.com/problems/GOODA/

#include <vector>

#include "../../src/algorithm/graph/specialized/maximum_distinct_vertex_weight_walk.hpp"
#include "../../src/structure/other/fastio.hpp"

namespace{
fastio io;
}

int main(){
    int size, edge_count, start, goal;
    io.readint(size);
    io.readint(edge_count);
    io.readint(start);
    io.readint(goal);
    std::vector<long long> vertex_weight(static_cast<std::size_t>(size));
    for(long long& value: vertex_weight) io.readll(value);
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(size));
    while(edge_count--){
        int from, to;
        io.readint(from);
        io.readint(to);
        graph[static_cast<std::size_t>(from - 1)].push_back(to - 1);
    }
    io.writell(maximum_distinct_vertex_weight_walk(graph, vertex_weight, start - 1, goal - 1).value());
    io.write('\n');
}
