#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_THREE_EDGE_CONNECTED_COMPONENTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_THREE_EDGE_CONNECTED_COMPONENTS_HPP_INCLUDED

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

struct ThreeEdgeConnectedComponentsResult{
    int count;
    std::vector<int> id;
    std::vector<std::vector<int>> groups;
};

inline ThreeEdgeConnectedComponentsResult three_edge_connected_components(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    constexpr const char* message =
        "library assertion fault: range violation "
        "(three_edge_connected_components).";
    if(vertex_count < 0)[[unlikely]] throw std::runtime_error(message);
    std::vector<std::vector<int>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(const auto& [left, right]: edges){
        if(left < 0 || vertex_count <= left ||
           right < 0 || vertex_count <= right)[[unlikely]]{
            throw std::runtime_error(message);
        }
        graph[static_cast<std::size_t>(left)].push_back(right);
        graph[static_cast<std::size_t>(right)].push_back(left);
    }

    const int none = vertex_count;
    std::vector<int> preorder(static_cast<std::size_t>(vertex_count));
    std::vector<int> subtree_end(static_cast<std::size_t>(vertex_count));
    std::vector<int> low(static_cast<std::size_t>(vertex_count), none);
    std::vector<int> degree(static_cast<std::size_t>(vertex_count));
    std::vector<int> path(static_cast<std::size_t>(vertex_count), none);
    std::vector<unsigned char> visited(
        static_cast<std::size_t>(vertex_count)
    );
    std::vector<int> component_next(static_cast<std::size_t>(vertex_count));
    std::iota(component_next.begin(), component_next.end(), 0);

    const auto absorb = [&](int vertex, int other){
        std::swap(
            component_next[static_cast<std::size_t>(vertex)],
            component_next[static_cast<std::size_t>(other)]
        );
        degree[static_cast<std::size_t>(vertex)] +=
            degree[static_cast<std::size_t>(other)];
    };
    const auto process_child = [&](int vertex, int child){
        int current = child;
        if(path[static_cast<std::size_t>(current)] == none &&
           degree[static_cast<std::size_t>(current)] <= 1){
            degree[static_cast<std::size_t>(vertex)] +=
                degree[static_cast<std::size_t>(current)];
            low[static_cast<std::size_t>(vertex)] = std::min(
                low[static_cast<std::size_t>(vertex)],
                low[static_cast<std::size_t>(current)]
            );
            return;
        }
        if(degree[static_cast<std::size_t>(current)] == 0){
            current = path[static_cast<std::size_t>(current)];
        }
        if(low[static_cast<std::size_t>(current)] <
           low[static_cast<std::size_t>(vertex)]){
            low[static_cast<std::size_t>(vertex)] =
                low[static_cast<std::size_t>(current)];
            std::swap(
                current, path[static_cast<std::size_t>(vertex)]
            );
        }
        while(current != none){
            absorb(vertex, current);
            current = path[static_cast<std::size_t>(current)];
        }
    };

    struct Frame{
        int vertex;
        int parent;
        std::size_t next_edge;
        bool parent_edge_skipped;
    };
    std::vector<Frame> stack;
    int discovery_count = 0;
    for(int start = 0; start < vertex_count; ++start){
        if(visited[static_cast<std::size_t>(start)]) continue;
        visited[static_cast<std::size_t>(start)] = 1;
        preorder[static_cast<std::size_t>(start)] = discovery_count++;
        stack.push_back({start, none, 0, false});
        while(!stack.empty()){
            Frame& frame = stack.back();
            const int vertex = frame.vertex;
            const auto& adjacent = graph[static_cast<std::size_t>(vertex)];
            if(frame.next_edge == adjacent.size()){
                subtree_end[static_cast<std::size_t>(vertex)] =
                    discovery_count;
                stack.pop_back();
                if(!stack.empty()){
                    process_child(stack.back().vertex, vertex);
                }
                continue;
            }

            const int to = adjacent[frame.next_edge++];
            if(to == vertex) continue;
            if(to == frame.parent && !frame.parent_edge_skipped){
                frame.parent_edge_skipped = true;
                continue;
            }
            if(visited[static_cast<std::size_t>(to)]){
                if(preorder[static_cast<std::size_t>(to)] <
                   preorder[static_cast<std::size_t>(vertex)]){
                    ++degree[static_cast<std::size_t>(vertex)];
                    low[static_cast<std::size_t>(vertex)] = std::min(
                        low[static_cast<std::size_t>(vertex)],
                        preorder[static_cast<std::size_t>(to)]
                    );
                }else{
                    --degree[static_cast<std::size_t>(vertex)];
                    int current = path[static_cast<std::size_t>(vertex)];
                    while(current != none &&
                          preorder[static_cast<std::size_t>(current)] <=
                              preorder[static_cast<std::size_t>(to)] &&
                          preorder[static_cast<std::size_t>(to)] <
                              subtree_end[static_cast<std::size_t>(current)]){
                        absorb(vertex, current);
                        current = path[static_cast<std::size_t>(current)];
                    }
                    path[static_cast<std::size_t>(vertex)] = current;
                }
                continue;
            }

            visited[static_cast<std::size_t>(to)] = 1;
            preorder[static_cast<std::size_t>(to)] = discovery_count++;
            stack.push_back({to, vertex, 0, false});
        }
    }

    ThreeEdgeConnectedComponentsResult result;
    result.id.assign(static_cast<std::size_t>(vertex_count), -1);
    std::vector<unsigned char> enumerated(
        static_cast<std::size_t>(vertex_count)
    );
    for(int start = 0; start < vertex_count; ++start){
        if(enumerated[static_cast<std::size_t>(start)]) continue;
        const int component = static_cast<int>(result.groups.size());
        result.groups.push_back({});
        int vertex = start;
        do{
            enumerated[static_cast<std::size_t>(vertex)] = 1;
            result.id[static_cast<std::size_t>(vertex)] = component;
            result.groups.back().push_back(vertex);
            vertex = component_next[static_cast<std::size_t>(vertex)];
        }while(vertex != start);
    }
    result.count = static_cast<int>(result.groups.size());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_THREE_EDGE_CONNECTED_COMPONENTS_HPP_INCLUDED
