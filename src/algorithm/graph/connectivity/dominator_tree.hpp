#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DOMINATOR_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DOMINATOR_TREE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

struct DominatorTreeResult{
    std::vector<int> immediate_dominator;
    std::vector<std::vector<int>> tree;
    std::vector<int> dfs_order;
};

inline DominatorTreeResult dominator_tree(
    const std::vector<std::vector<int>>& graph,
    int source
){
    const int n = static_cast<int>(graph.size());
    if(source < 0 || n <= source)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (dominator_tree source)."
        );
    }
    for(const auto& edges: graph){
        for(int to: edges){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation (dominator_tree edge)."
                );
            }
        }
    }

    std::vector<int> dfs_index(static_cast<std::size_t>(n), -1);
    std::vector<int> order;
    order.reserve(static_cast<std::size_t>(n));
    std::vector<int> parent(static_cast<std::size_t>(n), -1);
    std::vector<int> next_edge(static_cast<std::size_t>(n), 0);
    std::vector<int> stack = {source};
    dfs_index[static_cast<std::size_t>(source)] = 0;
    order.push_back(source);
    while(!stack.empty()){
        const int vertex = stack.back();
        int& edge_index = next_edge[static_cast<std::size_t>(vertex)];
        if(edge_index == static_cast<int>(graph[static_cast<std::size_t>(vertex)].size())){
            stack.pop_back();
            continue;
        }
        const int to = graph[static_cast<std::size_t>(vertex)]
            [static_cast<std::size_t>(edge_index++)];
        if(dfs_index[static_cast<std::size_t>(to)] != -1) continue;
        const int index = static_cast<int>(order.size());
        dfs_index[static_cast<std::size_t>(to)] = index;
        parent[static_cast<std::size_t>(index)] =
            dfs_index[static_cast<std::size_t>(vertex)];
        order.push_back(to);
        stack.push_back(to);
    }

    const int reachable = static_cast<int>(order.size());
    std::vector<std::vector<int>> predecessor(static_cast<std::size_t>(reachable));
    for(int from_index = 0; from_index < reachable; ++from_index){
        const int from = order[static_cast<std::size_t>(from_index)];
        for(int to: graph[static_cast<std::size_t>(from)]){
            const int to_index = dfs_index[static_cast<std::size_t>(to)];
            if(to_index != -1){
                predecessor[static_cast<std::size_t>(to_index)].push_back(from_index);
            }
        }
    }

    std::vector<int> semi(static_cast<std::size_t>(reachable));
    std::vector<int> label(static_cast<std::size_t>(reachable));
    std::vector<int> ancestor(static_cast<std::size_t>(reachable), -1);
    std::vector<int> immediate(static_cast<std::size_t>(reachable), -1);
    std::vector<std::vector<int>> bucket(static_cast<std::size_t>(reachable));
    for(int index = 0; index < reachable; ++index){
        semi[static_cast<std::size_t>(index)] = index;
        label[static_cast<std::size_t>(index)] = index;
    }

    std::vector<int> path;
    path.reserve(static_cast<std::size_t>(reachable));
    const auto eval = [&](int vertex) -> int {
        if(ancestor[static_cast<std::size_t>(vertex)] == -1){
            return label[static_cast<std::size_t>(vertex)];
        }
        path.clear();
        int current = vertex;
        while(ancestor[static_cast<std::size_t>(current)] != -1){
            const int parent_vertex = ancestor[static_cast<std::size_t>(current)];
            if(ancestor[static_cast<std::size_t>(parent_vertex)] == -1) break;
            path.push_back(current);
            current = parent_vertex;
        }
        for(auto iterator = path.rbegin(); iterator != path.rend(); ++iterator){
            const int node = *iterator;
            const int parent_node = ancestor[static_cast<std::size_t>(node)];
            if(semi[static_cast<std::size_t>(label[static_cast<std::size_t>(parent_node)])] <
               semi[static_cast<std::size_t>(label[static_cast<std::size_t>(node)])]){
                label[static_cast<std::size_t>(node)] =
                    label[static_cast<std::size_t>(parent_node)];
            }
            ancestor[static_cast<std::size_t>(node)] =
                ancestor[static_cast<std::size_t>(parent_node)];
        }
        return label[static_cast<std::size_t>(vertex)];
    };

    for(int vertex = reachable - 1; vertex >= 1; --vertex){
        for(int from: predecessor[static_cast<std::size_t>(vertex)]){
            const int representative = eval(from);
            semi[static_cast<std::size_t>(vertex)] = std::min(
                semi[static_cast<std::size_t>(vertex)],
                semi[static_cast<std::size_t>(representative)]
            );
        }
        bucket[static_cast<std::size_t>(semi[static_cast<std::size_t>(vertex)])]
            .push_back(vertex);
        const int parent_vertex = parent[static_cast<std::size_t>(vertex)];
        ancestor[static_cast<std::size_t>(vertex)] = parent_vertex;
        for(int candidate: bucket[static_cast<std::size_t>(parent_vertex)]){
            const int representative = eval(candidate);
            immediate[static_cast<std::size_t>(candidate)] =
                semi[static_cast<std::size_t>(representative)] <
                        semi[static_cast<std::size_t>(candidate)]
                    ? representative
                    : parent_vertex;
        }
        bucket[static_cast<std::size_t>(parent_vertex)].clear();
    }

    immediate[0] = 0;
    for(int vertex = 1; vertex < reachable; ++vertex){
        if(immediate[static_cast<std::size_t>(vertex)] !=
           semi[static_cast<std::size_t>(vertex)]){
            immediate[static_cast<std::size_t>(vertex)] = immediate[static_cast<std::size_t>(
                immediate[static_cast<std::size_t>(vertex)]
            )];
        }
    }

    DominatorTreeResult result;
    result.immediate_dominator.assign(static_cast<std::size_t>(n), -1);
    result.tree.resize(static_cast<std::size_t>(n));
    result.dfs_order = order;
    for(int index = 0; index < reachable; ++index){
        const int vertex = order[static_cast<std::size_t>(index)];
        const int dominator = order[static_cast<std::size_t>(
            immediate[static_cast<std::size_t>(index)]
        )];
        result.immediate_dominator[static_cast<std::size_t>(vertex)] = dominator;
        if(vertex != source){
            result.tree[static_cast<std::size_t>(dominator)].push_back(vertex);
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DOMINATOR_TREE_HPP_INCLUDED
