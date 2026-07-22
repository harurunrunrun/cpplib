#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_TREEWIDTH_TWO_DISTANCE_ORACLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_TREEWIDTH_TWO_DISTANCE_ORACLE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <queue>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "tree_decomposition_width_two.hpp"

template<class Weight>
class TreewidthTwoDistanceOracle{
public:
    struct Edge{
        int from;
        int to;
        Weight weight;
    };

private:
    struct Arc{
        int to;
        Weight weight;
    };

    struct HierarchyNode{
        int parent = -1;
        int depth = 0;
        int subtree_end = 0;
        std::array<int, 3> separator{};
        int separator_size = 0;
    };

    struct DistanceLabel{
        std::array<Weight, 3> to_separator;
        std::array<Weight, 3> from_separator;
    };

    int vertex_count_ = 0;
    Weight infinity_;
    int hierarchy_depth_ = 0;
    std::vector<std::vector<Arc>> graph_;
    std::vector<std::vector<Arc>> reverse_graph_;
    std::vector<std::vector<int>> bag_tree_;
    std::vector<std::vector<int>> bags_;
    std::vector<int> owner_;
    std::vector<int> owned_vertex_;
    std::vector<unsigned char> removed_vertex_;
    std::vector<int> removal_node_;
    std::vector<HierarchyNode> hierarchy_;
    std::vector<int> bag_mark_;
    std::vector<int> bag_parent_;
    std::vector<int> bag_subtree_weight_;
    int bag_stamp_ = 0;
    std::vector<std::vector<DistanceLabel>> labels_;
    std::vector<int> vertices_by_removal_node_;
    std::vector<int> removal_node_offset_;
    std::vector<Weight> dijkstra_distance_;

    [[nodiscard]] Weight saturated_add(Weight left, Weight right) const{
        if(left == infinity_ || right == infinity_ ||
           left > infinity_ - right){
            return infinity_;
        }
        return left + right;
    }

    [[nodiscard]] int active_weight(int bag) const{
        const int vertex = owned_vertex_[static_cast<std::size_t>(bag)];
        return vertex >= 0 && !removed_vertex_[static_cast<std::size_t>(vertex)];
    }

    [[nodiscard]] int weighted_centroid(
        const std::vector<int>& component,
        int total_weight
    ){
        ++bag_stamp_;
        for(const int bag: component){
            bag_mark_[static_cast<std::size_t>(bag)] = bag_stamp_;
            bag_parent_[static_cast<std::size_t>(bag)] = -2;
            bag_subtree_weight_[static_cast<std::size_t>(bag)] = 0;
        }

        std::vector<int> order;
        order.reserve(component.size());
        order.push_back(component.front());
        bag_parent_[static_cast<std::size_t>(component.front())] = -1;
        for(std::size_t index = 0; index < order.size(); ++index){
            const int bag = order[index];
            for(const int next: bag_tree_[static_cast<std::size_t>(bag)]){
                if(bag_mark_[static_cast<std::size_t>(next)] != bag_stamp_ ||
                   bag_parent_[static_cast<std::size_t>(next)] != -2){
                    continue;
                }
                bag_parent_[static_cast<std::size_t>(next)] = bag;
                order.push_back(next);
            }
        }
        if(order.size() != component.size())[[unlikely]]{
            throw std::logic_error(
                "TreewidthTwoDistanceOracle: disconnected bag component"
            );
        }

        for(auto iterator = order.rbegin(); iterator != order.rend(); ++iterator){
            const int bag = *iterator;
            int weight = active_weight(bag);
            for(const int next: bag_tree_[static_cast<std::size_t>(bag)]){
                if(bag_mark_[static_cast<std::size_t>(next)] == bag_stamp_ &&
                   bag_parent_[static_cast<std::size_t>(next)] == bag){
                    weight += bag_subtree_weight_[static_cast<std::size_t>(next)];
                }
            }
            bag_subtree_weight_[static_cast<std::size_t>(bag)] = weight;
        }

        for(const int bag: order){
            int maximum_component_weight =
                total_weight - bag_subtree_weight_[static_cast<std::size_t>(bag)];
            for(const int next: bag_tree_[static_cast<std::size_t>(bag)]){
                if(bag_mark_[static_cast<std::size_t>(next)] == bag_stamp_ &&
                   bag_parent_[static_cast<std::size_t>(next)] == bag){
                    maximum_component_weight = std::max(
                        maximum_component_weight,
                        bag_subtree_weight_[static_cast<std::size_t>(next)]
                    );
                }
            }
            if(maximum_component_weight <= total_weight / 2) return bag;
        }
        throw std::logic_error(
            "TreewidthTwoDistanceOracle: weighted centroid was not found"
        );
    }

    int build_hierarchy(
        const std::vector<int>& component,
        int parent,
        int depth
    ){
        int total_weight = 0;
        for(const int bag: component) total_weight += active_weight(bag);
        if(total_weight == 0) return -1;

        const int centroid = weighted_centroid(component, total_weight);
        const int node = static_cast<int>(hierarchy_.size());
        hierarchy_.push_back({});
        hierarchy_[static_cast<std::size_t>(node)].parent = parent;
        hierarchy_[static_cast<std::size_t>(node)].depth = depth;
        hierarchy_depth_ = std::max(hierarchy_depth_, depth + 1);

        for(const int vertex: bags_[static_cast<std::size_t>(centroid)]){
            if(removed_vertex_[static_cast<std::size_t>(vertex)]) continue;
            auto& current = hierarchy_[static_cast<std::size_t>(node)];
            current.separator[static_cast<std::size_t>(current.separator_size++)] =
                vertex;
            removed_vertex_[static_cast<std::size_t>(vertex)] = 1;
            removal_node_[static_cast<std::size_t>(vertex)] = node;
        }

        std::vector<std::vector<int>> child_components;
        for(const int first: bag_tree_[static_cast<std::size_t>(centroid)]){
            if(bag_mark_[static_cast<std::size_t>(first)] != bag_stamp_) continue;
            std::vector<int> child;
            std::vector<int> stack{first};
            bag_mark_[static_cast<std::size_t>(first)] = -bag_stamp_;
            while(!stack.empty()){
                const int bag = stack.back();
                stack.pop_back();
                child.push_back(bag);
                for(const int next: bag_tree_[static_cast<std::size_t>(bag)]){
                    if(next == centroid ||
                       bag_mark_[static_cast<std::size_t>(next)] != bag_stamp_){
                        continue;
                    }
                    bag_mark_[static_cast<std::size_t>(next)] = -bag_stamp_;
                    stack.push_back(next);
                }
            }
            bool has_active_vertex = false;
            for(const int bag: child){
                if(active_weight(bag) != 0){
                    has_active_vertex = true;
                    break;
                }
            }
            if(has_active_vertex) child_components.push_back(std::move(child));
        }

        for(const auto& child: child_components){
            static_cast<void>(build_hierarchy(child, node, depth + 1));
        }
        hierarchy_[static_cast<std::size_t>(node)].subtree_end =
            static_cast<int>(hierarchy_.size());
        return node;
    }

    void run_dijkstra(int node, int separator_index, bool reverse){
        const int subtree_end =
            hierarchy_[static_cast<std::size_t>(node)].subtree_end;
        const int first = removal_node_offset_[static_cast<std::size_t>(node)];
        const int last = removal_node_offset_[static_cast<std::size_t>(subtree_end)];
        for(int index = first; index < last; ++index){
            const int vertex =
                vertices_by_removal_node_[static_cast<std::size_t>(index)];
            dijkstra_distance_[static_cast<std::size_t>(vertex)] = infinity_;
        }
        const int source = hierarchy_[static_cast<std::size_t>(node)]
            .separator[static_cast<std::size_t>(separator_index)];
        using QueueEntry = std::pair<Weight, int>;
        std::priority_queue<
            QueueEntry,
            std::vector<QueueEntry>,
            std::greater<QueueEntry>
        > queue;
        dijkstra_distance_[static_cast<std::size_t>(source)] = Weight{};
        queue.emplace(Weight{}, source);

        const auto& selected_graph = reverse ? reverse_graph_ : graph_;
        while(!queue.empty()){
            const auto [distance, vertex] = queue.top();
            queue.pop();
            if(distance != dijkstra_distance_[static_cast<std::size_t>(vertex)]){
                continue;
            }
            for(const auto& arc: selected_graph[static_cast<std::size_t>(vertex)]){
                const int removal_node =
                    removal_node_[static_cast<std::size_t>(arc.to)];
                if(removal_node < node || subtree_end <= removal_node) continue;
                const Weight next_distance = saturated_add(distance, arc.weight);
                if(next_distance <
                   dijkstra_distance_[static_cast<std::size_t>(arc.to)]){
                    dijkstra_distance_[static_cast<std::size_t>(arc.to)] =
                        next_distance;
                    queue.emplace(next_distance, arc.to);
                }
            }
        }

        for(int index = first; index < last; ++index){
            const int vertex =
                vertices_by_removal_node_[static_cast<std::size_t>(index)];
            auto& label = labels_[static_cast<std::size_t>(vertex)].back();
            if(reverse){
                label.to_separator[static_cast<std::size_t>(separator_index)] =
                    dijkstra_distance_[static_cast<std::size_t>(vertex)];
            }else{
                label.from_separator[static_cast<std::size_t>(separator_index)] =
                    dijkstra_distance_[static_cast<std::size_t>(vertex)];
            }
        }
    }

    void build_distance_labels(){
        removal_node_offset_.assign(hierarchy_.size() + 1, 0);
        for(const int node: removal_node_){
            ++removal_node_offset_[static_cast<std::size_t>(node + 1)];
        }
        for(std::size_t index = 1; index < removal_node_offset_.size(); ++index){
            removal_node_offset_[index] += removal_node_offset_[index - 1];
        }
        vertices_by_removal_node_.assign(
            static_cast<std::size_t>(vertex_count_), 0
        );
        auto cursor = removal_node_offset_;
        for(int vertex = 0; vertex < vertex_count_; ++vertex){
            const int node = removal_node_[static_cast<std::size_t>(vertex)];
            vertices_by_removal_node_[static_cast<std::size_t>(
                cursor[static_cast<std::size_t>(node)]++
            )] = vertex;
            labels_[static_cast<std::size_t>(vertex)].reserve(
                static_cast<std::size_t>(
                    hierarchy_[static_cast<std::size_t>(node)].depth + 1
                )
            );
        }

        for(int node = 0; node < static_cast<int>(hierarchy_.size()); ++node){
            const auto& current = hierarchy_[static_cast<std::size_t>(node)];
            const int first = removal_node_offset_[static_cast<std::size_t>(node)];
            const int last = removal_node_offset_[
                static_cast<std::size_t>(current.subtree_end)
            ];
            for(int index = first; index < last; ++index){
                const int vertex =
                    vertices_by_removal_node_[static_cast<std::size_t>(index)];
                DistanceLabel label;
                label.to_separator.fill(infinity_);
                label.from_separator.fill(infinity_);
                labels_[static_cast<std::size_t>(vertex)].push_back(label);
            }
            for(int separator_index = 0;
                separator_index < current.separator_size;
                ++separator_index){
                run_dijkstra(node, separator_index, false);
                run_dijkstra(node, separator_index, true);
            }
        }
    }

    void validate_vertex(int vertex) const{
        if(vertex < 0 || vertex_count_ <= vertex)[[unlikely]]{
            throw std::out_of_range(
                "TreewidthTwoDistanceOracle: vertex is out of range"
            );
        }
    }

public:
    explicit TreewidthTwoDistanceOracle(
        int vertex_count,
        const std::vector<Edge>& edges,
        Weight infinity = std::numeric_limits<Weight>::max() / Weight{4}
    ):
        vertex_count_(vertex_count),
        infinity_(infinity)
    {
        if(vertex_count < 0)[[unlikely]]{
            throw std::invalid_argument(
                "TreewidthTwoDistanceOracle: negative vertex count"
            );
        }
        if(!(Weight{} < infinity_))[[unlikely]]{
            throw std::invalid_argument(
                "TreewidthTwoDistanceOracle: infinity must be positive"
            );
        }

        graph_.resize(static_cast<std::size_t>(vertex_count_));
        reverse_graph_.resize(static_cast<std::size_t>(vertex_count_));
        std::set<std::pair<int, int>> undirected_edges;
        for(const auto& edge: edges){
            if(edge.from < 0 || vertex_count_ <= edge.from ||
               edge.to < 0 || vertex_count_ <= edge.to)[[unlikely]]{
                throw std::invalid_argument(
                    "TreewidthTwoDistanceOracle: edge endpoint is out of range"
                );
            }
            if(!(Weight{} <= edge.weight) || !(edge.weight < infinity_))[[unlikely]]{
                throw std::invalid_argument(
                    "TreewidthTwoDistanceOracle: invalid edge weight"
                );
            }
            graph_[static_cast<std::size_t>(edge.from)].push_back({
                edge.to, edge.weight
            });
            reverse_graph_[static_cast<std::size_t>(edge.to)].push_back({
                edge.from, edge.weight
            });
            if(edge.from != edge.to){
                undirected_edges.emplace(
                    std::min(edge.from, edge.to),
                    std::max(edge.from, edge.to)
                );
            }
        }

        const std::vector<std::pair<int, int>> simple_edges(
            undirected_edges.begin(), undirected_edges.end()
        );
        const auto decomposition =
            tree_decomposition_width_two(vertex_count_, simple_edges);
        if(!decomposition.valid)[[unlikely]]{
            throw std::invalid_argument(
                "TreewidthTwoDistanceOracle: underlying graph has treewidth greater than two"
            );
        }
        if(vertex_count_ == 0) return;

        bags_ = decomposition.bags;
        bag_tree_.resize(bags_.size());
        for(const auto [first, second]: decomposition.tree_edges){
            bag_tree_[static_cast<std::size_t>(first)].push_back(second);
            bag_tree_[static_cast<std::size_t>(second)].push_back(first);
        }

        owner_.assign(static_cast<std::size_t>(vertex_count_), -1);
        for(int bag = 0; bag < vertex_count_; ++bag){
            for(const int vertex: bags_[static_cast<std::size_t>(bag)]){
                owner_[static_cast<std::size_t>(vertex)] = std::max(
                    owner_[static_cast<std::size_t>(vertex)], bag
                );
            }
        }
        owned_vertex_.assign(static_cast<std::size_t>(vertex_count_), -1);
        for(int vertex = 0; vertex < vertex_count_; ++vertex){
            const int bag = owner_[static_cast<std::size_t>(vertex)];
            if(bag < 0 || owned_vertex_[static_cast<std::size_t>(bag)] >= 0)[[unlikely]]{
                throw std::logic_error(
                    "TreewidthTwoDistanceOracle: invalid elimination ownership"
                );
            }
            owned_vertex_[static_cast<std::size_t>(bag)] = vertex;
        }

        removed_vertex_.assign(static_cast<std::size_t>(vertex_count_), 0);
        removal_node_.assign(static_cast<std::size_t>(vertex_count_), -1);
        bag_mark_.assign(static_cast<std::size_t>(vertex_count_), 0);
        bag_parent_.resize(static_cast<std::size_t>(vertex_count_));
        bag_subtree_weight_.resize(static_cast<std::size_t>(vertex_count_));
        labels_.resize(static_cast<std::size_t>(vertex_count_));
        dijkstra_distance_.resize(static_cast<std::size_t>(vertex_count_));

        std::vector<int> all_bags(static_cast<std::size_t>(vertex_count_));
        for(int bag = 0; bag < vertex_count_; ++bag){
            all_bags[static_cast<std::size_t>(bag)] = bag;
        }
        static_cast<void>(build_hierarchy(all_bags, -1, 0));
        if(std::any_of(
            removal_node_.begin(), removal_node_.end(),
            [](int node){ return node < 0; }
        ))[[unlikely]]{
            throw std::logic_error(
                "TreewidthTwoDistanceOracle: hierarchy omitted a vertex"
            );
        }
        build_distance_labels();
    }

    [[nodiscard]] int size() const noexcept{
        return vertex_count_;
    }

    [[nodiscard]] Weight infinity() const noexcept{
        return infinity_;
    }

    [[nodiscard]] int hierarchy_depth() const noexcept{
        return hierarchy_depth_;
    }

    [[nodiscard]] Weight distance(int from, int to) const{
        validate_vertex(from);
        validate_vertex(to);
        if(from == to) return Weight{};

        int left_node = removal_node_[static_cast<std::size_t>(from)];
        int right_node = removal_node_[static_cast<std::size_t>(to)];
        while(hierarchy_[static_cast<std::size_t>(left_node)].depth >
              hierarchy_[static_cast<std::size_t>(right_node)].depth){
            left_node = hierarchy_[static_cast<std::size_t>(left_node)].parent;
        }
        while(hierarchy_[static_cast<std::size_t>(right_node)].depth >
              hierarchy_[static_cast<std::size_t>(left_node)].depth){
            right_node = hierarchy_[static_cast<std::size_t>(right_node)].parent;
        }
        while(left_node != right_node){
            left_node = hierarchy_[static_cast<std::size_t>(left_node)].parent;
            right_node = hierarchy_[static_cast<std::size_t>(right_node)].parent;
        }
        const int common_label_count =
            hierarchy_[static_cast<std::size_t>(left_node)].depth + 1;

        Weight answer = infinity_;
        for(int level = 0; level < common_label_count; ++level){
            const auto& left =
                labels_[static_cast<std::size_t>(from)][static_cast<std::size_t>(level)];
            const auto& right =
                labels_[static_cast<std::size_t>(to)][static_cast<std::size_t>(level)];
            for(int separator_index = 0; separator_index < 3; ++separator_index){
                answer = std::min(
                    answer,
                    saturated_add(
                        left.to_separator[static_cast<std::size_t>(separator_index)],
                        right.from_separator[static_cast<std::size_t>(separator_index)]
                    )
                );
            }
        }
        return answer;
    }

    [[nodiscard]] bool reachable(int from, int to) const{
        return distance(from, to) != infinity_;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_TREEWIDTH_TWO_DISTANCE_ORACLE_HPP_INCLUDED
