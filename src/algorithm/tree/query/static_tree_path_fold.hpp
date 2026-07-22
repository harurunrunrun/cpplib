#ifndef CPPLIB_SRC_ALGORITHM_TREE_QUERY_STATIC_TREE_PATH_FOLD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_QUERY_STATIC_TREE_PATH_FOLD_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

// Static weighted-tree path products by binary lifting.
// Both directions of every lifted segment are stored, so Monoid may be
// non-commutative.
template<auto Monoid, int MAX_SIZE>
struct StaticTreePathFold{
    static_assert(MAX_SIZE > 0);
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

private:
    struct Edge{
        int to;
        int id;
        S value;
    };

    int _n;
    int _log;
    int edge_count = 0;
    int root_vertex = -1;
    bool built = false;
    bool edges_sealed = false;
    std::vector<std::vector<Edge>> graph;
    std::vector<int> parent_vertex;
    std::vector<int> depth_vertex;
    std::vector<int> up;
    std::vector<S> upward_product;
    std::vector<S> downward_product;

    static int logarithm(int n){
        int result = 1;
        while((1LL << result) <= std::max(1, n)) ++result;
        return result;
    }

    static std::size_t checked_size(int n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        return static_cast<std::size_t>(n);
    }

    static std::size_t checked_table_size(int n, int logarithm_value){
        const std::size_t width = static_cast<std::size_t>(std::max(1, n));
        const std::size_t levels = static_cast<std::size_t>(logarithm_value);
        if(std::numeric_limits<std::size_t>::max() / levels < width)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: table size overflow (constructor)."
            );
        }
        return width * levels;
    }

    void check_vertex(int vertex, const char* message) const{
        if(vertex < 0 || _n <= vertex)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void require_built(const char* message) const{
        if(!built)[[unlikely]] throw std::runtime_error(message);
    }

    std::size_t index(int level, int vertex) const{
        return static_cast<std::size_t>(level) * static_cast<std::size_t>(_n)
            + static_cast<std::size_t>(vertex);
    }

    int ancestor(int level, int vertex) const{
        return up[index(level, vertex)];
    }

    void climb(int& vertex, int distance, S* forward, S* reverse) const{
        for(int level = _log - 1; level >= 0; --level){
            if(((distance >> level) & 1) == 0) continue;
            const std::size_t position = index(level, vertex);
            if(forward != nullptr){
                *forward = Monoid.op(*forward, upward_product[position]);
            }
            if(reverse != nullptr){
                *reverse = Monoid.op(downward_product[position], *reverse);
            }
            vertex = up[position];
        }
    }

public:
    explicit StaticTreePathFold(int n = MAX_SIZE):
        _n(n),
        _log(logarithm(n)),
        graph(checked_size(n)),
        parent_vertex(graph.size(), -1),
        depth_vertex(graph.size(), 0){
        const std::size_t table_size = checked_table_size(n, _log);
        up.assign(table_size, 0);
        upward_product.assign(table_size, Monoid.e());
        downward_product.assign(table_size, Monoid.e());
    }

    int size() const{
        return _n;
    }

    void add_edge(int left, int right, const S& value){
        if(edges_sealed)[[unlikely]]{
            throw std::runtime_error("library assertion fault: already built (add_edge).");
        }
        check_vertex(left, "library assertion fault: range violation (add_edge).");
        check_vertex(right, "library assertion fault: range violation (add_edge).");
        if(edge_count == std::numeric_limits<int>::max())[[unlikely]]{
            throw std::runtime_error("library assertion fault: too many edges (add_edge).");
        }
        const int id = edge_count++;
        graph[static_cast<std::size_t>(left)].push_back({right, id, value});
        graph[static_cast<std::size_t>(right)].push_back({left, id, value});
    }

    void build(int root = 0){
        built = false;
        root_vertex = -1;
        if(_n == 0){
            if(edge_count != 0)[[unlikely]]{
                throw std::runtime_error("library assertion fault: graph is not a tree (build).");
            }
            root_vertex = -1;
            built = true;
            edges_sealed = true;
            return;
        }
        check_vertex(root, "library assertion fault: range violation (build).");
        if(edge_count != _n - 1)[[unlikely]]{
            throw std::runtime_error("library assertion fault: graph is not a tree (build).");
        }

        std::fill(parent_vertex.begin(), parent_vertex.end(), -1);
        std::fill(depth_vertex.begin(), depth_vertex.end(), 0);
        std::fill(up.begin(), up.end(), 0);
        std::fill(upward_product.begin(), upward_product.end(), Monoid.e());
        std::fill(downward_product.begin(), downward_product.end(), Monoid.e());

        std::vector<int> parent_edge(static_cast<std::size_t>(_n), -1);
        std::vector<int> order;
        order.reserve(static_cast<std::size_t>(_n));
        std::vector<int> stack = {root};
        parent_vertex[static_cast<std::size_t>(root)] = root;
        while(!stack.empty()){
            const int vertex = stack.back();
            stack.pop_back();
            order.push_back(vertex);
            for(const Edge& edge: graph[static_cast<std::size_t>(vertex)]){
                if(edge.id == parent_edge[static_cast<std::size_t>(vertex)]) continue;
                if(parent_vertex[static_cast<std::size_t>(edge.to)] != -1)[[unlikely]]{
                    throw std::runtime_error("library assertion fault: graph is not a tree (build).");
                }
                parent_vertex[static_cast<std::size_t>(edge.to)] = vertex;
                parent_edge[static_cast<std::size_t>(edge.to)] = edge.id;
                depth_vertex[static_cast<std::size_t>(edge.to)]
                    = depth_vertex[static_cast<std::size_t>(vertex)] + 1;
                const std::size_t position = index(0, edge.to);
                up[position] = vertex;
                upward_product[position] = edge.value;
                downward_product[position] = edge.value;
                stack.push_back(edge.to);
            }
        }
        if(static_cast<int>(order.size()) != _n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: graph is not connected (build).");
        }

        up[index(0, root)] = root;
        parent_vertex[static_cast<std::size_t>(root)] = -1;
        for(int level = 1; level < _log; ++level){
            for(int vertex = 0; vertex < _n; ++vertex){
                const std::size_t lower = index(level - 1, vertex);
                const int middle = up[lower];
                const std::size_t upper = index(level - 1, middle);
                const std::size_t current = index(level, vertex);
                up[current] = up[upper];
                upward_product[current] = Monoid.op(
                    upward_product[lower],
                    upward_product[upper]
                );
                downward_product[current] = Monoid.op(
                    downward_product[upper],
                    downward_product[lower]
                );
            }
        }
        root_vertex = root;
        built = true;
        edges_sealed = true;
    }

    int root() const{
        require_built("library assertion fault: not built (root).");
        return root_vertex;
    }

    int parent(int vertex) const{
        require_built("library assertion fault: not built (parent).");
        check_vertex(vertex, "library assertion fault: range violation (parent).");
        return parent_vertex[static_cast<std::size_t>(vertex)];
    }

    int depth(int vertex) const{
        require_built("library assertion fault: not built (depth).");
        check_vertex(vertex, "library assertion fault: range violation (depth).");
        return depth_vertex[static_cast<std::size_t>(vertex)];
    }

    int jump(int vertex, int distance) const{
        require_built("library assertion fault: not built (jump).");
        check_vertex(vertex, "library assertion fault: range violation (jump).");
        if(distance < 0 || depth_vertex[static_cast<std::size_t>(vertex)] < distance)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (jump).");
        }
        climb(vertex, distance, nullptr, nullptr);
        return vertex;
    }

    int lca(int left, int right) const{
        require_built("library assertion fault: not built (lca).");
        check_vertex(left, "library assertion fault: range violation (lca).");
        check_vertex(right, "library assertion fault: range violation (lca).");
        if(depth_vertex[static_cast<std::size_t>(left)]
            < depth_vertex[static_cast<std::size_t>(right)]){
            std::swap(left, right);
        }
        climb(
            left,
            depth_vertex[static_cast<std::size_t>(left)]
                - depth_vertex[static_cast<std::size_t>(right)],
            nullptr,
            nullptr
        );
        if(left == right) return left;
        for(int level = _log - 1; level >= 0; --level){
            if(ancestor(level, left) == ancestor(level, right)) continue;
            left = ancestor(level, left);
            right = ancestor(level, right);
        }
        return parent_vertex[static_cast<std::size_t>(left)];
    }

    int dist(int left, int right) const{
        const int ancestor_vertex = lca(left, right);
        return depth_vertex[static_cast<std::size_t>(left)]
            + depth_vertex[static_cast<std::size_t>(right)]
            - 2 * depth_vertex[static_cast<std::size_t>(ancestor_vertex)];
    }

    int kth_vertex_on_path(int left, int right, int index_on_path) const{
        const int ancestor_vertex = lca(left, right);
        const int upward_edges = depth_vertex[static_cast<std::size_t>(left)]
            - depth_vertex[static_cast<std::size_t>(ancestor_vertex)];
        const int downward_edges = depth_vertex[static_cast<std::size_t>(right)]
            - depth_vertex[static_cast<std::size_t>(ancestor_vertex)];
        const int path_edges = upward_edges + downward_edges;
        if(index_on_path < 0 || path_edges < index_on_path)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(kth_vertex_on_path)."
            );
        }
        if(index_on_path <= upward_edges){
            return jump(left, index_on_path);
        }
        return jump(right, path_edges - index_on_path);
    }

    S prod(int left, int right) const{
        require_built("library assertion fault: not built (prod).");
        check_vertex(left, "library assertion fault: range violation (prod).");
        check_vertex(right, "library assertion fault: range violation (prod).");

        S left_product = Monoid.e();
        S right_product = Monoid.e();
        if(depth_vertex[static_cast<std::size_t>(left)]
            > depth_vertex[static_cast<std::size_t>(right)]){
            climb(
                left,
                depth_vertex[static_cast<std::size_t>(left)]
                    - depth_vertex[static_cast<std::size_t>(right)],
                &left_product,
                nullptr
            );
        }else if(depth_vertex[static_cast<std::size_t>(right)]
            > depth_vertex[static_cast<std::size_t>(left)]){
            climb(
                right,
                depth_vertex[static_cast<std::size_t>(right)]
                    - depth_vertex[static_cast<std::size_t>(left)],
                nullptr,
                &right_product
            );
        }
        if(left == right) return Monoid.op(left_product, right_product);

        for(int level = _log - 1; level >= 0; --level){
            if(ancestor(level, left) == ancestor(level, right)) continue;
            const std::size_t left_position = index(level, left);
            const std::size_t right_position = index(level, right);
            left_product = Monoid.op(
                left_product,
                upward_product[left_position]
            );
            right_product = Monoid.op(
                downward_product[right_position],
                right_product
            );
            left = up[left_position];
            right = up[right_position];
        }
        left_product = Monoid.op(
            left_product,
            upward_product[index(0, left)]
        );
        right_product = Monoid.op(
            downward_product[index(0, right)],
            right_product
        );
        return Monoid.op(left_product, right_product);
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_QUERY_STATIC_TREE_PATH_FOLD_HPP_INCLUDED
