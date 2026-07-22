#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_STRONGLY_CONNECTED_COMPONENTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_STRONGLY_CONNECTED_COMPONENTS_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <vector>

struct StronglyConnectedComponentsResult{
    int count;
    std::vector<int> id;
    std::vector<std::vector<int>> groups;

    bool same_component(int first, int second) const{
        const int vertex_count = static_cast<int>(id.size());
        if(first < 0 || vertex_count <= first ||
           second < 0 || vertex_count <= second)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(StronglyConnectedComponentsResult::same_component)."
            );
        }
        return id[static_cast<std::size_t>(first)] ==
            id[static_cast<std::size_t>(second)];
    }
};

StronglyConnectedComponentsResult strongly_connected_components(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    std::vector<std::vector<int>> reverse_graph(static_cast<std::size_t>(n));
    for(int v = 0; v < n; v++){
        for(int to: graph[static_cast<std::size_t>(v)]){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (strongly_connected_components).");
            }
            reverse_graph[static_cast<std::size_t>(to)].push_back(v);
        }
    }

    std::vector<char> seen(static_cast<std::size_t>(n), 0);
    std::vector<int> order;
    order.reserve(static_cast<std::size_t>(n));
    auto dfs1 = [&](auto&& self, int v) -> void {
        seen[static_cast<std::size_t>(v)] = 1;
        for(int to: graph[static_cast<std::size_t>(v)]){
            if(!seen[static_cast<std::size_t>(to)]) self(self, to);
        }
        order.push_back(v);
    };
    for(int v = 0; v < n; v++){
        if(!seen[static_cast<std::size_t>(v)]) dfs1(dfs1, v);
    }

    StronglyConnectedComponentsResult result;
    result.id.assign(static_cast<std::size_t>(n), -1);
    auto dfs2 = [&](auto&& self, int v, int id) -> void {
        result.id[static_cast<std::size_t>(v)] = id;
        result.groups[static_cast<std::size_t>(id)].push_back(v);
        for(int to: reverse_graph[static_cast<std::size_t>(v)]){
            if(result.id[static_cast<std::size_t>(to)] == -1) self(self, to, id);
        }
    };

    std::reverse(order.begin(), order.end());
    for(int v: order){
        if(result.id[static_cast<std::size_t>(v)] != -1) continue;
        int id = static_cast<int>(result.groups.size());
        result.groups.push_back({});
        dfs2(dfs2, v, id);
    }
    result.count = static_cast<int>(result.groups.size());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_STRONGLY_CONNECTED_COMPONENTS_HPP_INCLUDED
