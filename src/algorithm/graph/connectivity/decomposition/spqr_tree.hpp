#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_SPQR_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_SPQR_TREE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

enum class SPQRNodeType{
    S,
    P,
    Q,
    R
};

struct SPQRSkeletonEdge{
    int from;
    int to;
    int original_edge_id;
    int twin_node;
    int twin_edge;

    [[nodiscard]] bool is_virtual() const noexcept{
        return original_edge_id == -1;
    }
};

struct SPQRTreeNode{
    SPQRNodeType type;
    std::vector<SPQRSkeletonEdge> skeleton_edges;
};

struct SPQRTreeResult{
    int vertex_count;
    std::vector<SPQRTreeNode> nodes;
    std::vector<std::pair<int, int>> tree_edges;
    std::vector<int> original_edge_q_node;
};

namespace cpplib_spqr_detail{

struct DisjointSetUnion{
    std::vector<int> parent;
    std::vector<int> size;

    explicit DisjointSetUnion(int element_count)
        : parent(static_cast<std::size_t>(element_count)),
          size(static_cast<std::size_t>(element_count), 1){
        std::iota(parent.begin(), parent.end(), 0);
    }

    int leader(int vertex){
        int root = vertex;
        while(parent[static_cast<std::size_t>(root)] != root){
            root = parent[static_cast<std::size_t>(root)];
        }
        while(parent[static_cast<std::size_t>(vertex)] != vertex){
            int next = parent[static_cast<std::size_t>(vertex)];
            parent[static_cast<std::size_t>(vertex)] = root;
            vertex = next;
        }
        return root;
    }

    void merge(int left, int right){
        left = leader(left);
        right = leader(right);
        if(left == right) return;
        if(size[static_cast<std::size_t>(left)]
            < size[static_cast<std::size_t>(right)]){
            std::swap(left, right);
        }
        parent[static_cast<std::size_t>(right)] = left;
        size[static_cast<std::size_t>(left)]
            += size[static_cast<std::size_t>(right)];
    }
};

struct WorkEdge{
    int from;
    int to;
    int original_edge_id;
    int virtual_token;
};

struct TemporaryNode{
    SPQRNodeType type;
    std::vector<WorkEdge> edges;
};

struct EdgeReference{
    int node = -1;
    int edge = -1;
};

inline std::uint64_t endpoint_key(int left, int right){
    if(left > right) std::swap(left, right);
    return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(left)) << 32)
        | static_cast<std::uint32_t>(right);
}

class SPQRBuilder{
    int vertex_count_;
    int original_edge_count_;
    int next_virtual_token_ = 0;
    std::vector<TemporaryNode> temporary_nodes_;

    static std::vector<int> vertices_of(const std::vector<WorkEdge>& edges){
        std::vector<int> vertices;
        vertices.reserve(edges.size() * 2);
        for(const WorkEdge& edge: edges){
            vertices.push_back(edge.from);
            vertices.push_back(edge.to);
        }
        std::sort(vertices.begin(), vertices.end());
        vertices.erase(
            std::unique(vertices.begin(), vertices.end()), vertices.end()
        );
        return vertices;
    }

    static bool is_cycle(
        const std::vector<WorkEdge>& edges,
        const std::vector<int>& vertices
    ){
        if(vertices.size() < 3 || edges.size() != vertices.size()) return false;
        std::vector<int> degree(vertices.size(), 0);
        for(const WorkEdge& edge: edges){
            std::size_t from = static_cast<std::size_t>(
                std::lower_bound(vertices.begin(), vertices.end(), edge.from)
                    - vertices.begin()
            );
            std::size_t to = static_cast<std::size_t>(
                std::lower_bound(vertices.begin(), vertices.end(), edge.to)
                    - vertices.begin()
            );
            ++degree[from];
            ++degree[to];
        }
        for(int value: degree){
            if(value != 2) return false;
        }
        return true;
    }

    static std::pair<int, int> find_parallel_split(
        const std::vector<WorkEdge>& edges,
        std::size_t vertex_count
    ){
        if(vertex_count <= 2) return {-1, -1};
        std::vector<std::uint64_t> keys;
        keys.reserve(edges.size());
        for(const WorkEdge& edge: edges){
            keys.push_back(endpoint_key(edge.from, edge.to));
        }
        std::sort(keys.begin(), keys.end());
        for(std::size_t index = 1; index < keys.size(); ++index){
            if(keys[index - 1] != keys[index]) continue;
            std::uint64_t key = keys[index];
            int left = static_cast<int>(key >> 32);
            int right = static_cast<int>(static_cast<std::uint32_t>(key));
            return {left, right};
        }
        return {-1, -1};
    }

    static std::pair<int, int> find_separation_pair(
        const std::vector<WorkEdge>& edges,
        const std::vector<int>& vertices
    ){
        const int local_count = static_cast<int>(vertices.size());
        auto local_index = [&](int vertex){
            return static_cast<int>(
                std::lower_bound(vertices.begin(), vertices.end(), vertex)
                    - vertices.begin()
            );
        };
        struct Adjacent{
            int to;
            int edge_id;
        };
        std::vector<std::vector<Adjacent>> graph(
            static_cast<std::size_t>(local_count)
        );
        for(int edge_id = 0; edge_id < static_cast<int>(edges.size());
            ++edge_id){
            const WorkEdge& edge = edges[static_cast<std::size_t>(edge_id)];
            int from = local_index(edge.from);
            int to = local_index(edge.to);
            graph[static_cast<std::size_t>(from)].push_back({to, edge_id});
            graph[static_cast<std::size_t>(to)].push_back({from, edge_id});
        }
        for(int removed = 0; removed < local_count; ++removed){
            std::vector<int> order(
                static_cast<std::size_t>(local_count), -1
            );
            std::vector<int> low(static_cast<std::size_t>(local_count), -1);
            int timer = 0;
            int separating = -1;
            std::function<void(int, int)> dfs =
                [&](int vertex, int parent_edge){
                    order[static_cast<std::size_t>(vertex)] = timer;
                    low[static_cast<std::size_t>(vertex)] = timer;
                    ++timer;
                    int child_count = 0;
                    for(Adjacent adjacent:
                        graph[static_cast<std::size_t>(vertex)]){
                        if(adjacent.to == removed
                            || adjacent.edge_id == parent_edge){
                            continue;
                        }
                        if(order[static_cast<std::size_t>(adjacent.to)] == -1){
                            ++child_count;
                            dfs(adjacent.to, adjacent.edge_id);
                            low[static_cast<std::size_t>(vertex)] = std::min(
                                low[static_cast<std::size_t>(vertex)],
                                low[static_cast<std::size_t>(adjacent.to)]
                            );
                            if(parent_edge != -1
                                && low[static_cast<std::size_t>(adjacent.to)]
                                    >= order[
                                        static_cast<std::size_t>(vertex)
                                    ]){
                                separating = vertex;
                            }
                        }else{
                            low[static_cast<std::size_t>(vertex)] = std::min(
                                low[static_cast<std::size_t>(vertex)],
                                order[
                                    static_cast<std::size_t>(adjacent.to)
                                ]
                            );
                        }
                    }
                    if(parent_edge == -1 && child_count >= 2){
                        separating = vertex;
                    }
                };
            int start = -1;
            for(int vertex = 0; vertex < local_count; ++vertex){
                if(vertex != removed){
                    start = vertex;
                    break;
                }
            }
            if(start == -1) continue;
            dfs(start, -1);
            if(separating != -1){
                return {
                    vertices[static_cast<std::size_t>(removed)],
                    vertices[static_cast<std::size_t>(separating)]
                };
            }
        }
        return {-1, -1};
    }

    static std::vector<std::vector<WorkEdge>> split_groups(
        const std::vector<WorkEdge>& edges,
        int left,
        int right
    ){
        std::vector<int> vertices = vertices_of(edges);
        auto local_index = [&](int vertex){
            return static_cast<int>(
                std::lower_bound(vertices.begin(), vertices.end(), vertex)
                    - vertices.begin()
            );
        };
        std::vector<std::vector<int>> graph(vertices.size());
        for(const WorkEdge& edge: edges){
            if((edge.from == left && edge.to == right)
                || (edge.from == right && edge.to == left)){
                continue;
            }
            if(edge.from == left || edge.from == right
                || edge.to == left || edge.to == right){
                continue;
            }
            int from = local_index(edge.from);
            int to = local_index(edge.to);
            graph[static_cast<std::size_t>(from)].push_back(to);
            graph[static_cast<std::size_t>(to)].push_back(from);
        }
        std::vector<int> component(vertices.size(), -1);
        int component_count = 0;
        for(int start = 0; start < static_cast<int>(vertices.size()); ++start){
            int global = vertices[static_cast<std::size_t>(start)];
            if(global == left || global == right
                || component[static_cast<std::size_t>(start)] != -1){
                continue;
            }
            std::vector<int> stack{start};
            component[static_cast<std::size_t>(start)] = component_count;
            while(!stack.empty()){
                int vertex = stack.back();
                stack.pop_back();
                for(int to: graph[static_cast<std::size_t>(vertex)]){
                    if(component[static_cast<std::size_t>(to)] != -1) continue;
                    component[static_cast<std::size_t>(to)] = component_count;
                    stack.push_back(to);
                }
            }
            ++component_count;
        }
        std::vector<std::vector<WorkEdge>> groups(
            static_cast<std::size_t>(component_count)
        );
        for(const WorkEdge& edge: edges){
            if((edge.from == left && edge.to == right)
                || (edge.from == right && edge.to == left)){
                groups.push_back({edge});
                continue;
            }
            int other = edge.from;
            if(other == left || other == right) other = edge.to;
            int group = component[static_cast<std::size_t>(
                local_index(other)
            )];
            if(group < 0){
                throw std::logic_error(
                    "spqr_tree internal fault: split edge has no component."
                );
            }
            groups[static_cast<std::size_t>(group)].push_back(edge);
        }
        groups.erase(
            std::remove_if(
                groups.begin(), groups.end(),
                [](const std::vector<WorkEdge>& group){
                    return group.empty();
                }
            ),
            groups.end()
        );
        return groups;
    }

    void add_temporary_node(
        SPQRNodeType type,
        std::vector<WorkEdge> edges
    ){
        temporary_nodes_.push_back({type, std::move(edges)});
    }

    void decompose(std::vector<WorkEdge> edges){
        std::vector<int> vertices = vertices_of(edges);
        if(vertices.size() == 2){
            add_temporary_node(SPQRNodeType::P, std::move(edges));
            return;
        }
        if(is_cycle(edges, vertices)){
            add_temporary_node(SPQRNodeType::S, std::move(edges));
            return;
        }
        std::pair<int, int> split =
            find_parallel_split(edges, vertices.size());
        if(split.first == -1){
            split = find_separation_pair(edges, vertices);
        }
        if(split.first == -1){
            add_temporary_node(SPQRNodeType::R, std::move(edges));
            return;
        }
        std::vector<std::vector<WorkEdge>> groups =
            split_groups(edges, split.first, split.second);
        std::vector<WorkEdge>().swap(edges);
        std::vector<int>().swap(vertices);
        if(groups.size() < 2){
            throw std::logic_error(
                "spqr_tree internal fault: invalid separation pair."
            );
        }
        if(groups.size() == 2){
            int token = next_virtual_token_++;
            for(std::vector<WorkEdge>& group: groups){
                group.push_back({
                    split.first, split.second, -1, token
                });
                decompose(std::move(group));
            }
            return;
        }
        std::vector<WorkEdge> parallel_edges;
        parallel_edges.reserve(groups.size());
        for(std::vector<WorkEdge>& group: groups){
            int token = next_virtual_token_++;
            parallel_edges.push_back({
                split.first, split.second, -1, token
            });
            group.push_back({
                split.first, split.second, -1, token
            });
        }
        add_temporary_node(
            SPQRNodeType::P, std::move(parallel_edges)
        );
        for(std::vector<WorkEdge>& group: groups){
            decompose(std::move(group));
        }
    }

    SPQRTreeResult finish(){
        const int temporary_count =
            static_cast<int>(temporary_nodes_.size());
        std::vector<std::vector<EdgeReference>> token_references(
            static_cast<std::size_t>(next_virtual_token_)
        );
        for(int node = 0; node < temporary_count; ++node){
            const auto& edges =
                temporary_nodes_[static_cast<std::size_t>(node)].edges;
            for(int edge = 0; edge < static_cast<int>(edges.size()); ++edge){
                int token =
                    edges[static_cast<std::size_t>(edge)].virtual_token;
                if(token != -1){
                    token_references[static_cast<std::size_t>(token)]
                        .push_back({node, edge});
                }
            }
        }
        DisjointSetUnion dsu(temporary_count);
        for(const auto& references: token_references){
            if(references.size() != 2){
                throw std::logic_error(
                    "spqr_tree internal fault: unmatched virtual edge."
                );
            }
            int left = references[0].node;
            int right = references[1].node;
            SPQRNodeType left_type =
                temporary_nodes_[static_cast<std::size_t>(left)].type;
            SPQRNodeType right_type =
                temporary_nodes_[static_cast<std::size_t>(right)].type;
            if(left_type == right_type
                && (left_type == SPQRNodeType::S
                    || left_type == SPQRNodeType::P)){
                dsu.merge(left, right);
            }
        }
        std::vector<int> group_index(
            static_cast<std::size_t>(temporary_count), -1
        );
        std::vector<TemporaryNode> reduced_nodes;
        for(int node = 0; node < temporary_count; ++node){
            int root = dsu.leader(node);
            if(group_index[static_cast<std::size_t>(root)] == -1){
                group_index[static_cast<std::size_t>(root)] =
                    static_cast<int>(reduced_nodes.size());
                reduced_nodes.push_back({
                    temporary_nodes_[static_cast<std::size_t>(node)].type,
                    {}
                });
            }
            group_index[static_cast<std::size_t>(node)] =
                group_index[static_cast<std::size_t>(root)];
        }
        for(int node = 0; node < temporary_count; ++node){
            int output_node =
                group_index[static_cast<std::size_t>(node)];
            for(const WorkEdge& edge:
                temporary_nodes_[static_cast<std::size_t>(node)].edges){
                if(edge.virtual_token != -1){
                    const auto& references = token_references[
                        static_cast<std::size_t>(edge.virtual_token)
                    ];
                    int other = references[0].node == node
                        ? references[1].node : references[0].node;
                    if(group_index[static_cast<std::size_t>(other)]
                        == output_node){
                        continue;
                    }
                }
                reduced_nodes[static_cast<std::size_t>(output_node)]
                    .edges.push_back(edge);
            }
        }

        SPQRTreeResult result;
        result.vertex_count = vertex_count_;
        result.original_edge_q_node.assign(
            static_cast<std::size_t>(original_edge_count_), -1
        );
        result.nodes.reserve(
            reduced_nodes.size()
                + static_cast<std::size_t>(original_edge_count_)
        );
        for(const TemporaryNode& node: reduced_nodes){
            SPQRTreeNode output{node.type, {}};
            for(const WorkEdge& edge: node.edges){
                output.skeleton_edges.push_back({
                    edge.from, edge.to, -1, -1, -1
                });
            }
            result.nodes.push_back(std::move(output));
        }

        int reduced_count = static_cast<int>(reduced_nodes.size());
        for(int node = 0; node < reduced_count; ++node){
            const TemporaryNode& source =
                reduced_nodes[static_cast<std::size_t>(node)];
            for(const WorkEdge& edge: source.edges){
                if(edge.original_edge_id == -1) continue;
                int q_node = static_cast<int>(result.nodes.size());
                result.original_edge_q_node[
                    static_cast<std::size_t>(edge.original_edge_id)
                ] = q_node;
                result.nodes.push_back({
                    SPQRNodeType::Q,
                    {
                        {
                            edge.from, edge.to, edge.original_edge_id,
                            -1, -1
                        },
                        {edge.from, edge.to, -1, -1, -1}
                    }
                });

            }
        }

        std::vector<std::vector<EdgeReference>> final_references(
            static_cast<std::size_t>(
                next_virtual_token_ + original_edge_count_
            )
        );
        int next_token = next_virtual_token_;
        for(int node = 0; node < reduced_count; ++node){
            const TemporaryNode& source =
                reduced_nodes[static_cast<std::size_t>(node)];
            int output_edge = 0;
            for(const WorkEdge& edge: source.edges){
                int token = edge.original_edge_id == -1
                    ? edge.virtual_token : next_token++;
                final_references[static_cast<std::size_t>(token)]
                    .push_back({node, output_edge});
                ++output_edge;
            }
        }
        next_token = next_virtual_token_;
        for(int node = 0; node < reduced_count; ++node){
            const TemporaryNode& source =
                reduced_nodes[static_cast<std::size_t>(node)];
            for(const WorkEdge& edge: source.edges){
                if(edge.original_edge_id == -1) continue;
                int q_node = result.original_edge_q_node[
                    static_cast<std::size_t>(edge.original_edge_id)
                ];
                final_references[static_cast<std::size_t>(next_token)]
                    .push_back({q_node, 1});
                ++next_token;
            }
        }
        for(const auto& references: final_references){
            if(references.empty()) continue;
            if(references.size() != 2){
                throw std::logic_error(
                    "spqr_tree internal fault: final virtual edge is unmatched."
                );
            }
            EdgeReference left = references[0];
            EdgeReference right = references[1];
            SPQRSkeletonEdge& left_edge =
                result.nodes[static_cast<std::size_t>(left.node)]
                    .skeleton_edges[static_cast<std::size_t>(left.edge)];
            SPQRSkeletonEdge& right_edge =
                result.nodes[static_cast<std::size_t>(right.node)]
                    .skeleton_edges[static_cast<std::size_t>(right.edge)];
            left_edge.twin_node = right.node;
            left_edge.twin_edge = right.edge;
            right_edge.twin_node = left.node;
            right_edge.twin_edge = left.edge;
            result.tree_edges.emplace_back(left.node, right.node);
        }
        return result;
    }

public:
    SPQRBuilder(int vertex_count, int edge_count)
        : vertex_count_(vertex_count), original_edge_count_(edge_count){}

    SPQRTreeResult build(const std::vector<std::pair<int, int>>& edges){
        if(edges.size() == 1){
            SPQRTreeResult result;
            result.vertex_count = vertex_count_;
            result.nodes.push_back({
                SPQRNodeType::Q,
                {{
                    edges[0].first, edges[0].second, 0, -1, -1
                }}
            });
            result.original_edge_q_node = {0};
            return result;
        }
        std::vector<WorkEdge> work_edges;
        work_edges.reserve(edges.size());
        for(int edge_id = 0; edge_id < static_cast<int>(edges.size());
            ++edge_id){
            work_edges.push_back({
                edges[static_cast<std::size_t>(edge_id)].first,
                edges[static_cast<std::size_t>(edge_id)].second,
                edge_id,
                -1
            });
        }
        decompose(std::move(work_edges));
        return finish();
    }
};

inline void validate_biconnected_input(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    if(vertex_count < 2){
        throw std::invalid_argument(
            "spqr_tree requires at least two vertices."
        );
    }
    if(edges.empty()){
        throw std::invalid_argument(
            "spqr_tree requires at least one edge."
        );
    }
    struct Adjacent{
        int to;
        int edge_id;
    };
    std::vector<std::vector<Adjacent>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(int edge_id = 0; edge_id < static_cast<int>(edges.size());
        ++edge_id){
        auto [from, to] = edges[static_cast<std::size_t>(edge_id)];
        if(from < 0 || from >= vertex_count || to < 0 || to >= vertex_count){
            throw std::invalid_argument(
                "spqr_tree edge endpoint is outside [0, vertex_count)."
            );
        }
        if(from == to){
            throw std::invalid_argument(
                "spqr_tree does not accept self-loops."
            );
        }
        graph[static_cast<std::size_t>(from)].push_back({to, edge_id});
        graph[static_cast<std::size_t>(to)].push_back({from, edge_id});
    }
    if(vertex_count == 2) return;
    std::vector<int> order(static_cast<std::size_t>(vertex_count), -1);
    std::vector<int> low(static_cast<std::size_t>(vertex_count), -1);
    int timer = 0;
    bool has_articulation = false;
    std::function<void(int, int)> dfs = [&](int vertex, int parent_edge){
        order[static_cast<std::size_t>(vertex)] = timer;
        low[static_cast<std::size_t>(vertex)] = timer;
        ++timer;
        int child_count = 0;
        for(Adjacent adjacent: graph[static_cast<std::size_t>(vertex)]){
            if(adjacent.edge_id == parent_edge) continue;
            if(order[static_cast<std::size_t>(adjacent.to)] == -1){
                ++child_count;
                dfs(adjacent.to, adjacent.edge_id);
                low[static_cast<std::size_t>(vertex)] = std::min(
                    low[static_cast<std::size_t>(vertex)],
                    low[static_cast<std::size_t>(adjacent.to)]
                );
                if(parent_edge != -1
                    && low[static_cast<std::size_t>(adjacent.to)]
                        >= order[static_cast<std::size_t>(vertex)]){
                    has_articulation = true;
                }
            }else{
                low[static_cast<std::size_t>(vertex)] = std::min(
                    low[static_cast<std::size_t>(vertex)],
                    order[static_cast<std::size_t>(adjacent.to)]
                );
            }
        }
        if(parent_edge == -1 && child_count >= 2){
            has_articulation = true;
        }
    };
    dfs(0, -1);
    if(timer != vertex_count || has_articulation){
        throw std::invalid_argument(
            "spqr_tree requires a biconnected undirected graph."
        );
    }
}

} // namespace cpplib_spqr_detail

inline SPQRTreeResult spqr_tree(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    cpplib_spqr_detail::validate_biconnected_input(vertex_count, edges);
    cpplib_spqr_detail::SPQRBuilder builder(
        vertex_count, static_cast<int>(edges.size())
    );
    return builder.build(edges);
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_SPQR_TREE_HPP_INCLUDED
