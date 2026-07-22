#ifndef CPPLIB_SRC_ALGORITHM_TREE_AUXILIARY_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_AUXILIARY_TREE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <span>
#include <stdexcept>

template<int MAX_SIZE>
class AuxiliaryTree{
    static_assert(MAX_SIZE >= 0);

    static constexpr int EDGE_CAPACITY = MAX_SIZE == 0 ? 0 : MAX_SIZE - 1;

    static consteval int binary_lifting_log(){
        int result = 1;
        int size = 1;
        while(size < MAX_SIZE){
            size <<= 1;
            ++result;
        }
        return result;
    }

    static constexpr int LOG = binary_lifting_log();

    int _size;
    int _edge_count = 0;
    int _compressed_size = 0;
    bool prepared = false;

    std::array<int, EDGE_CAPACITY> edge_u;
    std::array<int, EDGE_CAPACITY> edge_v;
    std::array<int, MAX_SIZE + 1> offset;
    std::array<int, 2 * EDGE_CAPACITY> adjacency;

    std::array<int, MAX_SIZE> parent_vertex;
    std::array<int, MAX_SIZE> depth_vertex;
    std::array<int, MAX_SIZE> tin_vertex;
    std::array<int, MAX_SIZE> tout_vertex;
    std::array<int, LOG * MAX_SIZE> ancestors;

    std::array<int, MAX_SIZE> dfs_vertex;
    std::array<int, MAX_SIZE> dfs_next;
    std::array<int, MAX_SIZE> cursor;

    std::array<int, 2 * MAX_SIZE> compressed_vertices;
    std::array<int, 2 * MAX_SIZE> compressed_parents;
    std::array<int, 2 * MAX_SIZE> compressed_edge_lengths;
    std::array<bool, 2 * MAX_SIZE> compressed_selected;
    std::array<int, 2 * MAX_SIZE> compressed_stack;
    std::array<int, MAX_SIZE> selected_vertices;

    void check_vertex(int vertex, const char* message) const{
        if(vertex < 0 || _size <= vertex)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_compressed_index(int index, const char* message) const{
        if(index < 0 || _compressed_size <= index)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void require_prepared(const char* message) const{
        if(!prepared)[[unlikely]] throw std::runtime_error(message);
    }

    int ancestor(int level, int vertex) const{
        return ancestors[static_cast<std::size_t>(level * MAX_SIZE + vertex)];
    }

public:
    explicit AuxiliaryTree(int size = MAX_SIZE): _size(size){
        if(size < 0 || MAX_SIZE < size)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (AuxiliaryTree constructor)."
            );
        }
    }

    int size() const{
        return _size;
    }

    int edge_count() const{
        return _edge_count;
    }

    void add_edge(int u, int v){
        check_vertex(u, "library assertion fault: range violation (AuxiliaryTree::add_edge).");
        check_vertex(v, "library assertion fault: range violation (AuxiliaryTree::add_edge).");
        if(_edge_count == EDGE_CAPACITY)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity exceeded (AuxiliaryTree::add_edge)."
            );
        }
        edge_u[static_cast<std::size_t>(_edge_count)] = u;
        edge_v[static_cast<std::size_t>(_edge_count)] = v;
        ++_edge_count;
        prepared = false;
        _compressed_size = 0;
    }

    void build(int root = 0){
        prepared = false;
        _compressed_size = 0;
        if(_size == 0){
            if(_edge_count != 0)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: graph is not a tree (AuxiliaryTree::build)."
                );
            }
            prepared = true;
            return;
        }
        check_vertex(root, "library assertion fault: range violation (AuxiliaryTree::build).");
        if(_edge_count != _size - 1)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: graph is not a tree (AuxiliaryTree::build)."
            );
        }

        std::fill_n(offset.begin(), _size + 1, 0);
        for(int edge = 0; edge < _edge_count; ++edge){
            ++offset[static_cast<std::size_t>(edge_u[static_cast<std::size_t>(edge)] + 1)];
            ++offset[static_cast<std::size_t>(edge_v[static_cast<std::size_t>(edge)] + 1)];
        }
        for(int vertex = 0; vertex < _size; ++vertex){
            offset[static_cast<std::size_t>(vertex + 1)] +=
                offset[static_cast<std::size_t>(vertex)];
        }
        std::copy_n(offset.begin(), _size, cursor.begin());
        for(int edge = 0; edge < _edge_count; ++edge){
            const int u = edge_u[static_cast<std::size_t>(edge)];
            const int v = edge_v[static_cast<std::size_t>(edge)];
            adjacency[static_cast<std::size_t>(cursor[static_cast<std::size_t>(u)]++)] = v;
            adjacency[static_cast<std::size_t>(cursor[static_cast<std::size_t>(v)]++)] = u;
        }

        std::fill_n(parent_vertex.begin(), _size, -2);
        int timer = 0;
        int stack_size = 1;
        parent_vertex[static_cast<std::size_t>(root)] = -1;
        depth_vertex[static_cast<std::size_t>(root)] = 0;
        tin_vertex[static_cast<std::size_t>(root)] = timer++;
        dfs_vertex[0] = root;
        dfs_next[0] = offset[static_cast<std::size_t>(root)];

        while(stack_size > 0){
            const int top = stack_size - 1;
            const int vertex = dfs_vertex[static_cast<std::size_t>(top)];
            int& next = dfs_next[static_cast<std::size_t>(top)];
            if(next == offset[static_cast<std::size_t>(vertex + 1)]){
                tout_vertex[static_cast<std::size_t>(vertex)] = timer;
                --stack_size;
                continue;
            }
            const int to = adjacency[static_cast<std::size_t>(next++)];
            if(to == parent_vertex[static_cast<std::size_t>(vertex)]) continue;
            if(parent_vertex[static_cast<std::size_t>(to)] != -2)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: graph is not a tree (AuxiliaryTree::build)."
                );
            }
            parent_vertex[static_cast<std::size_t>(to)] = vertex;
            depth_vertex[static_cast<std::size_t>(to)] =
                depth_vertex[static_cast<std::size_t>(vertex)] + 1;
            tin_vertex[static_cast<std::size_t>(to)] = timer++;
            dfs_vertex[static_cast<std::size_t>(stack_size)] = to;
            dfs_next[static_cast<std::size_t>(stack_size)] =
                offset[static_cast<std::size_t>(to)];
            ++stack_size;
        }
        if(timer != _size)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: graph is not connected (AuxiliaryTree::build)."
            );
        }

        for(int vertex = 0; vertex < _size; ++vertex){
            ancestors[static_cast<std::size_t>(vertex)] =
                parent_vertex[static_cast<std::size_t>(vertex)] == -1
                    ? vertex
                    : parent_vertex[static_cast<std::size_t>(vertex)];
        }
        for(int level = 1; level < LOG; ++level){
            for(int vertex = 0; vertex < _size; ++vertex){
                ancestors[static_cast<std::size_t>(level * MAX_SIZE + vertex)] =
                    ancestor(level - 1, ancestor(level - 1, vertex));
            }
        }
        prepared = true;
    }

    int depth(int vertex) const{
        require_prepared("library assertion fault: AuxiliaryTree is not built (depth).");
        check_vertex(vertex, "library assertion fault: range violation (AuxiliaryTree::depth).");
        return depth_vertex[static_cast<std::size_t>(vertex)];
    }

    int tin(int vertex) const{
        require_prepared("library assertion fault: AuxiliaryTree is not built (tin).");
        check_vertex(vertex, "library assertion fault: range violation (AuxiliaryTree::tin).");
        return tin_vertex[static_cast<std::size_t>(vertex)];
    }

    bool is_ancestor(int ancestor_vertex, int descendant_vertex) const{
        require_prepared("library assertion fault: AuxiliaryTree is not built (is_ancestor).");
        check_vertex(
            ancestor_vertex,
            "library assertion fault: range violation (AuxiliaryTree::is_ancestor)."
        );
        check_vertex(
            descendant_vertex,
            "library assertion fault: range violation (AuxiliaryTree::is_ancestor)."
        );
        return tin_vertex[static_cast<std::size_t>(ancestor_vertex)] <=
                tin_vertex[static_cast<std::size_t>(descendant_vertex)] &&
            tin_vertex[static_cast<std::size_t>(descendant_vertex)] <
                tout_vertex[static_cast<std::size_t>(ancestor_vertex)];
    }

    int lca(int u, int v) const{
        require_prepared("library assertion fault: AuxiliaryTree is not built (lca).");
        check_vertex(u, "library assertion fault: range violation (AuxiliaryTree::lca).");
        check_vertex(v, "library assertion fault: range violation (AuxiliaryTree::lca).");
        if(is_ancestor(u, v)) return u;
        if(is_ancestor(v, u)) return v;
        for(int level = LOG - 1; level >= 0; --level){
            const int next = ancestor(level, u);
            if(!is_ancestor(next, v)) u = next;
        }
        return ancestor(0, u);
    }

    int dist(int u, int v) const{
        const int w = lca(u, v);
        return depth_vertex[static_cast<std::size_t>(u)] +
            depth_vertex[static_cast<std::size_t>(v)] -
            2 * depth_vertex[static_cast<std::size_t>(w)];
    }

    int compress(std::span<const int> vertices){
        require_prepared("library assertion fault: AuxiliaryTree is not built (compress).");
        if(vertices.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity exceeded (AuxiliaryTree::compress)."
            );
        }
        int selected_size = static_cast<int>(vertices.size());
        for(int index = 0; index < selected_size; ++index){
            const int vertex = vertices[static_cast<std::size_t>(index)];
            check_vertex(
                vertex,
                "library assertion fault: range violation (AuxiliaryTree::compress)."
            );
            selected_vertices[static_cast<std::size_t>(index)] = vertex;
        }
        auto tin_less = [this](int lhs, int rhs){
            return tin_vertex[static_cast<std::size_t>(lhs)] <
                tin_vertex[static_cast<std::size_t>(rhs)];
        };
        std::sort(selected_vertices.begin(), selected_vertices.begin() + selected_size, tin_less);
        selected_size = static_cast<int>(std::unique(
            selected_vertices.begin(), selected_vertices.begin() + selected_size
        ) - selected_vertices.begin());

        if(selected_size == 0){
            _compressed_size = 0;
            return 0;
        }
        for(int index = 0; index < selected_size; ++index){
            compressed_vertices[static_cast<std::size_t>(index)] =
                selected_vertices[static_cast<std::size_t>(index)];
        }
        int candidate_size = selected_size;
        for(int index = 0; index + 1 < selected_size; ++index){
            compressed_vertices[static_cast<std::size_t>(candidate_size++)] = lca(
                selected_vertices[static_cast<std::size_t>(index)],
                selected_vertices[static_cast<std::size_t>(index + 1)]
            );
        }
        std::sort(
            compressed_vertices.begin(),
            compressed_vertices.begin() + candidate_size,
            tin_less
        );
        _compressed_size = static_cast<int>(std::unique(
            compressed_vertices.begin(), compressed_vertices.begin() + candidate_size
        ) - compressed_vertices.begin());

        int stack_size = 0;
        for(int index = 0; index < _compressed_size; ++index){
            const int vertex = compressed_vertices[static_cast<std::size_t>(index)];
            while(stack_size > 0 && !is_ancestor(
                compressed_vertices[static_cast<std::size_t>(
                    compressed_stack[static_cast<std::size_t>(stack_size - 1)]
                )],
                vertex
            )){
                --stack_size;
            }
            const int parent_index = stack_size == 0
                ? -1
                : compressed_stack[static_cast<std::size_t>(stack_size - 1)];
            compressed_parents[static_cast<std::size_t>(index)] = parent_index;
            compressed_edge_lengths[static_cast<std::size_t>(index)] = parent_index == -1
                ? 0
                : depth_vertex[static_cast<std::size_t>(vertex)] -
                    depth_vertex[static_cast<std::size_t>(
                        compressed_vertices[static_cast<std::size_t>(parent_index)]
                    )];
            compressed_selected[static_cast<std::size_t>(index)] = std::binary_search(
                selected_vertices.begin(),
                selected_vertices.begin() + selected_size,
                vertex,
                tin_less
            );
            compressed_stack[static_cast<std::size_t>(stack_size++)] = index;
        }
        return _compressed_size;
    }

    int compressed_size() const{
        return _compressed_size;
    }

    int vertex(int index) const{
        check_compressed_index(
            index,
            "library assertion fault: range violation (AuxiliaryTree::vertex)."
        );
        return compressed_vertices[static_cast<std::size_t>(index)];
    }

    int parent_index(int index) const{
        check_compressed_index(
            index,
            "library assertion fault: range violation (AuxiliaryTree::parent_index)."
        );
        return compressed_parents[static_cast<std::size_t>(index)];
    }

    int edge_length(int index) const{
        check_compressed_index(
            index,
            "library assertion fault: range violation (AuxiliaryTree::edge_length)."
        );
        return compressed_edge_lengths[static_cast<std::size_t>(index)];
    }

    bool selected(int index) const{
        check_compressed_index(
            index,
            "library assertion fault: range violation (AuxiliaryTree::selected)."
        );
        return compressed_selected[static_cast<std::size_t>(index)];
    }
};

template<int MAX_SIZE>
using VirtualTree = AuxiliaryTree<MAX_SIZE>;

#endif  // CPPLIB_SRC_ALGORITHM_TREE_AUXILIARY_TREE_HPP_INCLUDED
