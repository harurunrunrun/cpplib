// competitive-verifier: PROBLEM https://www.spoj.com/problems/AKBAR/

#include <utility>
#include <vector>

#include "../../src/algorithm/graph/exactly_once_guard_coverage.hpp"
#include "../../src/structure/other/fastio.hpp"

namespace{
fastio io;
}

int main(){
    int tests;
    io.readint(tests);
    while(tests--){
        int size, edge_count, guard_count;
        io.readint(size);
        io.readint(edge_count);
        io.readint(guard_count);
        std::vector<std::pair<int, int>> edges;
        edges.reserve(static_cast<std::size_t>(edge_count));
        while(edge_count--){
            int left, right;
            io.readint(left);
            io.readint(right);
            edges.emplace_back(left - 1, right - 1);
        }
        std::vector<std::pair<int, int>> guards;
        guards.reserve(static_cast<std::size_t>(guard_count));
        while(guard_count--){
            int base, strength;
            io.readint(base);
            io.readint(strength);
            guards.emplace_back(base - 1, strength);
        }
        io.write(has_exactly_once_guard_coverage(size, edges, guards)
            ? "Yes\n" : "No\n");
    }
}
