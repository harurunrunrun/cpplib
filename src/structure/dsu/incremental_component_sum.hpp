#ifndef CPPLIB_SRC_STRUCTURE_DSU_INCREMENTAL_COMPONENT_SUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_DSU_INCREMENTAL_COMPONENT_SUM_HPP_INCLUDED

#include <array>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T, int MAX_SIZE>
struct IncrementalComponentSum{
    static_assert(MAX_SIZE > 0);

private:
    int _n = 0;
    int group_count = 0;
    std::array<int, MAX_SIZE> parent_or_size{};
    std::array<T, MAX_SIZE> component_value{};

    void check_vertex(int vertex, const char* message) const{
        if(vertex < 0 || _n <= vertex)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void initialize(int n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        _n = n;
        group_count = n;
        for(int vertex = 0; vertex < _n; ++vertex){
            parent_or_size[vertex] = -1;
            component_value[vertex] = T{};
        }
    }

public:
    explicit IncrementalComponentSum(int n = MAX_SIZE){
        initialize(n);
    }

    explicit IncrementalComponentSum(const std::vector<T>& values){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        initialize(static_cast<int>(values.size()));
        for(int vertex = 0; vertex < _n; ++vertex){
            component_value[vertex] = values[vertex];
        }
    }

    int size() const{ return _n; }
    int groups() const{ return group_count; }

    int leader(int vertex){
        check_vertex(
            vertex,
            "library assertion fault: range violation (leader)."
        );
        if(parent_or_size[vertex] < 0) return vertex;
        return parent_or_size[vertex] = leader(parent_or_size[vertex]);
    }

    bool same(int first, int second){
        check_vertex(
            first,
            "library assertion fault: range violation (same)."
        );
        check_vertex(
            second,
            "library assertion fault: range violation (same)."
        );
        return leader(first) == leader(second);
    }

    int component_size(int vertex){
        check_vertex(
            vertex,
            "library assertion fault: range violation (component_size)."
        );
        return -parent_or_size[leader(vertex)];
    }

    int size(int vertex){
        return component_size(vertex);
    }

    T component_sum(int vertex){
        check_vertex(
            vertex,
            "library assertion fault: range violation (component_sum)."
        );
        return component_value[leader(vertex)];
    }

    void add_vertex(int vertex, const T& delta){
        check_vertex(
            vertex,
            "library assertion fault: range violation (add_vertex)."
        );
        const int root = leader(vertex);
        component_value[root] = component_value[root] + delta;
    }

    bool add_edge(int first, int second){
        check_vertex(
            first,
            "library assertion fault: range violation (add_edge)."
        );
        check_vertex(
            second,
            "library assertion fault: range violation (add_edge)."
        );
        first = leader(first);
        second = leader(second);
        if(first == second) return false;
        if(-parent_or_size[first] < -parent_or_size[second]){
            std::swap(first, second);
        }
        parent_or_size[first] += parent_or_size[second];
        parent_or_size[second] = first;
        component_value[first] =
            component_value[first] + component_value[second];
        --group_count;
        return true;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_DSU_INCREMENTAL_COMPONENT_SUM_HPP_INCLUDED
