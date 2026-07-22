// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/graph/shortest_path/named_shortest_paths.hpp"

void self_test(){
    NamedShortestPaths<int> paths(
        {"a", "b", "c"},
        {{{1, 4}}, {{2, 3}}, {}}
    );
    assert(paths.size() == 3);
    assert(paths.contains("b"));
    assert(!paths.contains("x"));
    assert(paths.distance("a", "c") == 7);
    assert(!paths.distance("c", "a"));
    paths.clear_cache();
    assert(paths.distance("a", "c") == 7);

    bool thrown = false;
    try{
        (void)paths.distance("x", "a");
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)NamedShortestPaths<int>({"a", "a"}, {{}, {}});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)NamedShortestPaths<int>({"a"}, {{{0, -1}}});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();

    int vertex_count, edge_count, query_count;
    if(!(std::cin >> vertex_count >> edge_count >> query_count)) return 0;
    std::vector<std::string> names(static_cast<std::size_t>(vertex_count));
    for(auto& name: names) std::cin >> name;
    std::vector<std::vector<DijkstraEdge<long long>>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    while(edge_count-- > 0){
        int from, to;
        long long cost;
        std::cin >> from >> to >> cost;
        graph[static_cast<std::size_t>(from)].push_back({to, cost});
    }
    NamedShortestPaths<long long> paths(std::move(names), std::move(graph));
    while(query_count-- > 0){
        std::string source, target;
        std::cin >> source >> target;
        const auto answer = paths.distance(source, target);
        if(answer) std::cout << *answer << '\n';
        else std::cout << "unreachable\n";
    }
}
