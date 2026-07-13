#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

template<auto Monoid, int MAX_SIZE>
struct DynamicMonochromaticTree{
    static_assert(MAX_SIZE > 0);
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

private:
    static constexpr int SEGMENT_CAPACITY = 4 * MAX_SIZE;
    static constexpr int SEGMENT_TOTAL_CAPACITY = 2 * SEGMENT_CAPACITY;
    static constexpr int UNVISITED = -2;
    static constexpr int INFINITY_COUNT = std::numeric_limits<int>::max() / 4;

    struct MinimumProduct{
        int minimum;
        S product;
    };

    struct State{
        std::array<int, MAX_SIZE> graph_head{};
        std::array<int, 2 * MAX_SIZE> graph_to{};
        std::array<int, 2 * MAX_SIZE> graph_next{};
        int edge_count = 0;

        std::array<int, MAX_SIZE> parent{};
        std::array<int, MAX_SIZE> depth{};
        std::array<int, MAX_SIZE> subtree_size{};
        std::array<int, MAX_SIZE> heavy_child{};
        std::array<int, MAX_SIZE> chain_head{};
        std::array<int, MAX_SIZE> position{};
        std::array<int, MAX_SIZE> inverse_position{};
        std::array<int, MAX_SIZE> order{};
        std::array<int, MAX_SIZE> stack{};

        std::array<unsigned char, MAX_SIZE> color{};
        std::array<S, MAX_SIZE> value;

        std::array<int, SEGMENT_TOTAL_CAPACITY> minimum{};
        std::array<int, SEGMENT_TOTAL_CAPACITY> lazy{};
        std::array<S, SEGMENT_TOTAL_CAPACITY> minimum_product;
        std::array<int, SEGMENT_TOTAL_CAPACITY> color_max_position{};

        int root = -1;
        bool built = false;
    };

    int _n;
    std::unique_ptr<State> state;

    static int segment_index(int color, int node){
        return color * SEGMENT_CAPACITY + node;
    }

    void check_vertex(int vertex, const char* message) const{
        if(vertex < 0 || _n <= vertex)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_built(const char* message) const{
        if(!state->built)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    static MinimumProduct merge_minimum_product(
        const MinimumProduct& left,
        const MinimumProduct& right
    ){
        if(left.minimum < right.minimum) return left;
        if(right.minimum < left.minimum) return right;
        return {left.minimum, Monoid.op(left.product, right.product)};
    }

    void pull_minimum(int color, int node){
        const int index = segment_index(color, node);
        const int left_index = segment_index(color, node << 1);
        const int right_index = segment_index(color, node << 1 | 1);
        const MinimumProduct merged = merge_minimum_product(
            {state->minimum[left_index], state->minimum_product[left_index]},
            {state->minimum[right_index], state->minimum_product[right_index]}
        );
        state->minimum[index] = merged.minimum;
        state->minimum_product[index] = merged.product;
    }

    void apply_minimum(int color, int node, int difference){
        const int index = segment_index(color, node);
        state->minimum[index] += difference;
        state->lazy[index] += difference;
    }

    void push_minimum(int color, int node){
        const int index = segment_index(color, node);
        const int difference = state->lazy[index];
        if(difference == 0) return;
        apply_minimum(color, node << 1, difference);
        apply_minimum(color, node << 1 | 1, difference);
        state->lazy[index] = 0;
    }

    void build_minimum_segment(int color, int node, int left, int right){
        const int index = segment_index(color, node);
        state->lazy[index] = 0;
        if(right - left == 1){
            const int vertex = state->inverse_position[left];
            const int one_count = state->stack[vertex];
            state->minimum[index] = color == 0
                ? one_count
                : state->depth[vertex] + 1 - one_count;
            state->minimum_product[index] = state->value[vertex];
            return;
        }
        const int middle = (left + right) >> 1;
        build_minimum_segment(color, node << 1, left, middle);
        build_minimum_segment(color, node << 1 | 1, middle, right);
        pull_minimum(color, node);
    }

    void range_add_minimum(
        int color,
        int query_left,
        int query_right,
        int difference,
        int node,
        int left,
        int right
    ){
        if(query_right <= left || right <= query_left) return;
        if(query_left <= left && right <= query_right){
            apply_minimum(color, node, difference);
            return;
        }
        push_minimum(color, node);
        const int middle = (left + right) >> 1;
        range_add_minimum(
            color, query_left, query_right, difference, node << 1, left, middle
        );
        range_add_minimum(
            color, query_left, query_right, difference, node << 1 | 1, middle, right
        );
        pull_minimum(color, node);
    }

    void point_set_minimum(
        int color,
        int query_position,
        const S& value,
        int node,
        int left,
        int right
    ){
        if(right - left == 1){
            state->minimum_product[segment_index(color, node)] = value;
            return;
        }
        push_minimum(color, node);
        const int middle = (left + right) >> 1;
        if(query_position < middle){
            point_set_minimum(color, query_position, value, node << 1, left, middle);
        }else{
            point_set_minimum(
                color, query_position, value, node << 1 | 1, middle, right
            );
        }
        pull_minimum(color, node);
    }

    MinimumProduct query_minimum(
        int color,
        int query_left,
        int query_right,
        int node,
        int left,
        int right
    ){
        if(query_right <= left || right <= query_left){
            return {INFINITY_COUNT, Monoid.e()};
        }
        const int index = segment_index(color, node);
        if(query_left <= left && right <= query_right){
            return {state->minimum[index], state->minimum_product[index]};
        }
        push_minimum(color, node);
        const int middle = (left + right) >> 1;
        return merge_minimum_product(
            query_minimum(color, query_left, query_right, node << 1, left, middle),
            query_minimum(
                color, query_left, query_right, node << 1 | 1, middle, right
            )
        );
    }

    void pull_color(int node){
        for(int color = 0; color < 2; ++color){
            state->color_max_position[segment_index(color, node)] = std::max(
                state->color_max_position[segment_index(color, node << 1)],
                state->color_max_position[segment_index(color, node << 1 | 1)]
            );
        }
    }

    void build_color_segment(int node, int left, int right){
        if(right - left == 1){
            const int vertex = state->inverse_position[left];
            const int color = static_cast<int>(state->color[vertex]);
            state->color_max_position[segment_index(color, node)] = left;
            state->color_max_position[segment_index(color ^ 1, node)] = -1;
            return;
        }
        const int middle = (left + right) >> 1;
        build_color_segment(node << 1, left, middle);
        build_color_segment(node << 1 | 1, middle, right);
        pull_color(node);
    }

    void point_set_color(
        int query_position,
        int color,
        int node,
        int left,
        int right
    ){
        if(right - left == 1){
            state->color_max_position[segment_index(color, node)] = left;
            state->color_max_position[segment_index(color ^ 1, node)] = -1;
            return;
        }
        const int middle = (left + right) >> 1;
        if(query_position < middle){
            point_set_color(query_position, color, node << 1, left, middle);
        }else{
            point_set_color(query_position, color, node << 1 | 1, middle, right);
        }
        pull_color(node);
    }

    int query_color_max(
        int color,
        int query_left,
        int query_right,
        int node,
        int left,
        int right
    ) const{
        if(query_right <= left || right <= query_left) return -1;
        if(query_left <= left && right <= query_right){
            return state->color_max_position[segment_index(color, node)];
        }
        const int middle = (left + right) >> 1;
        return std::max(
            query_color_max(color, query_left, query_right, node << 1, left, middle),
            query_color_max(
                color, query_left, query_right, node << 1 | 1, middle, right
            )
        );
    }

    int deepest_ancestor_with_color(int vertex, int color) const{
        int current = vertex;
        while(true){
            const int head = state->chain_head[current];
            const int found_position = query_color_max(
                color,
                state->position[head],
                state->position[current] + 1,
                1,
                0,
                _n
            );
            if(found_position != -1){
                return state->inverse_position[found_position];
            }
            if(head == state->root) return -1;
            current = state->parent[head];
        }
    }

    int child_below(int ancestor, int descendant) const{
        int current = descendant;
        while(state->chain_head[current] != state->chain_head[ancestor]){
            const int head = state->chain_head[current];
            if(state->parent[head] == ancestor) return head;
            current = state->parent[head];
        }
        return state->inverse_position[state->position[ancestor] + 1];
    }

public:
    explicit DynamicMonochromaticTree(int n = MAX_SIZE): _n(n), state(nullptr){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        state = std::make_unique<State>();
        state->graph_head.fill(-1);
        for(int vertex = 0; vertex < _n; ++vertex){
            state->value[vertex] = Monoid.e();
        }
    }

    explicit DynamicMonochromaticTree(
        const std::vector<S>& values,
        const std::vector<bool>& colors = {}
    ): DynamicMonochromaticTree(static_cast<int>(values.size())){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE)
            || (!colors.empty() && colors.size() != values.size()))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        for(int vertex = 0; vertex < _n; ++vertex){
            state->value[vertex] = values[static_cast<std::size_t>(vertex)];
            if(!colors.empty()){
                state->color[vertex] = static_cast<unsigned char>(
                    colors[static_cast<std::size_t>(vertex)]
                );
            }
        }
    }

    DynamicMonochromaticTree(const DynamicMonochromaticTree&) = delete;
    DynamicMonochromaticTree& operator=(const DynamicMonochromaticTree&) = delete;
    DynamicMonochromaticTree(DynamicMonochromaticTree&&) = default;
    DynamicMonochromaticTree& operator=(DynamicMonochromaticTree&&) = default;

    int size() const{ return _n; }

    void add_edge(int left, int right){
        if(state->built)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: edge added after build (add_edge)."
            );
        }
        check_vertex(left, "library assertion fault: range violation (add_edge).");
        check_vertex(right, "library assertion fault: range violation (add_edge).");
        if(state->edge_count / 2 >= _n - 1)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: edge capacity exceeded (add_edge)."
            );
        }
        state->graph_to[state->edge_count] = right;
        state->graph_next[state->edge_count] = state->graph_head[left];
        state->graph_head[left] = state->edge_count++;
        state->graph_to[state->edge_count] = left;
        state->graph_next[state->edge_count] = state->graph_head[right];
        state->graph_head[right] = state->edge_count++;
    }

    void build(int root = 0){
        state->built = false;
        if(_n == 0){
            if(root != 0)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation (build)."
                );
            }
            state->root = -1;
            state->built = true;
            return;
        }
        check_vertex(root, "library assertion fault: range violation (build).");
        if(state->edge_count != 2 * (_n - 1))[[unlikely]]{
            throw std::runtime_error("library assertion fault: graph is not a tree (build).");
        }

        for(int vertex = 0; vertex < _n; ++vertex){
            state->parent[vertex] = UNVISITED;
            state->heavy_child[vertex] = -1;
        }
        int stack_size = 0;
        int order_size = 0;
        state->stack[stack_size++] = root;
        state->parent[root] = -1;
        state->depth[root] = 0;
        while(stack_size != 0){
            const int vertex = state->stack[--stack_size];
            state->order[order_size++] = vertex;
            for(int edge = state->graph_head[vertex]; edge != -1;
                edge = state->graph_next[edge]){
                const int to = state->graph_to[edge];
                if(to == state->parent[vertex]) continue;
                if(state->parent[to] != UNVISITED)[[unlikely]]{
                    throw std::runtime_error(
                        "library assertion fault: graph is not a tree (build)."
                    );
                }
                state->parent[to] = vertex;
                state->depth[to] = state->depth[vertex] + 1;
                state->stack[stack_size++] = to;
            }
        }
        if(order_size != _n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: graph is not a tree (build).");
        }

        for(int index = order_size - 1; index >= 0; --index){
            const int vertex = state->order[index];
            state->subtree_size[vertex] = 1;
            int largest_size = 0;
            for(int edge = state->graph_head[vertex]; edge != -1;
                edge = state->graph_next[edge]){
                const int to = state->graph_to[edge];
                if(state->parent[to] != vertex) continue;
                state->subtree_size[vertex] += state->subtree_size[to];
                if(largest_size < state->subtree_size[to]){
                    largest_size = state->subtree_size[to];
                    state->heavy_child[vertex] = to;
                }
            }
        }

        int timer = 0;
        stack_size = 0;
        state->stack[stack_size++] = root;
        while(stack_size != 0){
            const int chain_start = state->stack[--stack_size];
            for(int vertex = chain_start; vertex != -1;
                vertex = state->heavy_child[vertex]){
                state->chain_head[vertex] = chain_start;
                state->position[vertex] = timer;
                state->inverse_position[timer++] = vertex;
                for(int edge = state->graph_head[vertex]; edge != -1;
                    edge = state->graph_next[edge]){
                    const int to = state->graph_to[edge];
                    if(state->parent[to] == vertex && to != state->heavy_child[vertex]){
                        state->stack[stack_size++] = to;
                    }
                }
            }
        }

        for(int index = 0; index < order_size; ++index){
            const int vertex = state->order[index];
            const int parent = state->parent[vertex];
            state->stack[vertex] = (parent == -1 ? 0 : state->stack[parent])
                + static_cast<int>(state->color[vertex]);
        }
        build_minimum_segment(0, 1, 0, _n);
        build_minimum_segment(1, 1, 0, _n);
        build_color_segment(1, 0, _n);
        state->root = root;
        state->built = true;
    }

    int root() const{
        check_built("library assertion fault: build is required (root).");
        return state->root;
    }

    bool color(int vertex) const{
        check_vertex(vertex, "library assertion fault: range violation (color).");
        return state->color[vertex] != 0;
    }

    bool set_color(int vertex, bool new_color){
        check_vertex(vertex, "library assertion fault: range violation (set_color).");
        const bool old_color = state->color[vertex] != 0;
        if(old_color == new_color) return false;
        if(state->built){
            const int left = state->position[vertex];
            const int right = left + state->subtree_size[vertex];
            const int difference = new_color ? 1 : -1;
            range_add_minimum(0, left, right, difference, 1, 0, _n);
            range_add_minimum(1, left, right, -difference, 1, 0, _n);
            point_set_color(left, static_cast<int>(new_color), 1, 0, _n);
        }
        state->color[vertex] = static_cast<unsigned char>(new_color);
        return true;
    }

    bool toggle(int vertex){
        check_vertex(vertex, "library assertion fault: range violation (toggle).");
        const bool new_color = state->color[vertex] == 0;
        (void)set_color(vertex, new_color);
        return new_color;
    }

    void set(int vertex, const S& new_value){
        check_vertex(vertex, "library assertion fault: range violation (set).");
        state->value[vertex] = new_value;
        if(state->built){
            const int query_position = state->position[vertex];
            point_set_minimum(0, query_position, new_value, 1, 0, _n);
            point_set_minimum(1, query_position, new_value, 1, 0, _n);
        }
    }

    S get(int vertex) const{
        check_vertex(vertex, "library assertion fault: range violation (get).");
        return state->value[vertex];
    }

    S component_prod(int vertex){
        check_built("library assertion fault: build is required (component_prod).");
        check_vertex(vertex, "library assertion fault: range violation (component_prod).");
        const int vertex_color = static_cast<int>(state->color[vertex]);
        const int different_ancestor = deepest_ancestor_with_color(
            vertex, vertex_color ^ 1
        );
        const int component_root = different_ancestor == -1
            ? state->root
            : child_below(different_ancestor, vertex);
        const int left = state->position[component_root];
        const int right = left + state->subtree_size[component_root];
        return query_minimum(vertex_color, left, right, 1, 0, _n).product;
    }
};
