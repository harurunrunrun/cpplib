#ifndef CPPLIB_SRC_STRUCTURE_DSU_COMMUTATIVE_MONOID_DSU_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_DSU_COMMUTATIVE_MONOID_DSU_HPP_INCLUDED

#include <array>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<auto Monoid, int MAX_SIZE>
struct CommutativeMonoidDSU{
    static_assert(MAX_SIZE > 0);

    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

private:
    int _n = 0;
    int group_count = 0;
    std::array<int, MAX_SIZE> parent_or_size{};
    std::array<S, MAX_SIZE> product{};

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
            product[vertex] = Monoid.e();
        }
    }

public:
    explicit CommutativeMonoidDSU(int n = MAX_SIZE){
        initialize(n);
    }

    explicit CommutativeMonoidDSU(const std::vector<S>& values){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        initialize(static_cast<int>(values.size()));
        for(int vertex = 0; vertex < _n; ++vertex){
            product[vertex] = values[vertex];
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

    S component_product(int vertex){
        check_vertex(
            vertex,
            "library assertion fault: range violation (component_product)."
        );
        return product[leader(vertex)];
    }

    bool merge(int first, int second){
        check_vertex(
            first,
            "library assertion fault: range violation (merge)."
        );
        check_vertex(
            second,
            "library assertion fault: range violation (merge)."
        );
        first = leader(first);
        second = leader(second);
        if(first == second) return false;
        if(-parent_or_size[first] < -parent_or_size[second]){
            std::swap(first, second);
        }
        parent_or_size[first] += parent_or_size[second];
        parent_or_size[second] = first;
        product[first] = Monoid.op(product[first], product[second]);
        --group_count;
        return true;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_DSU_COMMUTATIVE_MONOID_DSU_HPP_INCLUDED
