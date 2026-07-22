#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_PLANARITY_TEST_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_PLANARITY_TEST_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

namespace planarity_test_internal{

struct Interval{
    int low = -1;
    int high = -1;

    bool empty() const noexcept{ return low == -1; }
};

struct ConflictPair{
    Interval left;
    Interval right;

    void swap_sides() noexcept{ std::swap(left, right); }
};

class LeftRightPlanarity{
    struct Edge{
        int first;
        int second;
    };

    struct OrientationFrame{
        int vertex;
        std::size_t next = 0;
        int pending_edge = -1;
    };

    struct TestingFrame{
        int vertex;
        std::size_t next = 0;
        int pending_edge = -1;
        std::size_t pending_position = 0;
    };

    int vertex_count;
    std::vector<Edge> edges;
    std::vector<std::vector<std::pair<int, int>>> adjacency;
    std::vector<std::vector<int>> outgoing;
    std::vector<unsigned char> oriented;
    std::vector<int> height;
    std::vector<int> parent_edge;
    std::vector<int> lowpoint;
    std::vector<int> second_lowpoint;
    std::vector<int> nesting_depth;
    std::vector<int> reference;
    std::vector<int> lowpoint_edge;
    std::vector<std::size_t> stack_bottom;
    std::vector<ConflictPair> conflict_stack;

    void finish_orientation_edge(int edge, int from){
        nesting_depth[static_cast<std::size_t>(edge)] =
            2 * lowpoint[static_cast<std::size_t>(edge)] +
            static_cast<int>(
                second_lowpoint[static_cast<std::size_t>(edge)] <
                height[static_cast<std::size_t>(from)]
            );
        const int parent = parent_edge[static_cast<std::size_t>(from)];
        if(parent == -1) return;
        if(lowpoint[static_cast<std::size_t>(edge)] <
           lowpoint[static_cast<std::size_t>(parent)]){
            second_lowpoint[static_cast<std::size_t>(parent)] = std::min(
                lowpoint[static_cast<std::size_t>(parent)],
                second_lowpoint[static_cast<std::size_t>(edge)]
            );
            lowpoint[static_cast<std::size_t>(parent)] =
                lowpoint[static_cast<std::size_t>(edge)];
        }else if(lowpoint[static_cast<std::size_t>(parent)] <
                 lowpoint[static_cast<std::size_t>(edge)]){
            second_lowpoint[static_cast<std::size_t>(parent)] = std::min(
                second_lowpoint[static_cast<std::size_t>(parent)],
                lowpoint[static_cast<std::size_t>(edge)]
            );
        }else{
            second_lowpoint[static_cast<std::size_t>(parent)] = std::min(
                second_lowpoint[static_cast<std::size_t>(parent)],
                second_lowpoint[static_cast<std::size_t>(edge)]
            );
        }
    }

    void orient_component(int root){
        height[static_cast<std::size_t>(root)] = 0;
        std::vector<OrientationFrame> stack{{root, 0, -1}};
        while(!stack.empty()){
            OrientationFrame& frame = stack.back();
            if(frame.pending_edge != -1){
                const int edge = frame.pending_edge;
                frame.pending_edge = -1;
                finish_orientation_edge(edge, frame.vertex);
                continue;
            }
            if(frame.next == adjacency[static_cast<std::size_t>(frame.vertex)].size()){
                stack.pop_back();
                continue;
            }
            const auto [neighbor, edge] =
                adjacency[static_cast<std::size_t>(frame.vertex)][frame.next++];
            if(oriented[static_cast<std::size_t>(edge)]) continue;
            oriented[static_cast<std::size_t>(edge)] = 1;
            edges[static_cast<std::size_t>(edge)] = {frame.vertex, neighbor};
            outgoing[static_cast<std::size_t>(frame.vertex)].push_back(edge);
            lowpoint[static_cast<std::size_t>(edge)] =
                height[static_cast<std::size_t>(frame.vertex)];
            second_lowpoint[static_cast<std::size_t>(edge)] =
                height[static_cast<std::size_t>(frame.vertex)];
            if(height[static_cast<std::size_t>(neighbor)] == -1){
                parent_edge[static_cast<std::size_t>(neighbor)] = edge;
                height[static_cast<std::size_t>(neighbor)] =
                    height[static_cast<std::size_t>(frame.vertex)] + 1;
                frame.pending_edge = edge;
                stack.push_back({neighbor, 0, -1});
            }else{
                lowpoint[static_cast<std::size_t>(edge)] =
                    height[static_cast<std::size_t>(neighbor)];
                finish_orientation_edge(edge, frame.vertex);
            }
        }
    }

    bool interval_conflicts(const Interval& interval, int edge) const{
        return !interval.empty() &&
            lowpoint[static_cast<std::size_t>(interval.high)] >
            lowpoint[static_cast<std::size_t>(edge)];
    }

    int lowest(const ConflictPair& pair) const{
        if(pair.left.empty()){
            return lowpoint[static_cast<std::size_t>(pair.right.low)];
        }
        if(pair.right.empty()){
            return lowpoint[static_cast<std::size_t>(pair.left.low)];
        }
        return std::min(
            lowpoint[static_cast<std::size_t>(pair.left.low)],
            lowpoint[static_cast<std::size_t>(pair.right.low)]
        );
    }

    bool add_constraints(int edge, int parent){
        ConflictPair merged;
        do{
            if(conflict_stack.empty()) return false;
            ConflictPair current = conflict_stack.back();
            conflict_stack.pop_back();
            if(!current.left.empty()) current.swap_sides();
            if(!current.left.empty()) return false;
            if(lowpoint[static_cast<std::size_t>(current.right.low)] >
               lowpoint[static_cast<std::size_t>(parent)]){
                if(merged.right.empty()){
                    merged.right = current.right;
                }else{
                    reference[static_cast<std::size_t>(merged.right.low)] =
                        current.right.high;
                }
                merged.right.low = current.right.low;
            }else{
                reference[static_cast<std::size_t>(current.right.low)] =
                    lowpoint_edge[static_cast<std::size_t>(parent)];
            }
        }while(conflict_stack.size() >
               stack_bottom[static_cast<std::size_t>(edge)]);

        while(!conflict_stack.empty() &&
              (interval_conflicts(conflict_stack.back().left, edge) ||
               interval_conflicts(conflict_stack.back().right, edge))){
            ConflictPair current = conflict_stack.back();
            conflict_stack.pop_back();
            if(interval_conflicts(current.right, edge)) current.swap_sides();
            if(interval_conflicts(current.right, edge)) return false;

            if(!merged.right.empty()){
                reference[static_cast<std::size_t>(merged.right.low)] =
                    current.right.high;
            }
            if(!current.right.empty()) merged.right.low = current.right.low;

            if(merged.left.empty()){
                merged.left = current.left;
            }else if(!current.left.empty()){
                reference[static_cast<std::size_t>(merged.left.low)] =
                    current.left.high;
            }
            if(!current.left.empty()) merged.left.low = current.left.low;
        }
        if(!merged.left.empty() || !merged.right.empty()){
            conflict_stack.push_back(merged);
        }
        return true;
    }

    void remove_back_edges(int edge){
        const int parent_vertex = edges[static_cast<std::size_t>(edge)].first;
        while(!conflict_stack.empty() &&
              lowest(conflict_stack.back()) ==
              height[static_cast<std::size_t>(parent_vertex)]){
            conflict_stack.pop_back();
        }
        if(!conflict_stack.empty()){
            ConflictPair pair = conflict_stack.back();
            conflict_stack.pop_back();
            while(pair.left.high != -1 &&
                  edges[static_cast<std::size_t>(pair.left.high)].second ==
                  parent_vertex){
                pair.left.high =
                    reference[static_cast<std::size_t>(pair.left.high)];
            }
            if(pair.left.high == -1 && pair.left.low != -1){
                reference[static_cast<std::size_t>(pair.left.low)] =
                    pair.right.low;
                pair.left.low = -1;
            }
            while(pair.right.high != -1 &&
                  edges[static_cast<std::size_t>(pair.right.high)].second ==
                  parent_vertex){
                pair.right.high =
                    reference[static_cast<std::size_t>(pair.right.high)];
            }
            if(pair.right.high == -1 && pair.right.low != -1){
                reference[static_cast<std::size_t>(pair.right.low)] =
                    pair.left.low;
                pair.right.low = -1;
            }
            conflict_stack.push_back(pair);
        }

        if(lowpoint[static_cast<std::size_t>(edge)] <
           height[static_cast<std::size_t>(parent_vertex)]){
            const ConflictPair& top = conflict_stack.back();
            const int left_high = top.left.high;
            const int right_high = top.right.high;
            if(left_high != -1 &&
               (right_high == -1 ||
                lowpoint[static_cast<std::size_t>(right_high)] <
                lowpoint[static_cast<std::size_t>(left_high)])){
                reference[static_cast<std::size_t>(edge)] = left_high;
            }else{
                reference[static_cast<std::size_t>(edge)] = right_high;
            }
        }
    }

    bool integrate_testing_edge(
        int vertex,
        std::size_t position,
        int edge
    ){
        if(lowpoint[static_cast<std::size_t>(edge)] >=
           height[static_cast<std::size_t>(vertex)]){
            return true;
        }
        const int parent = parent_edge[static_cast<std::size_t>(vertex)];
        if(position == 0){
            lowpoint_edge[static_cast<std::size_t>(parent)] =
                lowpoint_edge[static_cast<std::size_t>(edge)];
            return true;
        }
        return add_constraints(edge, parent);
    }

    bool test_component(int root){
        conflict_stack.clear();
        std::vector<TestingFrame> stack{{root, 0, -1, 0}};
        while(!stack.empty()){
            TestingFrame& frame = stack.back();
            if(frame.pending_edge != -1){
                const int edge = frame.pending_edge;
                const std::size_t position = frame.pending_position;
                frame.pending_edge = -1;
                if(!integrate_testing_edge(frame.vertex, position, edge)){
                    return false;
                }
                continue;
            }
            if(frame.next == outgoing[static_cast<std::size_t>(frame.vertex)].size()){
                const int parent = parent_edge[static_cast<std::size_t>(frame.vertex)];
                if(parent != -1) remove_back_edges(parent);
                stack.pop_back();
                continue;
            }
            const std::size_t position = frame.next;
            const int edge =
                outgoing[static_cast<std::size_t>(frame.vertex)][frame.next++];
            stack_bottom[static_cast<std::size_t>(edge)] = conflict_stack.size();
            const int next_vertex = edges[static_cast<std::size_t>(edge)].second;
            if(parent_edge[static_cast<std::size_t>(next_vertex)] == edge){
                frame.pending_edge = edge;
                frame.pending_position = position;
                stack.push_back({next_vertex, 0, -1, 0});
            }else{
                lowpoint_edge[static_cast<std::size_t>(edge)] = edge;
                conflict_stack.push_back({{}, {edge, edge}});
                if(!integrate_testing_edge(frame.vertex, position, edge)){
                    return false;
                }
            }
        }
        return true;
    }

public:
    LeftRightPlanarity(
        int vertex_count_,
        const std::vector<std::pair<int, int>>& input_edges
    ) : vertex_count(vertex_count_),
        adjacency(static_cast<std::size_t>(vertex_count)),
        outgoing(static_cast<std::size_t>(vertex_count)),
        height(static_cast<std::size_t>(vertex_count), -1),
        parent_edge(static_cast<std::size_t>(vertex_count), -1){
        std::set<std::pair<int, int>> unique_edges;
        for(const auto [first, second]: input_edges){
            if(first < 0 || vertex_count <= first ||
               second < 0 || vertex_count <= second)[[unlikely]]{
                throw std::invalid_argument("planarity test: invalid edge endpoint");
            }
            if(first == second) continue;
            unique_edges.emplace(
                std::min(first, second), std::max(first, second)
            );
        }
        edges.resize(unique_edges.size());
        std::size_t edge_index = 0;
        for(const auto [first, second]: unique_edges){
            edges[edge_index] = {first, second};
            adjacency[static_cast<std::size_t>(first)].emplace_back(
                second, static_cast<int>(edge_index)
            );
            adjacency[static_cast<std::size_t>(second)].emplace_back(
                first, static_cast<int>(edge_index)
            );
            ++edge_index;
        }
        const std::size_t edge_count = edges.size();
        oriented.assign(edge_count, 0);
        lowpoint.assign(edge_count, 0);
        second_lowpoint.assign(edge_count, 0);
        nesting_depth.assign(edge_count, 0);
        reference.assign(edge_count, -1);
        lowpoint_edge.assign(edge_count, -1);
        stack_bottom.assign(edge_count, 0);
    }

    bool run(){
        if(vertex_count > 2 &&
           edges.size() > static_cast<std::size_t>(3 * vertex_count - 6)){
            return false;
        }
        std::vector<int> roots;
        for(int vertex = 0; vertex < vertex_count; ++vertex){
            if(height[static_cast<std::size_t>(vertex)] == -1){
                roots.push_back(vertex);
                orient_component(vertex);
            }
        }
        for(auto& edge_list: outgoing){
            std::stable_sort(
                edge_list.begin(), edge_list.end(),
                [&](int first, int second){
                    return nesting_depth[static_cast<std::size_t>(first)] <
                        nesting_depth[static_cast<std::size_t>(second)];
                }
            );
        }
        for(const int root: roots){
            if(!test_component(root)) return false;
        }
        return true;
    }
};

} // namespace planarity_test_internal

inline bool is_planar_graph(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::invalid_argument("planarity test: negative vertex count");
    }
    return planarity_test_internal::LeftRightPlanarity(
        vertex_count, edges
    ).run();
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_PLANARITY_TEST_HPP_INCLUDED
