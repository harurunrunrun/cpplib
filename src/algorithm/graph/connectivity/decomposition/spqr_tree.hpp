#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_SPQR_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_DECOMPOSITION_SPQR_TREE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
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

struct WorkEdge{
    int from;
    int to;
    int original_edge_id;
    int virtual_token;
    int palm_edge_id = -1;
    int high_node = -1;
};

struct TemporaryNode{
    SPQRNodeType type;
    std::vector<WorkEdge> edges;
};

struct EdgeReference{
    int node = -1;
    int edge = -1;
};

struct CoreEdge{
    int from;
    int to;
    WorkEdge work;
    int tail = -1;
    int head = -1;
    bool tree = false;
    bool starts_path = false;
};

struct Type2Triple{
    int high = -1;
    int left = -1;
    int right = -1;
    bool end_marker = false;
};

class SPQRBuilder{
    struct HighNode{
        int target;
        int source;
        int previous;
        int next;
        bool active;
    };
    int vertex_count_;
    int original_edge_count_;
    int next_virtual_token_ = 0;
    std::vector<TemporaryNode> temporary_nodes_;
    std::vector<int> scratch_degree_;
    std::vector<std::uint64_t> scratch_stamp_;
    std::uint64_t next_scratch_stamp_ = 1;
    std::vector<HighNode> high_nodes_;
    std::vector<int> high_head_;
    std::vector<int> high_tail_;

    void deactivate_high(int node_id){
        if(node_id == -1) return;
        HighNode& node = high_nodes_[static_cast<std::size_t>(node_id)];
        if(!node.active) return;
        if(node.previous == -1){
            high_head_[static_cast<std::size_t>(node.target)] = node.next;
        }else{
            high_nodes_[static_cast<std::size_t>(node.previous)].next =
                node.next;
        }
        if(node.next == -1){
            high_tail_[static_cast<std::size_t>(node.target)] =
                node.previous;
        }else{
            high_nodes_[static_cast<std::size_t>(node.next)].previous =
                node.previous;
        }
        node.active = false;
    }

    [[nodiscard]] int append_high(int target, int source){
        int previous = high_tail_[static_cast<std::size_t>(target)];
        if(previous != -1
            && high_nodes_[static_cast<std::size_t>(previous)].source
                >= source){
            throw std::logic_error(
                "spqr_tree internal fault: highpoints are not ordered."
            );
        }
        int node_id = static_cast<int>(high_nodes_.size());
        high_nodes_.push_back({target, source, previous, -1, true});
        if(previous == -1){
            high_head_[static_cast<std::size_t>(target)] = node_id;
        }else{
            high_nodes_[static_cast<std::size_t>(previous)].next = node_id;
        }
        high_tail_[static_cast<std::size_t>(target)] = node_id;
        return node_id;
    }

    [[nodiscard]] int current_high(int target) const{
        int tail = high_tail_[static_cast<std::size_t>(target)];
        return tail == -1
            ? 0
            : high_nodes_[static_cast<std::size_t>(tail)].source;
    }

    [[nodiscard]] static bool same_endpoints(
        const WorkEdge& edge,
        int left,
        int right
    ) noexcept{
        return (edge.from == left && edge.to == right)
            || (edge.from == right && edge.to == left);
    }

    [[nodiscard]] SPQRNodeType component_type(
        const std::vector<WorkEdge>& edges
    ){
        const std::uint64_t stamp = next_scratch_stamp_++;
        std::vector<int> vertices;
        vertices.reserve(edges.size() * 2);
        for(const WorkEdge& edge: edges){
            for(int vertex: {edge.from, edge.to}){
                if(scratch_stamp_[static_cast<std::size_t>(vertex)] != stamp){
                    scratch_stamp_[static_cast<std::size_t>(vertex)] = stamp;
                    scratch_degree_[static_cast<std::size_t>(vertex)] = 0;
                    vertices.push_back(vertex);
                }
                ++scratch_degree_[static_cast<std::size_t>(vertex)];
            }
        }
        if(vertices.size() == 2) return SPQRNodeType::P;
        if(vertices.size() >= 3 && edges.size() == vertices.size()){
            bool cycle = true;
            for(int vertex: vertices){
                if(scratch_degree_[static_cast<std::size_t>(vertex)] != 2){
                    cycle = false;
                    break;
                }
            }
            if(cycle) return SPQRNodeType::S;
        }
        return SPQRNodeType::R;
    }

    void add_component(std::vector<WorkEdge> edges){
        if(edges.empty()){
            throw std::logic_error(
                "spqr_tree internal fault: an empty split component was made."
            );
        }
        SPQRNodeType type = component_type(edges);
        temporary_nodes_.push_back({type, std::move(edges)});
    }

    [[nodiscard]] WorkEdge split_off(
        std::vector<WorkEdge> removed,
        int left,
        int right,
        std::vector<int>& degree,
        std::vector<unsigned char>& palm_active,
        int preserved_high_node = -1
    ){
        for(const WorkEdge& edge: removed){
            --degree[static_cast<std::size_t>(edge.from)];
            --degree[static_cast<std::size_t>(edge.to)];
            if(edge.palm_edge_id != -1){
                palm_active[static_cast<std::size_t>(
                    edge.palm_edge_id
                )] = 0;
            }
            if(edge.high_node != preserved_high_node){
                deactivate_high(edge.high_node);
            }
        }
        int token = next_virtual_token_++;
        WorkEdge replacement{
            left, right, -1, token, -1, preserved_high_node
        };
        removed.push_back(replacement);
        add_component(std::move(removed));
        ++degree[static_cast<std::size_t>(left)];
        ++degree[static_cast<std::size_t>(right)];
        return replacement;
    }

    template<class Key>
    void stable_counting_sort(
        std::vector<int>& values,
        int key_count,
        Key key
    ) const{
        std::vector<int> count(static_cast<std::size_t>(key_count), 0);
        for(int value: values){
            ++count[static_cast<std::size_t>(key(value))];
        }
        int prefix = 0;
        for(int& amount: count){
            int next = prefix + amount;
            amount = prefix;
            prefix = next;
        }
        std::vector<int> ordered(values.size());
        for(int value: values){
            ordered[static_cast<std::size_t>(
                count[static_cast<std::size_t>(key(value))]++
            )] = value;
        }
        values.swap(ordered);
    }

    std::vector<CoreEdge> separate_input_parallel_edges(
        const std::vector<std::pair<int, int>>& edges
    ){
        std::vector<int> ids(edges.size());
        for(int edge_id = 0; edge_id < static_cast<int>(edges.size());
            ++edge_id){
            ids[static_cast<std::size_t>(edge_id)] = edge_id;
        }
        auto low_endpoint = [&](int edge_id){
            auto [from, to] = edges[static_cast<std::size_t>(edge_id)];
            return std::min(from, to);
        };
        auto high_endpoint = [&](int edge_id){
            auto [from, to] = edges[static_cast<std::size_t>(edge_id)];
            return std::max(from, to);
        };
        stable_counting_sort(ids, vertex_count_, high_endpoint);
        stable_counting_sort(ids, vertex_count_, low_endpoint);

        std::vector<CoreEdge> placed(edges.size());
        std::vector<unsigned char> has_core(edges.size(), 0);
        for(std::size_t begin = 0; begin < ids.size();){
            std::size_t end = begin + 1;
            int first_id = ids[begin];
            int left = low_endpoint(first_id);
            int right = high_endpoint(first_id);
            while(end < ids.size()
                && low_endpoint(ids[end]) == left
                && high_endpoint(ids[end]) == right){
                ++end;
            }
            auto [from, to] = edges[static_cast<std::size_t>(first_id)];
            if(end - begin == 1){
                placed[static_cast<std::size_t>(first_id)] = {
                    from, to, {from, to, first_id, -1}
                };
                has_core[static_cast<std::size_t>(first_id)] = 1;
            }else{
                int token = next_virtual_token_++;
                std::vector<WorkEdge> parallel;
                parallel.reserve(end - begin + 1);
                for(std::size_t index = begin; index < end; ++index){
                    int edge_id = ids[index];
                    auto [edge_from, edge_to] =
                        edges[static_cast<std::size_t>(edge_id)];
                    parallel.push_back({
                        edge_from, edge_to, edge_id, -1
                    });
                }
                parallel.push_back({from, to, -1, token});
                add_component(std::move(parallel));
                placed[static_cast<std::size_t>(first_id)] = {
                    from, to, {from, to, -1, token}
                };
                has_core[static_cast<std::size_t>(first_id)] = 1;
            }
            begin = end;
        }
        std::vector<CoreEdge> core;
        core.reserve(edges.size());
        for(int edge_id = 0; edge_id < static_cast<int>(edges.size());
            ++edge_id){
            if(has_core[static_cast<std::size_t>(edge_id)] != 0){
                core.push_back(
                    placed[static_cast<std::size_t>(edge_id)]
                );
            }
        }
        return core;
    }

    void calculate_palm_tree(
        std::vector<CoreEdge>& core,
        std::vector<std::vector<int>>& ordered_out,
        std::vector<int>& parent,
        std::vector<WorkEdge>& current_tree_edge,
        std::vector<int>& low1,
        std::vector<int>& low1_vertex,
        std::vector<int>& low2,
        std::vector<int>& number,
        std::vector<int>& subtree_size,
        std::vector<int>& high,
        std::vector<int>& first_child,
        std::vector<std::vector<int>>& high_edges
    ){
        std::vector<std::vector<int>> adjacency(
            static_cast<std::size_t>(vertex_count_)
        );
        for(int edge_id = 0; edge_id < static_cast<int>(core.size());
            ++edge_id){
            const CoreEdge& edge = core[static_cast<std::size_t>(edge_id)];
            adjacency[static_cast<std::size_t>(edge.from)].push_back(edge_id);
            adjacency[static_cast<std::size_t>(edge.to)].push_back(edge_id);
        }

        std::vector<int> preorder(
            static_cast<std::size_t>(vertex_count_), -1
        );
        std::vector<int> parent_edge(
            static_cast<std::size_t>(vertex_count_), -1
        );
        std::vector<int> low1_pre(
            static_cast<std::size_t>(vertex_count_), vertex_count_
        );
        std::vector<int> low2_pre(
            static_cast<std::size_t>(vertex_count_), vertex_count_
        );
        std::vector<int> low1_pre_vertex(
            static_cast<std::size_t>(vertex_count_), -1
        );
        auto relax = [&](int vertex, int value, int witness){
            int& first = low1_pre[static_cast<std::size_t>(vertex)];
            int& second = low2_pre[static_cast<std::size_t>(vertex)];
            int& first_vertex =
                low1_pre_vertex[static_cast<std::size_t>(vertex)];
            if(value < first){
                second = first;
                first = value;
                first_vertex = witness;
            }else if(value != first && value < second){
                second = value;
            }
        };
        struct PalmDfsFrame{
            int vertex;
            std::size_t next_edge;
        };
        int timer = 0;
        const int root = core[0].from;
        auto initialize_first_vertex = [&](int vertex){
            preorder[static_cast<std::size_t>(vertex)] = timer++;
            low1_pre[static_cast<std::size_t>(vertex)] =
                preorder[static_cast<std::size_t>(vertex)];
            low1_pre_vertex[static_cast<std::size_t>(vertex)] = vertex;
        };
        initialize_first_vertex(root);
        std::vector<PalmDfsFrame> dfs_stack{{root, 0}};
        while(!dfs_stack.empty()){
            PalmDfsFrame& frame = dfs_stack.back();
            int vertex = frame.vertex;
            const auto& incident =
                adjacency[static_cast<std::size_t>(vertex)];
            if(frame.next_edge == incident.size()){
                dfs_stack.pop_back();
                if(!dfs_stack.empty()){
                    int parent_vertex = dfs_stack.back().vertex;
                    relax(
                        parent_vertex,
                        low1_pre[static_cast<std::size_t>(vertex)],
                        low1_pre_vertex[static_cast<std::size_t>(vertex)]
                    );
                    relax(
                        parent_vertex,
                        low2_pre[static_cast<std::size_t>(vertex)],
                        vertex
                    );
                }
                continue;
            }
            int edge_id = incident[frame.next_edge++];
            if(edge_id == parent_edge[static_cast<std::size_t>(vertex)]){
                continue;
            }
            CoreEdge& edge = core[static_cast<std::size_t>(edge_id)];
            int to = edge.from == vertex ? edge.to : edge.from;
            if(preorder[static_cast<std::size_t>(to)] == -1){
                edge.tree = true;
                edge.tail = vertex;
                edge.head = to;
                parent_edge[static_cast<std::size_t>(to)] = edge_id;
                initialize_first_vertex(to);
                dfs_stack.push_back({to, 0});
            }else if(preorder[static_cast<std::size_t>(to)]
                < preorder[static_cast<std::size_t>(vertex)]){
                edge.tree = false;
                edge.tail = vertex;
                edge.head = to;
                relax(
                    vertex,
                    preorder[static_cast<std::size_t>(to)],
                    to
                );
            }
        }

        struct Incidence{
            int vertex;
            int edge_id;
        };
        const std::size_t potential_count =
            std::size_t{9} * static_cast<std::size_t>(vertex_count_) + 4;
        std::vector<std::vector<Incidence>> buckets(
            potential_count
        );
        for(int vertex = 0; vertex < vertex_count_; ++vertex){
            for(int edge_id: adjacency[static_cast<std::size_t>(vertex)]){
                const CoreEdge& edge =
                    core[static_cast<std::size_t>(edge_id)];
                int to = edge.from == vertex ? edge.to : edge.from;
                std::size_t potential;
                if(edge.tail == vertex){
                    if(edge.tree){
                        if(low2_pre[static_cast<std::size_t>(edge.head)]
                            < preorder[static_cast<std::size_t>(vertex)]){
                            potential = std::size_t{3}
                                * static_cast<std::size_t>(low1_pre[
                                    static_cast<std::size_t>(edge.head)
                                ]);
                        }else{
                            potential = std::size_t{3}
                                * static_cast<std::size_t>(low1_pre[
                                    static_cast<std::size_t>(edge.head)
                                ]) + 2;
                        }
                    }else{
                        potential = std::size_t{3}
                            * static_cast<std::size_t>(preorder[
                                static_cast<std::size_t>(edge.head)
                            ]) + 1;
                    }
                }else{
                    potential = std::size_t{9} * (
                        static_cast<std::size_t>(
                            preorder[static_cast<std::size_t>(to)]
                        ) + 1
                    ) + 3;
                }
                buckets[potential].push_back({vertex, edge_id});
            }
        }
        std::vector<std::vector<int>> sorted_adjacency(
            static_cast<std::size_t>(vertex_count_)
        );
        for(const auto& bucket: buckets){
            for(Incidence incidence: bucket){
                sorted_adjacency[
                    static_cast<std::size_t>(incidence.vertex)
                ].push_back(incidence.edge_id);
            }
        }

        std::fill(preorder.begin(), preorder.end(), -1);
        std::fill(parent_edge.begin(), parent_edge.end(), -1);
        std::fill(low1_pre.begin(), low1_pre.end(), vertex_count_);
        std::fill(low2_pre.begin(), low2_pre.end(), vertex_count_);
        std::fill(low1_pre_vertex.begin(), low1_pre_vertex.end(), -1);
        std::fill(parent.begin(), parent.end(), -1);
        std::fill(subtree_size.begin(), subtree_size.end(), 0);
        for(CoreEdge& edge: core){
            edge.tail = -1;
            edge.head = -1;
            edge.tree = false;
            edge.starts_path = false;
        }
        timer = 0;
        auto initialize_second_vertex = [&](int vertex){
            preorder[static_cast<std::size_t>(vertex)] = timer++;
            low1_pre[static_cast<std::size_t>(vertex)] =
                preorder[static_cast<std::size_t>(vertex)];
            low1_pre_vertex[static_cast<std::size_t>(vertex)] = vertex;
            subtree_size[static_cast<std::size_t>(vertex)] = 1;
        };
        initialize_second_vertex(root);
        dfs_stack.clear();
        dfs_stack.push_back({root, 0});
        while(!dfs_stack.empty()){
            PalmDfsFrame& frame = dfs_stack.back();
            int vertex = frame.vertex;
            const auto& incident =
                sorted_adjacency[static_cast<std::size_t>(vertex)];
            if(frame.next_edge == incident.size()){
                dfs_stack.pop_back();
                if(!dfs_stack.empty()){
                    int parent_vertex = dfs_stack.back().vertex;
                    subtree_size[static_cast<std::size_t>(parent_vertex)]
                        += subtree_size[static_cast<std::size_t>(vertex)];
                    relax(
                        parent_vertex,
                        low1_pre[static_cast<std::size_t>(vertex)],
                        low1_pre_vertex[static_cast<std::size_t>(vertex)]
                    );
                    relax(
                        parent_vertex,
                        low2_pre[static_cast<std::size_t>(vertex)],
                        vertex
                    );
                }
                continue;
            }
            int edge_id = incident[frame.next_edge++];
            if(edge_id == parent_edge[static_cast<std::size_t>(vertex)]){
                continue;
            }
            CoreEdge& edge = core[static_cast<std::size_t>(edge_id)];
            int to = edge.from == vertex ? edge.to : edge.from;
            if(preorder[static_cast<std::size_t>(to)] == -1){
                edge.tree = true;
                edge.tail = vertex;
                edge.head = to;
                parent[static_cast<std::size_t>(to)] = vertex;
                parent_edge[static_cast<std::size_t>(to)] = edge_id;
                current_tree_edge[static_cast<std::size_t>(to)] =
                    edge.work;
                initialize_second_vertex(to);
                dfs_stack.push_back({to, 0});
            }else if(preorder[static_cast<std::size_t>(to)]
                < preorder[static_cast<std::size_t>(vertex)]){
                edge.tree = false;
                edge.tail = vertex;
                edge.head = to;
                relax(
                    vertex,
                    preorder[static_cast<std::size_t>(to)],
                    to
                );
            }
        }

        ordered_out.assign(
            static_cast<std::size_t>(vertex_count_), {}
        );
        for(int vertex = 0; vertex < vertex_count_; ++vertex){
            for(int edge_id:
                sorted_adjacency[static_cast<std::size_t>(vertex)]){
                if(core[static_cast<std::size_t>(edge_id)].tail == vertex){
                    ordered_out[static_cast<std::size_t>(vertex)]
                        .push_back(edge_id);
                }
            }
        }

        bool next_edge_starts_path = true;
        int inverse_postorder = vertex_count_;
        dfs_stack.clear();
        dfs_stack.push_back({root, 0});
        while(!dfs_stack.empty()){
            PalmDfsFrame& frame = dfs_stack.back();
            const auto& outgoing =
                ordered_out[static_cast<std::size_t>(frame.vertex)];
            if(frame.next_edge < outgoing.size()){
                CoreEdge& edge = core[static_cast<std::size_t>(
                    outgoing[frame.next_edge++]
                )];
                if(next_edge_starts_path){
                    edge.starts_path = true;
                    next_edge_starts_path = false;
                }
                if(edge.tree){
                    dfs_stack.push_back({edge.head, 0});
                }else{
                    next_edge_starts_path = true;
                }
            }else{
                number[static_cast<std::size_t>(frame.vertex)] =
                    inverse_postorder--;
                dfs_stack.pop_back();
            }
        }
        std::vector<int> vertex_at_preorder(
            static_cast<std::size_t>(vertex_count_), -1
        );
        for(int vertex = 0; vertex < vertex_count_; ++vertex){
            vertex_at_preorder[static_cast<std::size_t>(
                preorder[static_cast<std::size_t>(vertex)]
            )] = vertex;
        }
        for(int vertex = 0; vertex < vertex_count_; ++vertex){
            int witness =
                low1_pre_vertex[static_cast<std::size_t>(vertex)];
            low1[static_cast<std::size_t>(vertex)] =
                number[static_cast<std::size_t>(witness)];
            low1_vertex[static_cast<std::size_t>(vertex)] = witness;
            int second_pre =
                low2_pre[static_cast<std::size_t>(vertex)];
            low2[static_cast<std::size_t>(vertex)] =
                second_pre == vertex_count_
                ? vertex_count_ + 1
                : number[static_cast<std::size_t>(
                    vertex_at_preorder[
                        static_cast<std::size_t>(second_pre)
                    ]
                )];
            for(int edge_id: ordered_out[static_cast<std::size_t>(vertex)]){
                const CoreEdge& edge =
                    core[static_cast<std::size_t>(edge_id)];
                if(edge.tree){
                    first_child[static_cast<std::size_t>(vertex)] = edge.head;
                    break;
                }
            }
        }
        std::vector<std::vector<int>> source_buckets(
            static_cast<std::size_t>(vertex_count_ + 1)
        );
        for(int edge_id = 0; edge_id < static_cast<int>(core.size());
            ++edge_id){
            const CoreEdge& edge = core[static_cast<std::size_t>(edge_id)];
            if(!edge.tree){
                source_buckets[static_cast<std::size_t>(
                    number[static_cast<std::size_t>(edge.tail)]
                )].push_back(edge_id);
            }
        }
        for(int source = 1; source <= vertex_count_; ++source){
            for(int edge_id:
                source_buckets[static_cast<std::size_t>(source)]){
                int head = core[static_cast<std::size_t>(edge_id)].head;
                high_edges[static_cast<std::size_t>(head)]
                    .push_back(edge_id);
            }
        }
        for(int vertex = 0; vertex < vertex_count_; ++vertex){
            const auto& incoming =
                high_edges[static_cast<std::size_t>(vertex)];
            high[static_cast<std::size_t>(vertex)] = incoming.empty()
                ? 0
                : number[static_cast<std::size_t>(
                    core[static_cast<std::size_t>(incoming.back())].tail
                )];
        }
    }

    void decompose_simple_core(std::vector<CoreEdge>& core){
        std::vector<std::vector<int>> ordered_out;
        std::vector<int> parent(
            static_cast<std::size_t>(vertex_count_), -1
        );
        std::vector<WorkEdge> current_tree_edge(
            static_cast<std::size_t>(vertex_count_), {-1, -1, -1, -1}
        );
        std::vector<int> low1(
            static_cast<std::size_t>(vertex_count_), 0
        );
        std::vector<int> low1_vertex(
            static_cast<std::size_t>(vertex_count_), 0
        );
        std::vector<int> low2(
            static_cast<std::size_t>(vertex_count_), 0
        );
        std::vector<int> number(
            static_cast<std::size_t>(vertex_count_), 0
        );
        std::vector<int> subtree_size(
            static_cast<std::size_t>(vertex_count_), 0
        );
        std::vector<int> high(
            static_cast<std::size_t>(vertex_count_), 0
        );
        std::vector<int> first_child(
            static_cast<std::size_t>(vertex_count_), -1
        );
        std::vector<std::vector<int>> high_edges(
            static_cast<std::size_t>(vertex_count_)
        );
        calculate_palm_tree(
            core, ordered_out, parent, current_tree_edge,
            low1, low1_vertex, low2, number, subtree_size,
            high, first_child, high_edges
        );
        std::vector<unsigned char> palm_active(core.size(), 1);
        for(int target = 0; target < vertex_count_; ++target){
            for(int edge_id:
                high_edges[static_cast<std::size_t>(target)]){
                int source = number[static_cast<std::size_t>(
                    core[static_cast<std::size_t>(edge_id)].tail
                )];
                core[static_cast<std::size_t>(edge_id)].work.high_node =
                    append_high(target, source);
            }
        }

        const int root = core[0].from;
        std::vector<int> degree(
            static_cast<std::size_t>(vertex_count_), 0
        );
        for(const CoreEdge& edge: core){
            ++degree[static_cast<std::size_t>(edge.from)];
            ++degree[static_cast<std::size_t>(edge.to)];
        }
        std::vector<WorkEdge> edge_stack;
        edge_stack.reserve(core.size() * 2);
        std::vector<Type2Triple> triple_stack;
        triple_stack.push_back({-1, -1, -1, true});
        std::vector<int> y_accum(
            static_cast<std::size_t>(vertex_count_), 0
        );

        auto pop_to_marker = [&]{
            while(!triple_stack.empty()){
                bool marker = triple_stack.back().end_marker;
                triple_stack.pop_back();
                if(marker) return;
            }
            throw std::logic_error(
                "spqr_tree internal fault: type-2 marker is missing."
            );
        };
        auto update_triple_stack = [&](const CoreEdge& edge){
            int threshold = edge.tree
                ? low1[static_cast<std::size_t>(edge.head)]
                : number[static_cast<std::size_t>(edge.head)];
            int maximum_high = -1;
            int last_right = -1;
            bool deleted = false;
            while(!triple_stack.empty()
                && !triple_stack.back().end_marker
                && number[static_cast<std::size_t>(
                    triple_stack.back().left
                )] > threshold){
                deleted = true;
                maximum_high = std::max(
                    maximum_high, triple_stack.back().high
                );
                last_right = triple_stack.back().right;
                triple_stack.pop_back();
            }
            if(deleted && edge.tree){
                maximum_high = std::max(
                    maximum_high,
                    y_accum[static_cast<std::size_t>(edge.tail)]
                );
                y_accum[static_cast<std::size_t>(edge.tail)] =
                    maximum_high;
            }
            if(edge.tree){
                int child = edge.head;
                int subtree_high =
                    number[static_cast<std::size_t>(child)]
                    + subtree_size[static_cast<std::size_t>(child)] - 1;
                if(!deleted){
                    triple_stack.push_back({
                        subtree_high,
                        low1_vertex[static_cast<std::size_t>(child)],
                        edge.tail,
                        false
                    });
                }else{
                    triple_stack.push_back({
                        std::max(maximum_high, subtree_high),
                        low1_vertex[static_cast<std::size_t>(child)],
                        last_right,
                        false
                    });
                }
                triple_stack.push_back({-1, -1, -1, true});
            }else if(!deleted){
                triple_stack.push_back({
                    number[static_cast<std::size_t>(edge.tail)],
                    edge.head,
                    edge.tail,
                    false
                });
            }else{
                triple_stack.push_back({
                    maximum_high, edge.head, last_right, false
                });
            }
        };
        auto check_highpoint = [&](int vertex){
            while(!triple_stack.empty()
                && !triple_stack.back().end_marker
                && triple_stack.back().left != vertex
                && triple_stack.back().right != vertex
                && current_high(vertex) > triple_stack.back().high){
                triple_stack.pop_back();
            }
        };
        auto stack_top_is = [&](int left, int right){
            return !edge_stack.empty()
                && same_endpoints(edge_stack.back(), left, right);
        };
        auto check_type2 = [&](int vertex, int initial_child){
            int child = initial_child;
            while(vertex != root){
                bool triple_case =
                    !triple_stack.empty()
                    && !triple_stack.back().end_marker
                    && triple_stack.back().left == vertex;
                int simple_child = -1;
                if(degree[static_cast<std::size_t>(child)] == 2
                    && edge_stack.size() >= 2){
                    for(const WorkEdge* edge: {
                        &edge_stack[edge_stack.size() - 1],
                        &edge_stack[edge_stack.size() - 2]
                    }){
                        for(int endpoint: {edge->from, edge->to}){
                            if(endpoint != vertex && endpoint != child){
                                if(simple_child == -1
                                    || number[static_cast<std::size_t>(
                                        endpoint
                                    )] < number[static_cast<std::size_t>(
                                        simple_child
                                    )]){
                                    simple_child = endpoint;
                                }
                            }
                        }
                    }
                }
                bool simple_case = simple_child != -1
                    && number[static_cast<std::size_t>(simple_child)]
                        > number[static_cast<std::size_t>(child)];
                if(!triple_case && !simple_case) break;
                if(triple_case
                    && parent[static_cast<std::size_t>(
                        triple_stack.back().right
                    )] == vertex){
                    triple_stack.pop_back();
                    continue;
                }

                int right;
                WorkEdge direct{-1, -1, -1, -1};
                bool has_direct = false;
                WorkEdge replacement{-1, -1, -1, -1};
                if(simple_case){
                    right = simple_child;
                    if(edge_stack.size() < 2){
                        throw std::logic_error(
                            "spqr_tree internal fault: short edge stack."
                        );
                    }
                    std::vector<WorkEdge> removed;
                    removed.push_back(edge_stack.back());
                    edge_stack.pop_back();
                    removed.push_back(edge_stack.back());
                    edge_stack.pop_back();
                    replacement = split_off(
                        std::move(removed), vertex, right, degree,
                        palm_active
                    );
                    parent[static_cast<std::size_t>(child)] = -1;
                    if(stack_top_is(vertex, right)){
                        direct = edge_stack.back();
                        edge_stack.pop_back();
                        has_direct = true;
                    }
                }else{
                    Type2Triple triple = triple_stack.back();
                    triple_stack.pop_back();
                    right = triple.right;
                    std::vector<WorkEdge> removed;
                    while(!edge_stack.empty()){
                        const WorkEdge& top = edge_stack.back();
                        int from_number =
                            number[static_cast<std::size_t>(top.from)];
                        int to_number =
                            number[static_cast<std::size_t>(top.to)];
                        if(from_number
                                < number[static_cast<std::size_t>(vertex)]
                            || from_number > triple.high
                            || to_number
                                < number[static_cast<std::size_t>(vertex)]
                            || to_number > triple.high){
                            break;
                        }
                        WorkEdge edge = edge_stack.back();
                        edge_stack.pop_back();
                        if(same_endpoints(edge, vertex, right)){
                            direct = edge;
                            has_direct = true;
                        }else{
                            removed.push_back(edge);
                        }
                    }
                    if(removed.empty()){
                        if(has_direct) edge_stack.push_back(direct);
                        continue;
                    }
                    replacement = split_off(
                        std::move(removed), vertex, right, degree,
                        palm_active
                    );
                    if(child != right){
                        parent[static_cast<std::size_t>(child)] = -1;
                    }
                }
                if(has_direct){
                    replacement = split_off(
                        {direct, replacement}, vertex, right, degree,
                        palm_active
                    );
                }
                edge_stack.push_back(replacement);
                parent[static_cast<std::size_t>(right)] = vertex;
                current_tree_edge[static_cast<std::size_t>(right)] =
                    replacement;
                child = right;
            }
            return child;
        };
        auto check_type1 = [&](
            int vertex,
            int child,
            bool has_future_tree_edge
        ){
            if(low2[static_cast<std::size_t>(child)]
                    < number[static_cast<std::size_t>(vertex)]
                || low1[static_cast<std::size_t>(child)]
                    >= number[static_cast<std::size_t>(vertex)]
                || (parent[static_cast<std::size_t>(vertex)] == root
                    && !has_future_tree_edge)){
                return;
            }
            int first =
                number[static_cast<std::size_t>(child)];
            int after = first
                + subtree_size[static_cast<std::size_t>(child)];
            std::vector<WorkEdge> removed;
            while(!edge_stack.empty()){
                const WorkEdge& top = edge_stack.back();
                int from_number =
                    number[static_cast<std::size_t>(top.from)];
                int to_number =
                    number[static_cast<std::size_t>(top.to)];
                if(!((first <= from_number && from_number < after)
                    || (first <= to_number && to_number < after))){
                    break;
                }
                removed.push_back(top);
                edge_stack.pop_back();
            }
            int low_vertex =
                low1_vertex[static_cast<std::size_t>(child)];
            WorkEdge replacement = split_off(
                std::move(removed), vertex, low_vertex, degree,
                palm_active
            );
            parent[static_cast<std::size_t>(child)] = -1;
            if(stack_top_is(vertex, low_vertex)){
                WorkEdge direct = edge_stack.back();
                edge_stack.pop_back();
                replacement = split_off(
                    {direct, replacement}, vertex, low_vertex, degree,
                    palm_active, direct.high_node
                );
            }
            if(low_vertex
                != parent[static_cast<std::size_t>(vertex)]){
                if(replacement.high_node == -1
                    && current_high(low_vertex)
                        < number[static_cast<std::size_t>(vertex)]){
                    replacement.high_node = append_high(
                        low_vertex,
                        number[static_cast<std::size_t>(vertex)]
                    );
                }
                edge_stack.push_back(replacement);
            }else{
                WorkEdge parent_work =
                    current_tree_edge[static_cast<std::size_t>(vertex)];
                int preserved_high_node = parent_work.high_node != -1
                    ? parent_work.high_node : replacement.high_node;
                replacement = split_off(
                    {replacement, parent_work},
                    low_vertex, vertex, degree, palm_active,
                    preserved_high_node
                );
                current_tree_edge[static_cast<std::size_t>(vertex)] =
                    replacement;
            }
        };

        struct PathSearchFrame{
            int vertex;
            std::size_t next_edge;
            int remaining_tree_edges;
            int returning_edge;
        };
        auto make_path_frame = [&](int vertex){
            const auto& outgoing =
                ordered_out[static_cast<std::size_t>(vertex)];
            int tree_edges = 0;
            for(int edge_id: outgoing){
                if(core[static_cast<std::size_t>(edge_id)].tree){
                    ++tree_edges;
                }
            }
            return PathSearchFrame{vertex, 0, tree_edges, -1};
        };
        std::vector<PathSearchFrame> path_stack;
        path_stack.push_back(make_path_frame(root));
        while(!path_stack.empty()){
            PathSearchFrame& frame = path_stack.back();
            int vertex = frame.vertex;
            if(frame.returning_edge != -1){
                CoreEdge& edge = core[static_cast<std::size_t>(
                    frame.returning_edge
                )];
                frame.returning_edge = -1;
                edge_stack.push_back(
                    current_tree_edge[static_cast<std::size_t>(edge.head)]
                );
                int current_child = check_type2(vertex, edge.head);
                check_type1(
                    vertex, current_child, frame.remaining_tree_edges != 0
                );
                if(edge.starts_path) pop_to_marker();
                check_highpoint(vertex);
                continue;
            }
            const auto& outgoing =
                ordered_out[static_cast<std::size_t>(vertex)];
            if(frame.next_edge == outgoing.size()){
                path_stack.pop_back();
                continue;
            }
            int edge_id = outgoing[frame.next_edge++];
            CoreEdge& edge = core[static_cast<std::size_t>(edge_id)];
            if(edge.starts_path) update_triple_stack(edge);
            if(edge.tree){
                --frame.remaining_tree_edges;
                frame.returning_edge = edge_id;
                int child = edge.head;
                path_stack.push_back(make_path_frame(child));
            }else{
                edge_stack.push_back(edge.work);
            }
        }
        if(edge_stack.empty()){
            throw std::logic_error(
                "spqr_tree internal fault: no final component."
            );
        }
        add_component(std::move(edge_stack));
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
        std::vector<std::vector<int>> merge_graph(
            static_cast<std::size_t>(temporary_count)
        );
        for(const auto& references: token_references){
            if(references.size() != 2){
                throw std::logic_error(
                    "spqr_tree internal fault: unmatched virtual edge."
                );
            }
            int left = references[0].node;
            int right = references[1].node;
            SPQRNodeType type =
                temporary_nodes_[static_cast<std::size_t>(left)].type;
            if(type
                    == temporary_nodes_[static_cast<std::size_t>(right)].type
                && (type == SPQRNodeType::S
                    || type == SPQRNodeType::P)){
                merge_graph[static_cast<std::size_t>(left)].push_back(right);
                merge_graph[static_cast<std::size_t>(right)].push_back(left);
            }
        }
        std::vector<int> group_index(
            static_cast<std::size_t>(temporary_count), -1
        );
        std::vector<TemporaryNode> reduced_nodes;
        for(int start = 0; start < temporary_count; ++start){
            if(group_index[static_cast<std::size_t>(start)] != -1) continue;
            int group = static_cast<int>(reduced_nodes.size());
            reduced_nodes.push_back({
                temporary_nodes_[static_cast<std::size_t>(start)].type, {}
            });
            group_index[static_cast<std::size_t>(start)] = group;
            std::vector<int> stack{start};
            while(!stack.empty()){
                int node = stack.back();
                stack.pop_back();
                for(int to: merge_graph[static_cast<std::size_t>(node)]){
                    if(group_index[static_cast<std::size_t>(to)] != -1){
                        continue;
                    }
                    group_index[static_cast<std::size_t>(to)] = group;
                    stack.push_back(to);
                }
            }
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
            output.skeleton_edges.reserve(node.edges.size());
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
        : vertex_count_(vertex_count),
          original_edge_count_(edge_count),
          scratch_degree_(static_cast<std::size_t>(vertex_count), 0),
          scratch_stamp_(static_cast<std::size_t>(vertex_count), 0),
          high_head_(static_cast<std::size_t>(vertex_count), -1),
          high_tail_(static_cast<std::size_t>(vertex_count), -1){}

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
        if(vertex_count_ == 2){
            std::vector<WorkEdge> parallel;
            parallel.reserve(edges.size());
            for(int edge_id = 0;
                edge_id < static_cast<int>(edges.size());
                ++edge_id){
                auto [from, to] =
                    edges[static_cast<std::size_t>(edge_id)];
                parallel.push_back({from, to, edge_id, -1});
            }
            add_component(std::move(parallel));
            return finish();
        }
        std::vector<CoreEdge> core =
            separate_input_parallel_edges(edges);
        for(int edge_id = 0; edge_id < static_cast<int>(core.size());
            ++edge_id){
            core[static_cast<std::size_t>(edge_id)].work.palm_edge_id =
                edge_id;
        }
        decompose_simple_core(core);
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
    if(vertex_count > std::numeric_limits<int>::max() - 2){
        throw std::length_error(
            "spqr_tree vertex_count is too large."
        );
    }
    constexpr std::size_t maximum_edge_count =
        static_cast<std::size_t>(std::numeric_limits<int>::max()) / 4;
    if(edges.size() > maximum_edge_count){
        throw std::length_error(
            "spqr_tree has too many edges."
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
    std::vector<int> parent_vertex(
        static_cast<std::size_t>(vertex_count), -1
    );
    std::vector<int> parent_edge(
        static_cast<std::size_t>(vertex_count), -1
    );
    struct DfsFrame{
        int vertex;
        std::size_t next_edge;
        int child_count;
    };
    auto initialize_vertex = [&](int vertex){
        order[static_cast<std::size_t>(vertex)] = timer;
        low[static_cast<std::size_t>(vertex)] = timer;
        ++timer;
    };
    initialize_vertex(0);
    std::vector<DfsFrame> stack{{0, 0, 0}};
    while(!stack.empty()){
        DfsFrame& frame = stack.back();
        int vertex = frame.vertex;
        const auto& incident = graph[static_cast<std::size_t>(vertex)];
        if(frame.next_edge == incident.size()){
            int children = frame.child_count;
            stack.pop_back();
            int parent = parent_vertex[static_cast<std::size_t>(vertex)];
            if(parent == -1){
                if(children >= 2) has_articulation = true;
            }else{
                low[static_cast<std::size_t>(parent)] = std::min(
                    low[static_cast<std::size_t>(parent)],
                    low[static_cast<std::size_t>(vertex)]
                );
                if(parent_vertex[static_cast<std::size_t>(parent)] != -1
                    && low[static_cast<std::size_t>(vertex)]
                        >= order[static_cast<std::size_t>(parent)]){
                    has_articulation = true;
                }
            }
            continue;
        }
        Adjacent adjacent = incident[frame.next_edge++];
        if(adjacent.edge_id
            == parent_edge[static_cast<std::size_t>(vertex)]){
            continue;
        }
        if(order[static_cast<std::size_t>(adjacent.to)] == -1){
            ++frame.child_count;
            parent_vertex[static_cast<std::size_t>(adjacent.to)] = vertex;
            parent_edge[static_cast<std::size_t>(adjacent.to)] =
                adjacent.edge_id;
            initialize_vertex(adjacent.to);
            stack.push_back({adjacent.to, 0, 0});
        }else{
            low[static_cast<std::size_t>(vertex)] = std::min(
                low[static_cast<std::size_t>(vertex)],
                order[static_cast<std::size_t>(adjacent.to)]
            );
        }
    }
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
