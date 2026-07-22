#ifndef CPPLIB_SRC_ALGORITHM_TREE_DECOMPOSITION_DSU_ON_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_DECOMPOSITION_DSU_ON_TREE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <functional>
#include <stdexcept>
#include <utility>

template<int MAX_SIZE>
class DsuOnTree{
    static_assert(MAX_SIZE >= 0);

    static constexpr int EDGE_CAPACITY =
        MAX_SIZE == 0 ? 0 : MAX_SIZE - 1;

    int _n;
    int _edge_count = 0;
    int _root = -1;
    bool built = false;

    std::array<int, EDGE_CAPACITY> edge_u;
    std::array<int, EDGE_CAPACITY> edge_v;
    std::array<int, MAX_SIZE + 1> offset;
    std::array<int, 2 * EDGE_CAPACITY> adjacency;

    std::array<int, MAX_SIZE> parent_vertex;
    std::array<int, MAX_SIZE> subtree_size_vertex;
    std::array<int, MAX_SIZE> heavy_child_vertex;
    std::array<int, MAX_SIZE> tin_vertex;
    std::array<int, MAX_SIZE> tout_vertex;
    std::array<int, MAX_SIZE> preorder_vertices;

    // build and callback traversal workspaces. Keeping these in the object avoids
    // using the process call stack even for a path-shaped tree.
    std::array<int, MAX_SIZE> stack_vertex;
    std::array<int, MAX_SIZE> stack_next;
    std::array<unsigned char, MAX_SIZE> stack_phase;
    std::array<unsigned char, MAX_SIZE> stack_keep;

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

    template<class Callback>
    static void call_vertex(Callback& callback, int vertex){
        std::invoke(callback, vertex);
    }

    template<class Callback>
    static void call_reset(Callback& callback){
        std::invoke(callback);
    }

public:
    explicit DsuOnTree(int n = MAX_SIZE) : _n(n){
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
        std::fill_n(subtree_size_vertex.begin(), _n, 1);
        std::fill_n(heavy_child_vertex.begin(), _n, -1);

        _root = root;
        parent_vertex[static_cast<std::size_t>(root)] = -1;
        int order_size = 0;
        int stack_size = 1;
        stack_vertex[0] = root;
        while(stack_size > 0){
            const int vertex = stack_vertex[static_cast<std::size_t>(--stack_size)];
            tin_vertex[static_cast<std::size_t>(vertex)] = order_size;
            preorder_vertices[static_cast<std::size_t>(order_size++)] = vertex;

            for(int index = offset[static_cast<std::size_t>(vertex)];
                index < offset[static_cast<std::size_t>(vertex + 1)];
                ++index){
                const int to = adjacency[static_cast<std::size_t>(index)];
                if(to == parent_vertex[static_cast<std::size_t>(vertex)]){
                    continue;
                }
                if(parent_vertex[static_cast<std::size_t>(to)] != -2)[[unlikely]]{
                    throw std::runtime_error(
                        "library assertion fault: graph is not a tree (build)."
                    );
                }
                parent_vertex[static_cast<std::size_t>(to)] = vertex;
                stack_vertex[static_cast<std::size_t>(stack_size++)] = to;
            }
        }
        if(order_size != _n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: graph is not connected (build)."
            );
        }

        for(int order = _n - 1; order >= 0; --order){
            const int vertex = preorder_vertices[static_cast<std::size_t>(order)];
            int largest_size = 0;
            for(int index = offset[static_cast<std::size_t>(vertex)];
                index < offset[static_cast<std::size_t>(vertex + 1)];
                ++index){
                const int to = adjacency[static_cast<std::size_t>(index)];
                if(parent_vertex[static_cast<std::size_t>(to)] != vertex){
                    continue;
                }
                subtree_size_vertex[static_cast<std::size_t>(vertex)] +=
                    subtree_size_vertex[static_cast<std::size_t>(to)];
                if(largest_size <
                    subtree_size_vertex[static_cast<std::size_t>(to)]){
                    largest_size =
                        subtree_size_vertex[static_cast<std::size_t>(to)];
                    heavy_child_vertex[static_cast<std::size_t>(vertex)] = to;
                }
            }
            tout_vertex[static_cast<std::size_t>(vertex)] =
                tin_vertex[static_cast<std::size_t>(vertex)] +
                subtree_size_vertex[static_cast<std::size_t>(vertex)];
        }
        built = true;
    }

    int root() const{
        require_built(
            "library assertion fault: DsuOnTree is not built (root)."
        );
        return _root;
    }

    int parent(int vertex) const{
        require_built(
            "library assertion fault: DsuOnTree is not built (parent)."
        );
        check_vertex(
            vertex,
            "library assertion fault: range violation (parent)."
        );
        return parent_vertex[static_cast<std::size_t>(vertex)];
    }

    int subtree_size(int vertex) const{
        require_built(
            "library assertion fault: DsuOnTree is not built (subtree_size)."
        );
        check_vertex(
            vertex,
            "library assertion fault: range violation (subtree_size)."
        );
        return subtree_size_vertex[static_cast<std::size_t>(vertex)];
    }

    int heavy_child(int vertex) const{
        require_built(
            "library assertion fault: DsuOnTree is not built (heavy_child)."
        );
        check_vertex(
            vertex,
            "library assertion fault: range violation (heavy_child)."
        );
        return heavy_child_vertex[static_cast<std::size_t>(vertex)];
    }

    int tin(int vertex) const{
        require_built(
            "library assertion fault: DsuOnTree is not built (tin)."
        );
        check_vertex(
            vertex,
            "library assertion fault: range violation (tin)."
        );
        return tin_vertex[static_cast<std::size_t>(vertex)];
    }

    int tout(int vertex) const{
        require_built(
            "library assertion fault: DsuOnTree is not built (tout)."
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

    int preorder_vertex(int index) const{
        require_built(
            "library assertion fault: DsuOnTree is not built (preorder_vertex)."
        );
        if(index < 0 || _n <= index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (preorder_vertex)."
            );
        }
        return preorder_vertices[static_cast<std::size_t>(index)];
    }

    template<class Add, class Remove, class Answer>
    void run(Add&& add, Remove&& remove, Answer&& answer){
        require_built(
            "library assertion fault: DsuOnTree is not built (run)."
        );
        if(_n == 0){
            return;
        }

        auto&& add_callback = add;
        auto&& remove_callback = remove;
        auto&& answer_callback = answer;

        int stack_size = 1;
        stack_vertex[0] = _root;
        stack_next[0] = offset[static_cast<std::size_t>(_root)];
        stack_phase[0] = 0;
        stack_keep[0] = 0;

        while(stack_size > 0){
            const int top = stack_size - 1;
            const int vertex = stack_vertex[static_cast<std::size_t>(top)];
            unsigned char& phase = stack_phase[static_cast<std::size_t>(top)];

            if(phase == 0){
                int& next = stack_next[static_cast<std::size_t>(top)];
                const int end = offset[static_cast<std::size_t>(vertex + 1)];
                bool descended = false;
                while(next < end){
                    const int to = adjacency[static_cast<std::size_t>(next++)];
                    if(parent_vertex[static_cast<std::size_t>(to)] == vertex &&
                        to != heavy_child_vertex[static_cast<std::size_t>(vertex)]){
                        stack_vertex[static_cast<std::size_t>(stack_size)] = to;
                        stack_next[static_cast<std::size_t>(stack_size)] =
                            offset[static_cast<std::size_t>(to)];
                        stack_phase[static_cast<std::size_t>(stack_size)] = 0;
                        stack_keep[static_cast<std::size_t>(stack_size)] = 0;
                        ++stack_size;
                        descended = true;
                        break;
                    }
                }
                if(!descended){
                    phase = 1;
                }
                continue;
            }

            if(phase == 1){
                phase = 2;
                const int heavy =
                    heavy_child_vertex[static_cast<std::size_t>(vertex)];
                if(heavy != -1){
                    stack_vertex[static_cast<std::size_t>(stack_size)] = heavy;
                    stack_next[static_cast<std::size_t>(stack_size)] =
                        offset[static_cast<std::size_t>(heavy)];
                    stack_phase[static_cast<std::size_t>(stack_size)] = 0;
                    stack_keep[static_cast<std::size_t>(stack_size)] = 1;
                    ++stack_size;
                }
                continue;
            }

            if(phase == 2){
                const int heavy =
                    heavy_child_vertex[static_cast<std::size_t>(vertex)];
                for(int index = offset[static_cast<std::size_t>(vertex)];
                    index < offset[static_cast<std::size_t>(vertex + 1)];
                    ++index){
                    const int child = adjacency[static_cast<std::size_t>(index)];
                    if(parent_vertex[static_cast<std::size_t>(child)] != vertex ||
                        child == heavy){
                        continue;
                    }
                    for(int order = tin_vertex[static_cast<std::size_t>(child)];
                        order < tout_vertex[static_cast<std::size_t>(child)];
                        ++order){
                        call_vertex(
                            add_callback,
                            preorder_vertices[static_cast<std::size_t>(order)]
                        );
                    }
                }
                call_vertex(add_callback, vertex);
                call_vertex(answer_callback, vertex);
                phase = 3;
                continue;
            }

            if(stack_keep[static_cast<std::size_t>(top)] == 0){
                for(int order = tin_vertex[static_cast<std::size_t>(vertex)];
                    order < tout_vertex[static_cast<std::size_t>(vertex)];
                    ++order){
                    call_vertex(
                        remove_callback,
                        preorder_vertices[static_cast<std::size_t>(order)]
                    );
                }
            }
            --stack_size;
        }
    }

    template<class Add, class Reset, class Answer>
    void run_with_reset(Add&& add, Reset&& reset, Answer&& answer){
        require_built(
            "library assertion fault: DsuOnTree is not built (run_with_reset)."
        );
        if(_n == 0){
            return;
        }

        auto&& add_callback = add;
        auto&& reset_callback = reset;
        auto&& answer_callback = answer;

        int stack_size = 1;
        stack_vertex[0] = _root;
        stack_next[0] = offset[static_cast<std::size_t>(_root)];
        stack_phase[0] = 0;

        while(stack_size > 0){
            const int top = stack_size - 1;
            const int vertex = stack_vertex[static_cast<std::size_t>(top)];
            unsigned char& phase = stack_phase[static_cast<std::size_t>(top)];

            if(phase == 0){
                int& next = stack_next[static_cast<std::size_t>(top)];
                const int end = offset[static_cast<std::size_t>(vertex + 1)];
                bool descended = false;
                while(next < end){
                    const int to = adjacency[static_cast<std::size_t>(next++)];
                    if(parent_vertex[static_cast<std::size_t>(to)] == vertex &&
                        to != heavy_child_vertex[static_cast<std::size_t>(vertex)]){
                        phase = 1;
                        stack_vertex[static_cast<std::size_t>(stack_size)] = to;
                        stack_next[static_cast<std::size_t>(stack_size)] =
                            offset[static_cast<std::size_t>(to)];
                        stack_phase[static_cast<std::size_t>(stack_size)] = 0;
                        ++stack_size;
                        descended = true;
                        break;
                    }
                }
                if(!descended){
                    phase = 2;
                }
                continue;
            }

            if(phase == 1){
                call_reset(reset_callback);
                phase = 0;
                continue;
            }

            if(phase == 2){
                phase = 3;
                const int heavy =
                    heavy_child_vertex[static_cast<std::size_t>(vertex)];
                if(heavy != -1){
                    stack_vertex[static_cast<std::size_t>(stack_size)] = heavy;
                    stack_next[static_cast<std::size_t>(stack_size)] =
                        offset[static_cast<std::size_t>(heavy)];
                    stack_phase[static_cast<std::size_t>(stack_size)] = 0;
                    ++stack_size;
                }
                continue;
            }

            if(phase == 3){
                const int heavy =
                    heavy_child_vertex[static_cast<std::size_t>(vertex)];
                for(int index = offset[static_cast<std::size_t>(vertex)];
                    index < offset[static_cast<std::size_t>(vertex + 1)];
                    ++index){
                    const int child = adjacency[static_cast<std::size_t>(index)];
                    if(parent_vertex[static_cast<std::size_t>(child)] != vertex ||
                        child == heavy){
                        continue;
                    }
                    for(int order = tin_vertex[static_cast<std::size_t>(child)];
                        order < tout_vertex[static_cast<std::size_t>(child)];
                        ++order){
                        call_vertex(
                            add_callback,
                            preorder_vertices[static_cast<std::size_t>(order)]
                        );
                    }
                }
                call_vertex(add_callback, vertex);
                call_vertex(answer_callback, vertex);
                phase = 4;
                continue;
            }

            --stack_size;
        }
        call_reset(reset_callback);
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_DECOMPOSITION_DSU_ON_TREE_HPP_INCLUDED
