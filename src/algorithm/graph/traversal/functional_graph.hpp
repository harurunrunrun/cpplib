#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_FUNCTIONAL_GRAPH_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_FUNCTIONAL_GRAPH_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <span>
#include <stdexcept>
#include <vector>

template<int MAX_SIZE>
struct FunctionalGraph{
    static_assert(MAX_SIZE >= 0);

private:
    static constexpr int LOG = 64;

    int _size = 0;
    int _component_count = 0;
    int _cycle_vertex_count = 0;
    std::array<int, static_cast<std::size_t>(MAX_SIZE)> _successor;
    std::array<std::array<int, static_cast<std::size_t>(MAX_SIZE)>, LOG> _jump;
    std::array<int, static_cast<std::size_t>(MAX_SIZE)> _component;
    std::array<int, static_cast<std::size_t>(MAX_SIZE)> _distance_to_cycle;
    std::array<int, static_cast<std::size_t>(MAX_SIZE)> _cycle_entry;
    std::array<int, static_cast<std::size_t>(MAX_SIZE)> _cycle_position;
    std::array<int, static_cast<std::size_t>(MAX_SIZE)> _cycle_length;
    std::array<int, static_cast<std::size_t>(MAX_SIZE)> _component_size;
    std::array<int, static_cast<std::size_t>(MAX_SIZE) + 1> _cycle_offset;
    std::array<int, static_cast<std::size_t>(MAX_SIZE)> _cycle_vertices;
    std::array<int, static_cast<std::size_t>(MAX_SIZE)> _work_indegree;
    std::array<int, static_cast<std::size_t>(MAX_SIZE)> _work_order;

    void check_vertex(int v, const char* message) const{
        if(v < 0 || _size <= v)[[unlikely]] throw std::runtime_error(message);
    }

    void check_component(int id, const char* message) const{
        if(id < 0 || _component_count <= id)[[unlikely]] throw std::runtime_error(message);
    }

    template<class Getter>
    void build_impl(int n, Getter get_successor){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation (FunctionalGraph::build)."
            );
        }
        _size = n;
        for(int v = 0; v < n; ++v){
            const int to = get_successor(v);
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation (FunctionalGraph::build)."
                );
            }
            _successor[static_cast<std::size_t>(v)] = to;
            _jump[0][static_cast<std::size_t>(v)] = to;
            _work_indegree[static_cast<std::size_t>(v)] = 0;
            _component[static_cast<std::size_t>(v)] = -1;
        }
        for(int v = 0; v < n; ++v){
            ++_work_indegree[static_cast<std::size_t>(
                _successor[static_cast<std::size_t>(v)]
            )];
        }

        int removed_count = 0;
        for(int v = 0; v < n; ++v){
            if(_work_indegree[static_cast<std::size_t>(v)] == 0){
                _work_order[static_cast<std::size_t>(removed_count++)] = v;
            }
        }
        for(int head = 0; head < removed_count; ++head){
            const int v = _work_order[static_cast<std::size_t>(head)];
            const int to = _successor[static_cast<std::size_t>(v)];
            if(--_work_indegree[static_cast<std::size_t>(to)] == 0){
                _work_order[static_cast<std::size_t>(removed_count++)] = to;
            }
        }

        _component_count = 0;
        _cycle_vertex_count = 0;
        _cycle_offset[0] = 0;
        for(int start = 0; start < n; ++start){
            if(_work_indegree[static_cast<std::size_t>(start)] == 0 ||
               _component[static_cast<std::size_t>(start)] != -1){
                continue;
            }
            const int id = _component_count++;
            _component_size[static_cast<std::size_t>(id)] = 0;
            int v = start;
            int position = 0;
            do{
                _component[static_cast<std::size_t>(v)] = id;
                _distance_to_cycle[static_cast<std::size_t>(v)] = 0;
                _cycle_entry[static_cast<std::size_t>(v)] = v;
                _cycle_position[static_cast<std::size_t>(v)] = position++;
                _cycle_vertices[static_cast<std::size_t>(_cycle_vertex_count++)] = v;
                ++_component_size[static_cast<std::size_t>(id)];
                v = _successor[static_cast<std::size_t>(v)];
            }while(v != start);
            _cycle_offset[static_cast<std::size_t>(id + 1)] = _cycle_vertex_count;
            const int length = position;
            for(int i = _cycle_offset[static_cast<std::size_t>(id)];
                i < _cycle_vertex_count;
                ++i){
                _cycle_length[static_cast<std::size_t>(
                    _cycle_vertices[static_cast<std::size_t>(i)]
                )] = length;
            }
        }

        for(int i = removed_count - 1; 0 <= i; --i){
            const int v = _work_order[static_cast<std::size_t>(i)];
            const int to = _successor[static_cast<std::size_t>(v)];
            const int id = _component[static_cast<std::size_t>(to)];
            _component[static_cast<std::size_t>(v)] = id;
            _distance_to_cycle[static_cast<std::size_t>(v)] =
                _distance_to_cycle[static_cast<std::size_t>(to)] + 1;
            _cycle_entry[static_cast<std::size_t>(v)] =
                _cycle_entry[static_cast<std::size_t>(to)];
            _cycle_position[static_cast<std::size_t>(v)] = -1;
            _cycle_length[static_cast<std::size_t>(v)] =
                _cycle_length[static_cast<std::size_t>(to)];
            ++_component_size[static_cast<std::size_t>(id)];
        }

        for(int level = 1; level < LOG; ++level){
            for(int v = 0; v < n; ++v){
                _jump[static_cast<std::size_t>(level)][static_cast<std::size_t>(v)] =
                    _jump[static_cast<std::size_t>(level - 1)][static_cast<std::size_t>(
                        _jump[static_cast<std::size_t>(level - 1)][static_cast<std::size_t>(v)]
                    )];
            }
        }
    }

public:
    FunctionalGraph() = default;

    explicit FunctionalGraph(const std::vector<int>& successor){
        build(successor);
    }

    template<std::size_t N>
    explicit FunctionalGraph(const std::array<int, N>& successor){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
        build(successor);
    }

    void build(const std::vector<int>& successor){
        build_impl(static_cast<int>(successor.size()), [&](int v){
            return successor[static_cast<std::size_t>(v)];
        });
    }

    template<std::size_t N>
    void build(const std::array<int, N>& successor){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
        build_impl(static_cast<int>(N), [&](int v){
            return successor[static_cast<std::size_t>(v)];
        });
    }

    int size() const{
        return _size;
    }

    bool empty() const{
        return _size == 0;
    }

    int component_count() const{
        return _component_count;
    }

    int successor(int v) const{
        check_vertex(v, "library assertion fault: range violation (FunctionalGraph::successor).");
        return _successor[static_cast<std::size_t>(v)];
    }

    int component_id(int v) const{
        check_vertex(v, "library assertion fault: range violation (FunctionalGraph::component_id).");
        return _component[static_cast<std::size_t>(v)];
    }

    int cycle_id(int v) const{
        return component_id(v);
    }

    bool on_cycle(int v) const{
        check_vertex(v, "library assertion fault: range violation (FunctionalGraph::on_cycle).");
        return _distance_to_cycle[static_cast<std::size_t>(v)] == 0;
    }

    int distance_to_cycle(int v) const{
        check_vertex(
            v,
            "library assertion fault: range violation (FunctionalGraph::distance_to_cycle)."
        );
        return _distance_to_cycle[static_cast<std::size_t>(v)];
    }

    int cycle_entry(int v) const{
        check_vertex(v, "library assertion fault: range violation (FunctionalGraph::cycle_entry).");
        return _cycle_entry[static_cast<std::size_t>(v)];
    }

    int cycle_position(int v) const{
        check_vertex(
            v,
            "library assertion fault: range violation (FunctionalGraph::cycle_position)."
        );
        return _cycle_position[static_cast<std::size_t>(v)];
    }

    int cycle_length(int v) const{
        check_vertex(v, "library assertion fault: range violation (FunctionalGraph::cycle_length).");
        return _cycle_length[static_cast<std::size_t>(v)];
    }

    int orbit_size(int v) const{
        check_vertex(v, "library assertion fault: range violation (FunctionalGraph::orbit_size).");
        return _distance_to_cycle[static_cast<std::size_t>(v)] +
            _cycle_length[static_cast<std::size_t>(v)];
    }

    int component_size(int id) const{
        check_component(
            id,
            "library assertion fault: range violation (FunctionalGraph::component_size)."
        );
        return _component_size[static_cast<std::size_t>(id)];
    }

    int cycle_size(int id) const{
        check_component(id, "library assertion fault: range violation (FunctionalGraph::cycle_size).");
        return _cycle_offset[static_cast<std::size_t>(id + 1)] -
            _cycle_offset[static_cast<std::size_t>(id)];
    }

    std::span<const int> cycle(int id) const{
        check_component(id, "library assertion fault: range violation (FunctionalGraph::cycle).");
        const int begin = _cycle_offset[static_cast<std::size_t>(id)];
        return std::span<const int>(
            _cycle_vertices.data() + begin,
            static_cast<std::size_t>(cycle_size(id))
        );
    }

    int jump(int v, unsigned long long step) const{
        check_vertex(v, "library assertion fault: range violation (FunctionalGraph::jump).");
        int level = 0;
        while(step != 0){
            if(step & 1ULL){
                v = _jump[static_cast<std::size_t>(level)][static_cast<std::size_t>(v)];
            }
            step >>= 1;
            ++level;
        }
        return v;
    }

    long long steps_to(int from, int to) const{
        check_vertex(from, "library assertion fault: range violation (FunctionalGraph::steps_to).");
        check_vertex(to, "library assertion fault: range violation (FunctionalGraph::steps_to).");
        if(_component[static_cast<std::size_t>(from)] !=
           _component[static_cast<std::size_t>(to)]){
            return -1;
        }
        if(_distance_to_cycle[static_cast<std::size_t>(to)] != 0){
            const int difference = _distance_to_cycle[static_cast<std::size_t>(from)] -
                _distance_to_cycle[static_cast<std::size_t>(to)];
            if(difference < 0 || jump(from, static_cast<unsigned long long>(difference)) != to){
                return -1;
            }
            return difference;
        }

        const int entry = _cycle_entry[static_cast<std::size_t>(from)];
        const int length = _cycle_length[static_cast<std::size_t>(from)];
        int around = _cycle_position[static_cast<std::size_t>(to)] -
            _cycle_position[static_cast<std::size_t>(entry)];
        if(around < 0) around += length;
        return static_cast<long long>(_distance_to_cycle[static_cast<std::size_t>(from)]) + around;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_TRAVERSAL_FUNCTIONAL_GRAPH_HPP_INCLUDED
