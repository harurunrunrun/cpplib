// competitive-verifier: PROBLEM https://www.spoj.com/problems/GOODA/

#include <vector>

#include "../../src/algorithm/graph/maximum_fun_walk.hpp"
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
    std::vector<long long> fun(static_cast<std::size_t>(size));
    for(long long& value: fun) io.readll(value);
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(size));
    while(edge_count--){
        int from, to;
        io.readint(from);
        io.readint(to);
        graph[static_cast<std::size_t>(from - 1)].push_back(to - 1);
    }
    io.writell(maximum_fun_walk(graph, fun, start - 1, goal - 1).value());
    io.write('\n');
}
