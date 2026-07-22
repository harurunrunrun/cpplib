#ifndef CPPLIB_SRC_STRUCTURE_DSU_WEIGHTED_DSU_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_DSU_WEIGHTED_DSU_HPP_INCLUDED

#include <array>
#include <optional>
#include <stdexcept>
#include <utility>

template<class T, int MAX_SIZE>
struct WeightedDSU{
    static_assert(MAX_SIZE > 0);

private:
    int _n = 0;
    int group_count = 0;
    std::array<int, MAX_SIZE> parent_or_size{};
    std::array<T, MAX_SIZE> diff_weight{};

    void check_vertex(int v, const char* message) const{
        if(v < 0 || _n <= v)[[unlikely]] throw std::runtime_error(message);
    }

public:
    explicit WeightedDSU(int n = MAX_SIZE): _n(n), group_count(n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        for(int i = 0; i < _n; i++){
            parent_or_size[i] = -1;
            diff_weight[i] = T();
        }
    }

    int size() const{ return _n; }
    int groups() const{ return group_count; }

    int leader(int v){
        check_vertex(v, "library assertion fault: range violation (leader).");
        if(parent_or_size[v] < 0) return v;
        int p = parent_or_size[v];
        int r = leader(p);
        diff_weight[v] = diff_weight[v] + diff_weight[p];
        return parent_or_size[v] = r;
    }
    T weight(int v){
        check_vertex(v, "library assertion fault: range violation (weight).");
        leader(v);
        return diff_weight[v];
    }
    bool same(int u, int v){
        check_vertex(u, "library assertion fault: range violation (same).");
        check_vertex(v, "library assertion fault: range violation (same).");
        return leader(u) == leader(v);
    }
    int component_size(int v){
        check_vertex(v, "library assertion fault: range violation (component_size).");
        return -parent_or_size[leader(v)];
    }
    T diff(int u, int v){
        check_vertex(u, "library assertion fault: range violation (diff).");
        check_vertex(v, "library assertion fault: range violation (diff).");
        return weight(v) - weight(u);
    }

    std::optional<T> diff_if_connected(int u, int v){
        check_vertex(u, "library assertion fault: range violation (diff_if_connected).");
        check_vertex(v, "library assertion fault: range violation (diff_if_connected).");
        const int root_u = leader(u);
        const int root_v = leader(v);
        if(root_u != root_v) return std::nullopt;
        return diff_weight[v] - diff_weight[u];
    }

    bool merge(int u, int v, T w){
        check_vertex(u, "library assertion fault: range violation (merge).");
        check_vertex(v, "library assertion fault: range violation (merge).");
        w = w + weight(u);
        w = w - weight(v);
        u = leader(u);
        v = leader(v);
        if(u == v) return w == T();
        if(-parent_or_size[u] < -parent_or_size[v]){
            std::swap(u, v);
            w = T() - w;
        }
        parent_or_size[u] += parent_or_size[v];
        parent_or_size[v] = u;
        diff_weight[v] = w;
        group_count--;
        return true;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_DSU_WEIGHTED_DSU_HPP_INCLUDED
