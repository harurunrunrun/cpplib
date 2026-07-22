#ifndef CPPLIB_SRC_STRUCTURE_TREE_CENTROID_DISTANCE_INDEX_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TREE_CENTROID_DISTANCE_INDEX_HPP_INCLUDED

#include <array>
#include <bit>
#include <cstddef>
#include <limits>
#include <memory>
#include <span>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace centroid_distance_index_internal{

template<class T>
T checked_add(T left, T right){
    static_assert(std::is_integral_v<T>);
    if constexpr(std::is_signed_v<T>){
        if((right > 0 && left > std::numeric_limits<T>::max() - right)
            || (right < 0 && left < std::numeric_limits<T>::lowest() - right)){
            throw std::overflow_error(
                "library assertion fault: distance overflow."
            );
        }
    }else if(left > std::numeric_limits<T>::max() - right){
        throw std::overflow_error(
            "library assertion fault: distance overflow."
        );
    }
    return static_cast<T>(left + right);
}

} // namespace centroid_distance_index_internal

template<class Distance, int MAX_SIZE>
struct CentroidDistanceIndex{
    static_assert(MAX_SIZE > 0);
    static_assert(std::is_integral_v<Distance>);

    struct Entry{
        int centroid = -1;
        int branch = -1;
        Distance distance{};
    };

    static constexpr int MAX_LOG =
        static_cast<int>(std::bit_width(static_cast<unsigned int>(MAX_SIZE)));
    static constexpr int MAX_BRANCH = 2 * MAX_SIZE;

private:
    static constexpr int EDGE_CAPACITY = 2 * (MAX_SIZE - 1);

    struct State{
        std::array<int, MAX_SIZE> head{};
        std::array<int, EDGE_CAPACITY> to{};
        std::array<int, EDGE_CAPACITY> next{};
        std::array<Distance, EDGE_CAPACITY> weight{};

        std::array<int, MAX_SIZE> centroid_parent{};
        std::array<int, MAX_SIZE> centroid_depth{};
        std::array<int, MAX_SIZE> subtree_size{};
        std::array<int, MAX_SIZE> component_parent{};
        std::array<unsigned char, MAX_SIZE> removed{};

        std::array<std::array<Entry, MAX_LOG>, MAX_SIZE> path{};
        std::array<int, MAX_SIZE> path_size{};
        std::array<int, MAX_BRANCH> branch_centroid{};
    };

    int _n = 0;
    int edge_count = 0;
    int root_centroid = -1;
    int _branch_count = 0;
    bool built = false;
    std::unique_ptr<State> state;

    void check_vertex(int vertex, const char* message) const{
        if(vertex < 0 || _n <= vertex)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void require_built(const char* message) const{
        if(!built)[[unlikely]] throw std::runtime_error(message);
    }

    void initialize_metadata(){
        state->centroid_parent.fill(-1);
        state->centroid_depth.fill(-1);
        state->subtree_size.fill(0);
        state->component_parent.fill(-1);
        state->removed.fill(0);
        state->path_size.fill(0);
        state->branch_centroid.fill(-1);
        root_centroid = -1;
        _branch_count = 0;
    }

    void validate_tree(){
        if(_n == 0){
            if(edge_count != 0)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: graph is not a tree (build)."
                );
            }
            return;
        }
        if(edge_count != 2 * (_n - 1))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: graph is not a tree (build)."
            );
        }

        std::vector<int> stack = {0};
        state->component_parent[0] = -2;
        int visited = 0;
        while(!stack.empty()){
            const int vertex = stack.back();
            stack.pop_back();
            ++visited;
            for(int edge = state->head[vertex]; edge != -1; edge = state->next[edge]){
                const int to = state->to[edge];
                if(to == state->component_parent[vertex]) continue;
                if(state->component_parent[to] != -1)[[unlikely]]{
                    throw std::runtime_error(
                        "library assertion fault: graph is not a tree (build)."
                    );
                }
                state->component_parent[to] = vertex;
                stack.push_back(to);
            }
        }
        if(visited != _n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: graph is not connected (build)."
            );
        }
        state->component_parent.fill(-1);
    }

    void collect_component(int start, std::vector<int>& nodes){
        nodes.clear();
        std::vector<int> stack = {start};
        state->component_parent[start] = -2;
        while(!stack.empty()){
            const int vertex = stack.back();
            stack.pop_back();
            nodes.push_back(vertex);
            for(int edge = state->head[vertex]; edge != -1; edge = state->next[edge]){
                const int to = state->to[edge];
                if(state->removed[to] || to == state->component_parent[vertex]) continue;
                state->component_parent[to] = vertex;
                stack.push_back(to);
            }
        }
    }

    int find_centroid(const std::vector<int>& nodes){
        for(auto iterator = nodes.rbegin(); iterator != nodes.rend(); ++iterator){
            const int vertex = *iterator;
            state->subtree_size[vertex] = 1;
            for(int edge = state->head[vertex]; edge != -1; edge = state->next[edge]){
                const int to = state->to[edge];
                if(!state->removed[to] && state->component_parent[to] == vertex){
                    state->subtree_size[vertex] += state->subtree_size[to];
                }
            }
        }

        const int component_size = static_cast<int>(nodes.size());
        for(const int vertex: nodes){
            int largest = component_size - state->subtree_size[vertex];
            for(int edge = state->head[vertex]; edge != -1; edge = state->next[edge]){
                const int to = state->to[edge];
                if(!state->removed[to] && state->component_parent[to] == vertex
                    && largest < state->subtree_size[to]){
                    largest = state->subtree_size[to];
                }
            }
            if(largest * 2 <= component_size) return vertex;
        }
        throw std::runtime_error(
            "library assertion fault: centroid was not found (build)."
        );
    }

    int make_branch(int centroid){
        if(_branch_count == MAX_BRANCH)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: branch capacity exceeded (build)."
            );
        }
        const int branch = _branch_count++;
        state->branch_centroid[branch] = centroid;
        return branch;
    }

    void append_entry(int vertex, int centroid, int branch, Distance distance){
        int& count = state->path_size[vertex];
        if(count == MAX_LOG)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: centroid path capacity exceeded (build)."
            );
        }
        state->path[vertex][count++] = Entry{centroid, branch, distance};
    }

public:
    explicit CentroidDistanceIndex(int n = MAX_SIZE):
        _n(n), state(nullptr){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        state = std::make_unique<State>();
        state->head.fill(-1);
        initialize_metadata();
    }

    CentroidDistanceIndex(const CentroidDistanceIndex&) = delete;
    CentroidDistanceIndex& operator=(const CentroidDistanceIndex&) = delete;
    CentroidDistanceIndex(CentroidDistanceIndex&&) = default;
    CentroidDistanceIndex& operator=(CentroidDistanceIndex&&) = default;

    int size() const{ return _n; }

    void add_edge(int left, int right, Distance weight){
        if(built)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: edge added after build (add_edge)."
            );
        }
        check_vertex(left, "library assertion fault: range violation (add_edge).");
        check_vertex(right, "library assertion fault: range violation (add_edge).");
        if(edge_count + 2 > EDGE_CAPACITY)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: edge capacity exceeded (add_edge)."
            );
        }
        state->to[edge_count] = right;
        state->weight[edge_count] = weight;
        state->next[edge_count] = state->head[left];
        state->head[left] = edge_count++;
        state->to[edge_count] = left;
        state->weight[edge_count] = weight;
        state->next[edge_count] = state->head[right];
        state->head[right] = edge_count++;
    }

    void build(){
        built = false;
        initialize_metadata();
        validate_tree();
        if(_n == 0){
            built = true;
            return;
        }

        std::vector<std::tuple<int, int, int>> pending = {{0, -1, 0}};
        std::vector<int> nodes;
        nodes.reserve(static_cast<std::size_t>(_n));
        std::vector<std::tuple<int, int, Distance>> distance_stack;
        distance_stack.reserve(static_cast<std::size_t>(_n));
        int processed = 0;

        while(!pending.empty()){
            const auto [start, centroid_parent, centroid_depth] = pending.back();
            pending.pop_back();
            if(state->removed[start]) continue;

            collect_component(start, nodes);
            const int centroid = find_centroid(nodes);
            state->removed[centroid] = 1;
            state->centroid_parent[centroid] = centroid_parent;
            state->centroid_depth[centroid] = centroid_depth;
            if(centroid_parent == -1) root_centroid = centroid;
            ++processed;

            const int self_branch = make_branch(centroid);
            append_entry(centroid, centroid, self_branch, Distance{});

            for(int edge = state->head[centroid]; edge != -1; edge = state->next[edge]){
                const int neighbor = state->to[edge];
                if(state->removed[neighbor]) continue;
                const int branch = make_branch(centroid);
                pending.push_back({neighbor, centroid, centroid_depth + 1});

                distance_stack.clear();
                distance_stack.push_back({
                    neighbor, centroid, state->weight[edge]
                });
                while(!distance_stack.empty()){
                    const auto [vertex, parent, distance] = distance_stack.back();
                    distance_stack.pop_back();
                    append_entry(vertex, centroid, branch, distance);
                    for(int next_edge = state->head[vertex]; next_edge != -1;
                        next_edge = state->next[next_edge]){
                        const int to = state->to[next_edge];
                        if(state->removed[to] || to == parent) continue;
                        distance_stack.push_back({
                            to,
                            vertex,
                            centroid_distance_index_internal::checked_add(
                                distance, state->weight[next_edge]
                            )
                        });
                    }
                }
            }

            for(const int vertex: nodes) state->component_parent[vertex] = -1;
        }

        if(processed != _n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: graph is not connected (build)."
            );
        }
        built = true;
    }

    int root() const{
        require_built("library assertion fault: index is not built (root).");
        return root_centroid;
    }

    int parent(int vertex) const{
        require_built("library assertion fault: index is not built (parent).");
        check_vertex(vertex, "library assertion fault: range violation (parent).");
        return state->centroid_parent[vertex];
    }

    int depth(int vertex) const{
        require_built("library assertion fault: index is not built (depth).");
        check_vertex(vertex, "library assertion fault: range violation (depth).");
        return state->centroid_depth[vertex];
    }

    int branch_count() const{
        require_built(
            "library assertion fault: index is not built (branch_count)."
        );
        return _branch_count;
    }

    int branch_centroid(int branch) const{
        require_built(
            "library assertion fault: index is not built (branch_centroid)."
        );
        if(branch < 0 || _branch_count <= branch)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (branch_centroid)."
            );
        }
        return state->branch_centroid[branch];
    }

    std::span<const Entry> entries(int vertex) const{
        require_built("library assertion fault: index is not built (entries).");
        check_vertex(vertex, "library assertion fault: range violation (entries).");
        return std::span<const Entry>(
            state->path[vertex].data(),
            static_cast<std::size_t>(state->path_size[vertex])
        );
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TREE_CENTROID_DISTANCE_INDEX_HPP_INCLUDED
