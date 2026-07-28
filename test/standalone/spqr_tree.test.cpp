// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <queue>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/connectivity/decomposition/spqr_tree.hpp"

namespace{

int type_index(SPQRNodeType type){
    if(type == SPQRNodeType::S) return 0;
    if(type == SPQRNodeType::P) return 1;
    if(type == SPQRNodeType::Q) return 2;
    return 3;
}

bool connected_after_removing(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges,
    int removed_left,
    int removed_right
){
    int start = -1;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(vertex != removed_left && vertex != removed_right){
            start = vertex;
            break;
        }
    }
    if(start == -1) return true;
    std::vector<std::vector<int>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(auto [from, to]: edges){
        if(from == removed_left || from == removed_right
            || to == removed_left || to == removed_right){
            continue;
        }
        graph[static_cast<std::size_t>(from)].push_back(to);
        graph[static_cast<std::size_t>(to)].push_back(from);
    }
    std::vector<unsigned char> seen(
        static_cast<std::size_t>(vertex_count), 0
    );
    std::queue<int> queue;
    seen[static_cast<std::size_t>(start)] = 1;
    queue.push(start);
    while(!queue.empty()){
        int vertex = queue.front();
        queue.pop();
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            if(seen[static_cast<std::size_t>(to)] != 0) continue;
            seen[static_cast<std::size_t>(to)] = 1;
            queue.push(to);
        }
    }
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        if(vertex != removed_left && vertex != removed_right
            && seen[static_cast<std::size_t>(vertex)] == 0){
            return false;
        }
    }
    return true;
}

bool skeleton_connected_after_removing(
    const SPQRTreeNode& node,
    int removed_left,
    int removed_right
){
    std::set<int> vertices;
    for(const SPQRSkeletonEdge& edge: node.skeleton_edges){
        vertices.insert(edge.from);
        vertices.insert(edge.to);
    }
    int start = -1;
    for(int vertex: vertices){
        if(vertex != removed_left && vertex != removed_right){
            start = vertex;
            break;
        }
    }
    if(start == -1) return true;
    std::vector<std::vector<int>> graph;
    int maximum = *vertices.rbegin();
    graph.resize(static_cast<std::size_t>(maximum + 1));
    for(const SPQRSkeletonEdge& edge: node.skeleton_edges){
        if(edge.from == removed_left || edge.from == removed_right
            || edge.to == removed_left || edge.to == removed_right){
            continue;
        }
        graph[static_cast<std::size_t>(edge.from)].push_back(edge.to);
        graph[static_cast<std::size_t>(edge.to)].push_back(edge.from);
    }
    std::set<int> seen{start};
    std::vector<int> stack{start};
    while(!stack.empty()){
        int vertex = stack.back();
        stack.pop_back();
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            if(seen.insert(to).second) stack.push_back(to);
        }
    }
    for(int vertex: vertices){
        if(vertex != removed_left && vertex != removed_right
            && !seen.contains(vertex)){
            return false;
        }
    }
    return true;
}

int validate(
    int vertex_count,
    const std::vector<std::pair<int, int>>& original_edges,
    const SPQRTreeResult& result
){
    const int node_count = static_cast<int>(result.nodes.size());
    if(result.vertex_count != vertex_count || node_count == 0) return 10;
    if(result.original_edge_q_node.size() != original_edges.size()) return 11;
    if(result.tree_edges.size() + 1
        != static_cast<std::size_t>(node_count)){
        return 12;
    }
    std::vector<std::vector<int>> tree(
        static_cast<std::size_t>(node_count)
    );
    for(auto [from, to]: result.tree_edges){
        if(from < 0 || from >= node_count || to < 0 || to >= node_count
            || from == to){
            return 13;
        }
        tree[static_cast<std::size_t>(from)].push_back(to);
        tree[static_cast<std::size_t>(to)].push_back(from);
    }
    std::vector<unsigned char> reached(
        static_cast<std::size_t>(node_count), 0
    );
    std::vector<int> stack{0};
    reached[0] = 1;
    while(!stack.empty()){
        int node = stack.back();
        stack.pop_back();
        for(int to: tree[static_cast<std::size_t>(node)]){
            if(reached[static_cast<std::size_t>(to)] != 0) continue;
            reached[static_cast<std::size_t>(to)] = 1;
            stack.push_back(to);
        }
    }
    if(std::find(reached.begin(), reached.end(), 0) != reached.end()){
        return 14;
    }
    std::vector<int> real_count(original_edges.size(), 0);
    std::set<std::pair<int, int>> virtual_poles;
    std::set<std::pair<int, int>> series_pairs;
    for(int node_index = 0; node_index < node_count; ++node_index){
        const SPQRTreeNode& node =
            result.nodes[static_cast<std::size_t>(node_index)];
        std::set<int> vertices;
        std::set<std::pair<int, int>> endpoint_pairs;
        std::vector<int> degree(static_cast<std::size_t>(vertex_count), 0);
        int real_in_node = 0;
        for(int edge_index = 0;
            edge_index < static_cast<int>(node.skeleton_edges.size());
            ++edge_index){
            const SPQRSkeletonEdge& edge =
                node.skeleton_edges[static_cast<std::size_t>(edge_index)];
            if(edge.from < 0 || edge.from >= vertex_count || edge.to < 0
                || edge.to >= vertex_count || edge.from == edge.to){
                return 15;
            }
            vertices.insert(edge.from);
            vertices.insert(edge.to);
            ++degree[static_cast<std::size_t>(edge.from)];
            ++degree[static_cast<std::size_t>(edge.to)];
            auto endpoints = std::minmax(edge.from, edge.to);
            endpoint_pairs.emplace(endpoints.first, endpoints.second);
            if(edge.is_virtual()){
                if(edge.twin_node < 0 || edge.twin_node >= node_count){
                    return 16;
                }
                const auto& twin_node =
                    result.nodes[
                        static_cast<std::size_t>(edge.twin_node)
                    ];
                if(edge.twin_edge < 0
                    || edge.twin_edge
                        >= static_cast<int>(twin_node.skeleton_edges.size())){
                    return 17;
                }
                const SPQRSkeletonEdge& twin =
                    twin_node.skeleton_edges[
                        static_cast<std::size_t>(edge.twin_edge)
                    ];
                if(!twin.is_virtual() || twin.twin_node != node_index
                    || twin.twin_edge != edge_index){
                    return 18;
                }
                auto twin_endpoints = std::minmax(twin.from, twin.to);
                if(endpoints != twin_endpoints) return 19;
                virtual_poles.emplace(endpoints.first, endpoints.second);
            }else{
                if(edge.twin_node != -1 || edge.twin_edge != -1
                    || edge.original_edge_id < 0
                    || edge.original_edge_id
                        >= static_cast<int>(original_edges.size())){
                    return 20;
                }
                auto original = original_edges[
                    static_cast<std::size_t>(edge.original_edge_id)
                ];
                if(std::minmax(original.first, original.second) != endpoints){
                    return 21;
                }
                ++real_count[
                    static_cast<std::size_t>(edge.original_edge_id)
                ];
                ++real_in_node;
            }
        }
        if(node.type == SPQRNodeType::Q){
            if(real_in_node != 1
                || (node_count == 1
                    ? node.skeleton_edges.size() != 1
                    : node.skeleton_edges.size() != 2)){
                return 22;
            }
        }else{
            if(real_in_node != 0) return 23;
            if(node.type == SPQRNodeType::S){
                if(vertices.size() < 3
                    || node.skeleton_edges.size() != vertices.size()){
                    return 24;
                }
                for(int left: vertices){
                    for(int right: vertices){
                        if(left < right) series_pairs.emplace(left, right);
                    }
                }
                for(int vertex: vertices){
                    if(degree[static_cast<std::size_t>(vertex)] != 2){
                        return 25;
                    }
                }
            }else if(node.type == SPQRNodeType::P){
                if(vertices.size() != 2
                    || node.skeleton_edges.size() < 2){
                    return 26;
                }
            }else{
                if(vertices.size() < 4
                    || endpoint_pairs.size() != node.skeleton_edges.size()){
                    return 27;
                }
                for(int left: vertices){
                    for(int right: vertices){
                        if(left < right
                            && !skeleton_connected_after_removing(
                                node, left, right
                            )){
                            return 28;
                        }
                    }
                }
            }
        }
        for(int to: tree[static_cast<std::size_t>(node_index)]){
            if(node.type == result.nodes[static_cast<std::size_t>(to)].type
                && (node.type == SPQRNodeType::S
                    || node.type == SPQRNodeType::P)){
                return 29;
            }
        }
    }
    for(std::size_t edge_id = 0; edge_id < original_edges.size(); ++edge_id){
        if(real_count[edge_id] != 1) return 30;
        int q_node = result.original_edge_q_node[edge_id];
        if(q_node < 0 || q_node >= node_count
            || result.nodes[static_cast<std::size_t>(q_node)].type
                != SPQRNodeType::Q){
            return 31;
        }
    }
    for(int left = 0; left < vertex_count; ++left){
        for(int right = left + 1; right < vertex_count; ++right){
            if(!connected_after_removing(
                vertex_count, original_edges, left, right
            ) && !virtual_poles.contains({left, right})
                && !series_pairs.contains({left, right})){
                return 32;
            }
        }
    }
    return 0;
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    bool rejected = false;
    try{
        static_cast<void>(spqr_tree(3, {{0, 1}, {1, 2}}));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 2;

    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int vertex_count;
        int edge_count;
        std::array<int, 4> expected_types;
        std::cin >> vertex_count >> edge_count;
        for(int& value: expected_types) std::cin >> value;
        std::vector<std::pair<int, int>> edges(
            static_cast<std::size_t>(edge_count)
        );
        for(auto& [from, to]: edges) std::cin >> from >> to;
        SPQRTreeResult result = spqr_tree(vertex_count, edges);
        int error = validate(vertex_count, edges, result);
        if(error != 0) return error;
        std::array<int, 4> type_count{};
        for(const SPQRTreeNode& node: result.nodes){
            ++type_count[
                static_cast<std::size_t>(type_index(node.type))
            ];
        }
        for(int index = 0; index < 4; ++index){
            if(expected_types[static_cast<std::size_t>(index)] != -1
                && expected_types[static_cast<std::size_t>(index)]
                    != type_count[static_cast<std::size_t>(index)]){
                return 40 + index;
            }
        }
        std::cout << "ok\n";
    }
    return 0;
}
