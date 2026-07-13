#pragma once

#include <array>
#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

template<std::size_t MAX_VERTEX_COUNT, std::size_t MAX_EDGE_COUNT>
class SingleFailureConnectivity{
    static_assert(
        MAX_VERTEX_COUNT <= static_cast<std::size_t>(std::numeric_limits<int>::max())
    );
    static_assert(
        MAX_EDGE_COUNT <=
        static_cast<std::size_t>(std::numeric_limits<int>::max() / 2)
    );

    struct State{
        State() noexcept{}

        std::array<int, MAX_VERTEX_COUNT> head;
        std::array<int, MAX_VERTEX_COUNT> order;
        std::array<int, MAX_VERTEX_COUNT> low;
        std::array<int, MAX_VERTEX_COUNT> subtree_end;
        std::array<int, MAX_VERTEX_COUNT> parent;
        std::array<int, MAX_VERTEX_COUNT> parent_edge;
        std::array<int, MAX_VERTEX_COUNT> vertex_by_order;
        std::array<int, MAX_VERTEX_COUNT> iterator;
        std::array<int, MAX_VERTEX_COUNT> stack;
        std::array<int, MAX_VERTEX_COUNT> child_count;
        std::array<int, MAX_VERTEX_COUNT> child_write;
        std::array<int, MAX_VERTEX_COUNT + 1> child_offset;
        std::array<int, MAX_VERTEX_COUNT> children;

        std::array<int, MAX_EDGE_COUNT> bridge_child;
        std::array<int, MAX_EDGE_COUNT * 2> arc_to;
        std::array<int, MAX_EDGE_COUNT * 2> arc_edge;
        std::array<int, MAX_EDGE_COUNT * 2> arc_next;
    };

public:
    SingleFailureConnectivity(): state_(new State){}

    SingleFailureConnectivity(
        int vertex_count,
        const std::vector<std::pair<int, int>>& edges
    ): SingleFailureConnectivity(){
        build(vertex_count, edges);
    }

    void build(int vertex_count, const std::vector<std::pair<int, int>>& edges){
        built_ = false;
        vertex_count_ = 0;
        edge_count_ = 0;
        arc_count_ = 0;
        if(vertex_count <= 0 ||
           static_cast<std::size_t>(vertex_count) > MAX_VERTEX_COUNT ||
           edges.size() > MAX_EDGE_COUNT)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation "
                "(SingleFailureConnectivity::build)."
            );
        }
        for(const auto [from, to]: edges){
            if(from < 0 || vertex_count <= from || to < 0 || vertex_count <= to)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation "
                    "(SingleFailureConnectivity::build edge)."
                );
            }
        }

        State& state = *state_;
        vertex_count_ = vertex_count;
        edge_count_ = static_cast<int>(edges.size());
        arc_count_ = 0;
        for(int vertex = 0; vertex < vertex_count_; ++vertex){
            state.head[static_cast<std::size_t>(vertex)] = -1;
            state.order[static_cast<std::size_t>(vertex)] = -1;
            state.low[static_cast<std::size_t>(vertex)] = -1;
            state.subtree_end[static_cast<std::size_t>(vertex)] = -1;
            state.parent[static_cast<std::size_t>(vertex)] = -1;
            state.parent_edge[static_cast<std::size_t>(vertex)] = -1;
            state.child_count[static_cast<std::size_t>(vertex)] = 0;
        }
        for(int edge_id = 0; edge_id < edge_count_; ++edge_id){
            const auto [from, to] = edges[static_cast<std::size_t>(edge_id)];
            state.bridge_child[static_cast<std::size_t>(edge_id)] = -1;
            add_arc(from, to, edge_id);
            add_arc(to, from, edge_id);
        }

        int timer = 0;
        int stack_size = 1;
        state.stack[0] = 0;
        state.order[0] = timer;
        state.vertex_by_order[static_cast<std::size_t>(timer++)] = 0;
        state.low[0] = state.order[0];
        state.iterator[0] = state.head[0];

        while(stack_size > 0){
            const int vertex = state.stack[static_cast<std::size_t>(stack_size - 1)];
            int& arc = state.iterator[static_cast<std::size_t>(vertex)];
            if(arc == -1){
                state.subtree_end[static_cast<std::size_t>(vertex)] = timer;
                --stack_size;
                const int parent = state.parent[static_cast<std::size_t>(vertex)];
                if(parent != -1 &&
                   state.low[static_cast<std::size_t>(vertex)] <
                       state.low[static_cast<std::size_t>(parent)]){
                    state.low[static_cast<std::size_t>(parent)] =
                        state.low[static_cast<std::size_t>(vertex)];
                }
                continue;
            }

            const int current_arc = arc;
            arc = state.arc_next[static_cast<std::size_t>(current_arc)];
            const int edge_id = state.arc_edge[static_cast<std::size_t>(current_arc)];
            if(edge_id == state.parent_edge[static_cast<std::size_t>(vertex)]) continue;

            const int to = state.arc_to[static_cast<std::size_t>(current_arc)];
            if(state.order[static_cast<std::size_t>(to)] == -1){
                state.parent[static_cast<std::size_t>(to)] = vertex;
                state.parent_edge[static_cast<std::size_t>(to)] = edge_id;
                ++state.child_count[static_cast<std::size_t>(vertex)];
                state.order[static_cast<std::size_t>(to)] = timer;
                state.vertex_by_order[static_cast<std::size_t>(timer++)] = to;
                state.low[static_cast<std::size_t>(to)] =
                    state.order[static_cast<std::size_t>(to)];
                state.iterator[static_cast<std::size_t>(to)] =
                    state.head[static_cast<std::size_t>(to)];
                state.stack[static_cast<std::size_t>(stack_size++)] = to;
            }else if(state.order[static_cast<std::size_t>(to)] <
                     state.low[static_cast<std::size_t>(vertex)]){
                state.low[static_cast<std::size_t>(vertex)] =
                    state.order[static_cast<std::size_t>(to)];
            }
        }

        if(timer != vertex_count_)[[unlikely]]{
            vertex_count_ = 0;
            edge_count_ = 0;
            arc_count_ = 0;
            throw std::runtime_error(
                "library assertion fault: graph must be connected "
                "(SingleFailureConnectivity::build)."
            );
        }

        state.child_offset[0] = 0;
        for(int vertex = 0; vertex < vertex_count_; ++vertex){
            state.child_offset[static_cast<std::size_t>(vertex + 1)] =
                state.child_offset[static_cast<std::size_t>(vertex)] +
                state.child_count[static_cast<std::size_t>(vertex)];
            state.child_write[static_cast<std::size_t>(vertex)] =
                state.child_offset[static_cast<std::size_t>(vertex)];
        }
        for(int position = 1; position < vertex_count_; ++position){
            const int child = state.vertex_by_order[static_cast<std::size_t>(position)];
            const int parent = state.parent[static_cast<std::size_t>(child)];
            state.children[static_cast<std::size_t>(
                state.child_write[static_cast<std::size_t>(parent)]++
            )] = child;
            if(state.low[static_cast<std::size_t>(child)] >
               state.order[static_cast<std::size_t>(parent)]){
                state.bridge_child[static_cast<std::size_t>(
                    state.parent_edge[static_cast<std::size_t>(child)]
                )] = child;
            }
        }
        built_ = true;
    }

    int vertex_count() const noexcept{
        return vertex_count_;
    }

    int edge_count() const noexcept{
        return edge_count_;
    }

    bool connected_without_edge(int first, int second, int edge_id) const{
        check_built();
        check_vertex(first);
        check_vertex(second);
        if(edge_id < 0 || edge_count_ <= edge_id)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(SingleFailureConnectivity::connected_without_edge)."
            );
        }
        const int child = state_->bridge_child[static_cast<std::size_t>(edge_id)];
        if(child == -1) return true;
        return is_descendant(child, first) == is_descendant(child, second);
    }

    bool connected_without_vertex(int first, int second, int removed) const{
        check_built();
        check_vertex(first);
        check_vertex(second);
        check_vertex(removed);
        if(first == removed || second == removed) return false;
        return component_after_vertex_removal(first, removed) ==
               component_after_vertex_removal(second, removed);
    }

private:
    void add_arc(int from, int to, int edge_id){
        State& state = *state_;
        state.arc_to[static_cast<std::size_t>(arc_count_)] = to;
        state.arc_edge[static_cast<std::size_t>(arc_count_)] = edge_id;
        state.arc_next[static_cast<std::size_t>(arc_count_)] =
            state.head[static_cast<std::size_t>(from)];
        state.head[static_cast<std::size_t>(from)] = arc_count_++;
    }

    void check_built() const{
        if(!built_)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: object is not built "
                "(SingleFailureConnectivity)."
            );
        }
    }

    void check_vertex(int vertex) const{
        if(vertex < 0 || vertex_count_ <= vertex)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(SingleFailureConnectivity vertex)."
            );
        }
    }

    bool is_descendant(int ancestor, int vertex) const noexcept{
        const State& state = *state_;
        return state.order[static_cast<std::size_t>(ancestor)] <=
                   state.order[static_cast<std::size_t>(vertex)] &&
               state.order[static_cast<std::size_t>(vertex)] <
                   state.subtree_end[static_cast<std::size_t>(ancestor)];
    }

    int child_containing(int ancestor, int vertex) const noexcept{
        const State& state = *state_;
        int left = state.child_offset[static_cast<std::size_t>(ancestor)];
        int right = state.child_offset[static_cast<std::size_t>(ancestor + 1)];
        const int target_order = state.order[static_cast<std::size_t>(vertex)];
        while(left < right){
            const int middle = left + (right - left) / 2;
            const int child = state.children[static_cast<std::size_t>(middle)];
            if(state.order[static_cast<std::size_t>(child)] <= target_order){
                left = middle + 1;
            }else{
                right = middle;
            }
        }
        const int begin = state.child_offset[static_cast<std::size_t>(ancestor)];
        if(left == begin) return -1;
        const int child = state.children[static_cast<std::size_t>(left - 1)];
        return is_descendant(child, vertex) ? child : -1;
    }

    int component_after_vertex_removal(int vertex, int removed) const noexcept{
        const State& state = *state_;
        if(!is_descendant(removed, vertex)) return -1;
        const int child = child_containing(removed, vertex);
        if(child == -1) return -1;
        if(state.low[static_cast<std::size_t>(child)] >=
           state.order[static_cast<std::size_t>(removed)]){
            return child;
        }
        return -1;
    }

    int vertex_count_ = 0;
    int edge_count_ = 0;
    int arc_count_ = 0;
    bool built_ = false;
    std::unique_ptr<State> state_;
};
