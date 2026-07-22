#ifndef CPPLIB_SRC_ALGORITHM_TREE_DECOMPOSITION_EULER_TOUR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_DECOMPOSITION_EULER_TOUR_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <stdexcept>
#include <utility>
#include <vector>

template<int MAX_SIZE>
class TreeEulerTour{
    static_assert(MAX_SIZE >= 0);

    static constexpr int EDGE_CAPACITY =
        MAX_SIZE == 0 ? 0 : MAX_SIZE - 1;

public:
    struct Event{
        int vertex;
        bool entering;
    };

private:
    int _n;
    int _edge_count = 0;
    int _root = -1;
    int _timer = 0;
    int _event_count = 0;
    bool built = false;

    std::array<int, EDGE_CAPACITY> edge_u;
    std::array<int, EDGE_CAPACITY> edge_v;
    std::array<int, MAX_SIZE + 1> offset;
    std::array<int, 2 * EDGE_CAPACITY> adjacency;

    std::array<int, MAX_SIZE> parent_vertex;
    std::array<int, MAX_SIZE> depth_vertex;
    std::array<int, MAX_SIZE> tin_vertex;
    std::array<int, MAX_SIZE> tout_vertex;
    std::array<int, MAX_SIZE> preorder_vertices;
    std::array<Event, 2 * MAX_SIZE> events;

    std::array<int, MAX_SIZE> stack_vertex;
    std::array<int, MAX_SIZE> stack_next;

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

    void check_preorder_index(int index) const{
        if(index < 0 || _n <= index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (preorder_vertex)."
            );
        }
    }

public:
    explicit TreeEulerTour(int n = MAX_SIZE) : _n(n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
    }

    int size() const{
        return _n;
    }

    int edge_count() const{
        return _edge_count;
    }

    void add_edge(int u, int v){
        check_vertex(
            u,
            "library assertion fault: range violation (add_edge)."
        );
        check_vertex(
            v,
            "library assertion fault: range violation (add_edge)."
        );
        if(_edge_count == EDGE_CAPACITY)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity exceeded (add_edge)."
            );
        }
        edge_u[static_cast<std::size_t>(_edge_count)] = u;
        edge_v[static_cast<std::size_t>(_edge_count)] = v;
        ++_edge_count;
        built = false;
    }

    void build(int root = 0){
        built = false;
        _timer = 0;
        _event_count = 0;

        if(_n == 0){
            if(_edge_count != 0)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: graph is not a tree (build)."
                );
            }
            _root = -1;
            built = true;
            return;
        }

        check_vertex(
            root,
            "library assertion fault: range violation (build)."
        );
        if(_edge_count != _n - 1)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: graph is not a tree (build)."
            );
        }

        std::fill_n(offset.begin(), _n + 1, 0);
        for(int edge = 0; edge < _edge_count; ++edge){
            ++offset[static_cast<std::size_t>(
                edge_u[static_cast<std::size_t>(edge)] + 1
            )];
            ++offset[static_cast<std::size_t>(
                edge_v[static_cast<std::size_t>(edge)] + 1
            )];
        }
        for(int vertex = 0; vertex < _n; ++vertex){
            offset[static_cast<std::size_t>(vertex + 1)] +=
                offset[static_cast<std::size_t>(vertex)];
        }

        std::copy_n(offset.begin(), _n, stack_next.begin());
        for(int edge = 0; edge < _edge_count; ++edge){
            const int u = edge_u[static_cast<std::size_t>(edge)];
            const int v = edge_v[static_cast<std::size_t>(edge)];
            adjacency[static_cast<std::size_t>(
                stack_next[static_cast<std::size_t>(u)]++
            )] = v;
            adjacency[static_cast<std::size_t>(
                stack_next[static_cast<std::size_t>(v)]++
            )] = u;
        }

        std::fill_n(parent_vertex.begin(), _n, -2);
        std::fill_n(depth_vertex.begin(), _n, 0);
        std::fill_n(tin_vertex.begin(), _n, -1);
        std::fill_n(tout_vertex.begin(), _n, -1);

        _root = root;
        parent_vertex[static_cast<std::size_t>(root)] = -1;
        depth_vertex[static_cast<std::size_t>(root)] = 0;
        tin_vertex[static_cast<std::size_t>(root)] = _timer;
        preorder_vertices[static_cast<std::size_t>(_timer++)] = root;
        events[static_cast<std::size_t>(_event_count++)] = {root, true};

        int stack_size = 1;
        stack_vertex[0] = root;
        stack_next[0] = offset[static_cast<std::size_t>(root)];

        while(stack_size > 0){
            const int top = stack_size - 1;
            const int vertex = stack_vertex[static_cast<std::size_t>(top)];
            int& next = stack_next[static_cast<std::size_t>(top)];

            if(next == offset[static_cast<std::size_t>(vertex + 1)]){
                tout_vertex[static_cast<std::size_t>(vertex)] = _timer;
                events[static_cast<std::size_t>(_event_count++)] = {
                    vertex,
                    false
                };
                --stack_size;
                continue;
            }

            const int to = adjacency[static_cast<std::size_t>(next++)];
            if(to == parent_vertex[static_cast<std::size_t>(vertex)]){
                continue;
            }
            if(parent_vertex[static_cast<std::size_t>(to)] != -2)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: graph is not a tree (build)."
                );
            }

            parent_vertex[static_cast<std::size_t>(to)] = vertex;
            depth_vertex[static_cast<std::size_t>(to)] =
                depth_vertex[static_cast<std::size_t>(vertex)] + 1;
            tin_vertex[static_cast<std::size_t>(to)] = _timer;
            preorder_vertices[static_cast<std::size_t>(_timer++)] = to;
            events[static_cast<std::size_t>(_event_count++)] = {to, true};

            stack_vertex[static_cast<std::size_t>(stack_size)] = to;
            stack_next[static_cast<std::size_t>(stack_size)] =
                offset[static_cast<std::size_t>(to)];
            ++stack_size;
        }

        if(_timer != _n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: graph is not connected (build)."
            );
        }
        built = true;
    }

    int root() const{
        require_built(
            "library assertion fault: TreeEulerTour is not built (root)."
        );
        return _root;
    }

    int parent(int vertex) const{
        require_built(
            "library assertion fault: TreeEulerTour is not built (parent)."
        );
        check_vertex(
            vertex,
            "library assertion fault: range violation (parent)."
        );
        return parent_vertex[static_cast<std::size_t>(vertex)];
    }

    int depth(int vertex) const{
        require_built(
            "library assertion fault: TreeEulerTour is not built (depth)."
        );
        check_vertex(
            vertex,
            "library assertion fault: range violation (depth)."
        );
        return depth_vertex[static_cast<std::size_t>(vertex)];
    }

    int tin(int vertex) const{
        require_built(
            "library assertion fault: TreeEulerTour is not built (tin)."
        );
        check_vertex(
            vertex,
            "library assertion fault: range violation (tin)."
        );
        return tin_vertex[static_cast<std::size_t>(vertex)];
    }

    int tout(int vertex) const{
        require_built(
            "library assertion fault: TreeEulerTour is not built (tout)."
        );
        check_vertex(
            vertex,
            "library assertion fault: range violation (tout)."
        );
        return tout_vertex[static_cast<std::size_t>(vertex)];
    }

    std::pair<int, int> subtree_range(int vertex) const{
        return {tin(vertex), tout(vertex)};
    }

    int subtree_size(int vertex) const{
        return tout(vertex) - tin(vertex);
    }

    bool is_ancestor(int ancestor, int descendant) const{
        const int left = tin(ancestor);
        const int point = tin(descendant);
        return left <= point && point < tout(ancestor);
    }

    int preorder_vertex(int index) const{
        require_built(
            "library assertion fault: TreeEulerTour is not built (preorder_vertex)."
        );
        check_preorder_index(index);
        return preorder_vertices[static_cast<std::size_t>(index)];
    }

    std::vector<int> preorder() const{
        require_built(
            "library assertion fault: TreeEulerTour is not built (preorder)."
        );
        if constexpr(MAX_SIZE == 0){
            return {};
        }else{
            return std::vector<int>(
                preorder_vertices.begin(),
                preorder_vertices.begin() + _n
            );
        }
    }

    int euler_size() const{
        require_built(
            "library assertion fault: TreeEulerTour is not built (euler_size)."
        );
        return _event_count;
    }

    Event euler_event(int index) const{
        require_built(
            "library assertion fault: TreeEulerTour is not built (euler_event)."
        );
        if(index < 0 || _event_count <= index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (euler_event)."
            );
        }
        return events[static_cast<std::size_t>(index)];
    }

    std::vector<Event> euler_tour() const{
        require_built(
            "library assertion fault: TreeEulerTour is not built (euler_tour)."
        );
        if constexpr(MAX_SIZE == 0){
            return {};
        }else{
            return std::vector<Event>(
                events.begin(),
                events.begin() + _event_count
            );
        }
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_DECOMPOSITION_EULER_TOUR_HPP_INCLUDED
