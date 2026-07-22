#ifndef CPPLIB_SRC_ALGORITHM_TREE_TREE_PATH_ORDER_STATISTICS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_TREE_PATH_ORDER_STATISTICS_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

template<class Value, int MAX_SIZE>
struct TreePathOrderStatistics{
    static_assert(MAX_SIZE > 0);

private:
    static constexpr int MAX_LOG = static_cast<int>(
        std::bit_width(static_cast<unsigned int>(MAX_SIZE))
    );
    static constexpr std::size_t EDGE_CAPACITY =
        static_cast<std::size_t>(2) * static_cast<std::size_t>(MAX_SIZE - 1);
    static constexpr std::size_t NODE_CAPACITY =
        static_cast<std::size_t>(1)
        + static_cast<std::size_t>(MAX_SIZE)
            * static_cast<std::size_t>(MAX_LOG + 1);
    static_assert(NODE_CAPACITY <= static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ));

    struct Node{
        int left = 0;
        int right = 0;
        int count = 0;
    };

    struct State{
        std::array<int, MAX_SIZE> head{};
        std::array<int, EDGE_CAPACITY> to{};
        std::array<int, EDGE_CAPACITY> next{};
        std::array<int, MAX_SIZE> parent{};
        std::array<int, MAX_SIZE> depth{};
        std::array<std::array<int, MAX_SIZE>, MAX_LOG> up{};
        std::array<int, MAX_SIZE> rank{};
        std::array<int, MAX_SIZE> version_root{};
        std::array<Node, NODE_CAPACITY> nodes{};
    };

    struct Versions{
        int first;
        int second;
        int subtract_first;
        int subtract_second;
    };

    int _n;
    int edge_count = 0;
    int node_count = 1;
    int root_vertex = -1;
    bool built = false;
    std::unique_ptr<State> state;
    std::vector<Value> compressed_values;

    void check_vertex(int vertex, const char* message) const{
        if(vertex < 0 || _n <= vertex)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void require_built(const char* message) const{
        if(!built)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    int clone_node(int source){
        if(static_cast<std::size_t>(node_count) == NODE_CAPACITY)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: node capacity exceeded (build)."
            );
        }
        const int result = node_count++;
        state->nodes[static_cast<std::size_t>(result)] =
            state->nodes[static_cast<std::size_t>(source)];
        return result;
    }

    int add_version(int old_root, int left, int right, int position){
        const int current = clone_node(old_root);
        ++state->nodes[static_cast<std::size_t>(current)].count;
        if(right - left == 1){
            return current;
        }
        const int middle = left + (right - left) / 2;
        if(position < middle){
            state->nodes[static_cast<std::size_t>(current)].left = add_version(
                state->nodes[static_cast<std::size_t>(old_root)].left,
                left,
                middle,
                position
            );
        }else{
            state->nodes[static_cast<std::size_t>(current)].right = add_version(
                state->nodes[static_cast<std::size_t>(old_root)].right,
                middle,
                right,
                position
            );
        }
        return current;
    }

    int jump_unchecked(int vertex, int distance) const{
        for(int level = 0; distance != 0; ++level){
            if((distance & 1) != 0){
                vertex = state->up[static_cast<std::size_t>(level)]
                    [static_cast<std::size_t>(vertex)];
            }
            distance >>= 1;
        }
        return vertex;
    }

    int lca_unchecked(int left, int right) const{
        if(state->depth[static_cast<std::size_t>(left)]
            < state->depth[static_cast<std::size_t>(right)]){
            std::swap(left, right);
        }
        left = jump_unchecked(
            left,
            state->depth[static_cast<std::size_t>(left)]
                - state->depth[static_cast<std::size_t>(right)]
        );
        if(left == right){
            return left;
        }
        for(int level = MAX_LOG - 1; level >= 0; --level){
            const int left_ancestor = state->up[static_cast<std::size_t>(level)]
                [static_cast<std::size_t>(left)];
            const int right_ancestor = state->up[static_cast<std::size_t>(level)]
                [static_cast<std::size_t>(right)];
            if(left_ancestor != right_ancestor){
                left = left_ancestor;
                right = right_ancestor;
            }
        }
        return state->parent[static_cast<std::size_t>(left)];
    }

    Versions path_versions_unchecked(int left, int right) const{
        const int ancestor = lca_unchecked(left, right);
        const int ancestor_parent = state->parent[static_cast<std::size_t>(ancestor)];
        return Versions{
            state->version_root[static_cast<std::size_t>(left)],
            state->version_root[static_cast<std::size_t>(right)],
            state->version_root[static_cast<std::size_t>(ancestor)],
            ancestor_parent == -1
                ? 0
                : state->version_root[static_cast<std::size_t>(ancestor_parent)]
        };
    }

    int combined_count(const Versions& versions) const{
        return state->nodes[static_cast<std::size_t>(versions.first)].count
            + state->nodes[static_cast<std::size_t>(versions.second)].count
            - state->nodes[static_cast<std::size_t>(versions.subtract_first)].count
            - state->nodes[static_cast<std::size_t>(versions.subtract_second)].count;
    }

    Versions children(const Versions& versions, bool right_child) const{
        const auto child = [&](int node){
            return right_child
                ? state->nodes[static_cast<std::size_t>(node)].right
                : state->nodes[static_cast<std::size_t>(node)].left;
        };
        return Versions{
            child(versions.first),
            child(versions.second),
            child(versions.subtract_first),
            child(versions.subtract_second)
        };
    }

    Value kth_smallest_unchecked(Versions versions, int order) const{
        int left = 0;
        int right = static_cast<int>(compressed_values.size());
        while(right - left > 1){
            const int middle = left + (right - left) / 2;
            const Versions left_versions = children(versions, false);
            const int left_count = combined_count(left_versions);
            if(order < left_count){
                versions = left_versions;
                right = middle;
            }else{
                order -= left_count;
                versions = children(versions, true);
                left = middle;
            }
        }
        return compressed_values[static_cast<std::size_t>(left)];
    }

    int count_prefix(Versions versions, int rank_right) const{
        if(rank_right <= 0){
            return 0;
        }
        const int value_count = static_cast<int>(compressed_values.size());
        if(value_count <= rank_right){
            return combined_count(versions);
        }

        int answer = 0;
        int left = 0;
        int right = value_count;
        while(right - left > 1){
            const int middle = left + (right - left) / 2;
            if(rank_right <= middle){
                versions = children(versions, false);
                right = middle;
            }else{
                const Versions left_versions = children(versions, false);
                answer += combined_count(left_versions);
                versions = children(versions, true);
                left = middle;
            }
        }
        if(left < rank_right){
            answer += combined_count(versions);
        }
        return answer;
    }

public:
    explicit TreePathOrderStatistics(int n = MAX_SIZE): _n(n), state(nullptr){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        state = std::make_unique<State>();
        state->head.fill(-1);
        state->parent.fill(-1);
    }

    TreePathOrderStatistics(const TreePathOrderStatistics&) = delete;
    TreePathOrderStatistics& operator=(const TreePathOrderStatistics&) = delete;
    TreePathOrderStatistics(TreePathOrderStatistics&&) = default;
    TreePathOrderStatistics& operator=(TreePathOrderStatistics&&) = default;

    int size() const{
        return _n;
    }

    void add_edge(int left, int right){
        if(built)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: edge added after build (add_edge)."
            );
        }
        check_vertex(left, "library assertion fault: range violation (add_edge).");
        check_vertex(right, "library assertion fault: range violation (add_edge).");
        if(static_cast<std::size_t>(edge_count + 2) > EDGE_CAPACITY)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: edge capacity exceeded (add_edge)."
            );
        }
        state->to[static_cast<std::size_t>(edge_count)] = right;
        state->next[static_cast<std::size_t>(edge_count)] =
            state->head[static_cast<std::size_t>(left)];
        state->head[static_cast<std::size_t>(left)] = edge_count++;
        state->to[static_cast<std::size_t>(edge_count)] = left;
        state->next[static_cast<std::size_t>(edge_count)] =
            state->head[static_cast<std::size_t>(right)];
        state->head[static_cast<std::size_t>(right)] = edge_count++;
    }

    void build(const std::vector<Value>& values, int root = 0){
        built = false;
        if(values.size() != static_cast<std::size_t>(_n))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: size mismatch (build)."
            );
        }
        if(_n == 0){
            if(edge_count != 0)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: graph is not a tree (build)."
                );
            }
            compressed_values.clear();
            node_count = 1;
            state->nodes[0] = Node{};
            root_vertex = -1;
            built = true;
            return;
        }
        check_vertex(root, "library assertion fault: range violation (build).");
        if(edge_count != 2 * (_n - 1))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: graph is not a tree (build)."
            );
        }

        state->parent.fill(-1);
        state->depth.fill(0);
        std::vector<int> order;
        order.reserve(static_cast<std::size_t>(_n));
        std::vector<int> stack = {root};
        state->parent[static_cast<std::size_t>(root)] = -2;
        while(!stack.empty()){
            const int vertex = stack.back();
            stack.pop_back();
            order.push_back(vertex);
            for(int edge = state->head[static_cast<std::size_t>(vertex)];
                edge != -1;
                edge = state->next[static_cast<std::size_t>(edge)]){
                const int to = state->to[static_cast<std::size_t>(edge)];
                if(to == state->parent[static_cast<std::size_t>(vertex)]){
                    continue;
                }
                if(state->parent[static_cast<std::size_t>(to)] != -1)[[unlikely]]{
                    throw std::runtime_error(
                        "library assertion fault: graph is not a tree (build)."
                    );
                }
                state->parent[static_cast<std::size_t>(to)] = vertex;
                state->depth[static_cast<std::size_t>(to)] =
                    state->depth[static_cast<std::size_t>(vertex)] + 1;
                stack.push_back(to);
            }
        }
        if(static_cast<int>(order.size()) != _n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: graph is not connected (build)."
            );
        }
        state->parent[static_cast<std::size_t>(root)] = -1;
        root_vertex = root;

        for(int vertex = 0; vertex < _n; ++vertex){
            const int parent = state->parent[static_cast<std::size_t>(vertex)];
            state->up[0][static_cast<std::size_t>(vertex)] =
                parent == -1 ? vertex : parent;
        }
        for(int level = 1; level < MAX_LOG; ++level){
            for(int vertex = 0; vertex < _n; ++vertex){
                const int middle = state->up[static_cast<std::size_t>(level - 1)]
                    [static_cast<std::size_t>(vertex)];
                state->up[static_cast<std::size_t>(level)]
                    [static_cast<std::size_t>(vertex)] =
                    state->up[static_cast<std::size_t>(level - 1)]
                        [static_cast<std::size_t>(middle)];
            }
        }

        compressed_values = values;
        std::sort(compressed_values.begin(), compressed_values.end());
        compressed_values.erase(
            std::unique(
                compressed_values.begin(),
                compressed_values.end(),
                [](const Value& left, const Value& right){
                    return !(left < right) && !(right < left);
                }
            ),
            compressed_values.end()
        );
        for(int vertex = 0; vertex < _n; ++vertex){
            state->rank[static_cast<std::size_t>(vertex)] = static_cast<int>(
                std::lower_bound(
                    compressed_values.begin(),
                    compressed_values.end(),
                    values[static_cast<std::size_t>(vertex)]
                ) - compressed_values.begin()
            );
        }

        node_count = 1;
        state->nodes[0] = Node{};
        const int value_count = static_cast<int>(compressed_values.size());
        for(const int vertex: order){
            const int parent = state->parent[static_cast<std::size_t>(vertex)];
            const int parent_version = parent == -1
                ? 0
                : state->version_root[static_cast<std::size_t>(parent)];
            state->version_root[static_cast<std::size_t>(vertex)] = add_version(
                parent_version,
                0,
                value_count,
                state->rank[static_cast<std::size_t>(vertex)]
            );
        }
        built = true;
    }

    int root() const{
        require_built("library assertion fault: build is required (root).");
        return root_vertex;
    }

    int parent(int vertex) const{
        require_built("library assertion fault: build is required (parent).");
        check_vertex(vertex, "library assertion fault: range violation (parent).");
        return state->parent[static_cast<std::size_t>(vertex)];
    }

    int depth(int vertex) const{
        require_built("library assertion fault: build is required (depth).");
        check_vertex(vertex, "library assertion fault: range violation (depth).");
        return state->depth[static_cast<std::size_t>(vertex)];
    }

    Value value(int vertex) const{
        require_built("library assertion fault: build is required (value).");
        check_vertex(vertex, "library assertion fault: range violation (value).");
        return compressed_values[static_cast<std::size_t>(
            state->rank[static_cast<std::size_t>(vertex)]
        )];
    }

    int lca(int left, int right) const{
        require_built("library assertion fault: build is required (lca).");
        check_vertex(left, "library assertion fault: range violation (lca).");
        check_vertex(right, "library assertion fault: range violation (lca).");
        return lca_unchecked(left, right);
    }

    int path_size(int left, int right) const{
        require_built("library assertion fault: build is required (path_size).");
        check_vertex(left, "library assertion fault: range violation (path_size).");
        check_vertex(right, "library assertion fault: range violation (path_size).");
        const int ancestor = lca_unchecked(left, right);
        return state->depth[static_cast<std::size_t>(left)]
            + state->depth[static_cast<std::size_t>(right)]
            - 2 * state->depth[static_cast<std::size_t>(ancestor)] + 1;
    }

    Value kth_smallest(int left, int right, int order) const{
        require_built("library assertion fault: build is required (kth_smallest).");
        check_vertex(left, "library assertion fault: range violation (kth_smallest).");
        check_vertex(right, "library assertion fault: range violation (kth_smallest).");
        const Versions versions = path_versions_unchecked(left, right);
        const int count = combined_count(versions);
        if(order < 0 || count <= order)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (kth_smallest)."
            );
        }
        return kth_smallest_unchecked(versions, order);
    }

    Value kth_largest(int left, int right, int order) const{
        require_built("library assertion fault: build is required (kth_largest).");
        check_vertex(left, "library assertion fault: range violation (kth_largest).");
        check_vertex(right, "library assertion fault: range violation (kth_largest).");
        const Versions versions = path_versions_unchecked(left, right);
        const int count = combined_count(versions);
        if(order < 0 || count <= order)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (kth_largest)."
            );
        }
        return kth_smallest_unchecked(versions, count - 1 - order);
    }

    int count_less(int left, int right, const Value& upper) const{
        require_built("library assertion fault: build is required (count_less).");
        check_vertex(left, "library assertion fault: range violation (count_less).");
        check_vertex(right, "library assertion fault: range violation (count_less).");
        const int rank_right = static_cast<int>(
            std::lower_bound(compressed_values.begin(), compressed_values.end(), upper)
                - compressed_values.begin()
        );
        return count_prefix(path_versions_unchecked(left, right), rank_right);
    }

    int count_equal(int left, int right, const Value& target) const{
        require_built("library assertion fault: build is required (count_equal).");
        check_vertex(left, "library assertion fault: range violation (count_equal).");
        check_vertex(right, "library assertion fault: range violation (count_equal).");
        const auto iterator = std::lower_bound(
            compressed_values.begin(), compressed_values.end(), target
        );
        if(iterator == compressed_values.end() || target < *iterator || *iterator < target){
            return 0;
        }
        const int rank = static_cast<int>(iterator - compressed_values.begin());
        const Versions versions = path_versions_unchecked(left, right);
        return count_prefix(versions, rank + 1) - count_prefix(versions, rank);
    }

    int range_freq(
        int left,
        int right,
        const Value& lower,
        const Value& upper
    ) const{
        require_built("library assertion fault: build is required (range_freq).");
        check_vertex(left, "library assertion fault: range violation (range_freq).");
        check_vertex(right, "library assertion fault: range violation (range_freq).");
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid value range (range_freq)."
            );
        }
        const int rank_left = static_cast<int>(
            std::lower_bound(compressed_values.begin(), compressed_values.end(), lower)
                - compressed_values.begin()
        );
        const int rank_right = static_cast<int>(
            std::lower_bound(compressed_values.begin(), compressed_values.end(), upper)
                - compressed_values.begin()
        );
        const Versions versions = path_versions_unchecked(left, right);
        return count_prefix(versions, rank_right) - count_prefix(versions, rank_left);
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_TREE_PATH_ORDER_STATISTICS_HPP_INCLUDED
