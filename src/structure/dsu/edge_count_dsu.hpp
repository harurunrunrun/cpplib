#pragma once

#include <array>
#include <stdexcept>
#include <utility>

template<int MAX_SIZE>
struct EdgeCountDSU{
    static_assert(MAX_SIZE > 0);

private:
    int _n = 0;
    int group_count = 0;
    std::array<int, MAX_SIZE> parent_or_size{};
    std::array<int, MAX_SIZE> edge_count{};

    void check_vertex(int v, const char* message) const{
        if(v < 0 || _n <= v)[[unlikely]] throw std::runtime_error(message);
    }

public:
    explicit EdgeCountDSU(int n = MAX_SIZE): _n(n), group_count(n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        for(int i = 0; i < _n; i++){
            parent_or_size[i] = -1;
            edge_count[i] = 0;
        }
    }

    int size() const{ return _n; }
    int groups() const{ return group_count; }

    int leader(int v){
        check_vertex(v, "library assertion fault: range violation (leader).");
        if(parent_or_size[v] < 0) return v;
        return parent_or_size[v] = leader(parent_or_size[v]);
    }

    bool same(int u, int v){
        check_vertex(u, "library assertion fault: range violation (same).");
        check_vertex(v, "library assertion fault: range violation (same).");
        return leader(u) == leader(v);
    }

    int component_size(int v){
        check_vertex(
            v,
            "library assertion fault: range violation (component_size)."
        );
        return -parent_or_size[leader(v)];
    }

    int edges(int v){
        check_vertex(v, "library assertion fault: range violation (edges).");
        return edge_count[leader(v)];
    }

    int excess_edges(int v){
        check_vertex(
            v,
            "library assertion fault: range violation (excess_edges)."
        );
        int r = leader(v);
        return edge_count[r] + parent_or_size[r] + 1;
    }

    bool is_tree(int v){
        check_vertex(v, "library assertion fault: range violation (is_tree).");
        int r = leader(v);
        return edge_count[r] == -parent_or_size[r] - 1;
    }

    bool is_unicyclic(int v){
        check_vertex(
            v,
            "library assertion fault: range violation (is_unicyclic)."
        );
        int r = leader(v);
        return edge_count[r] == -parent_or_size[r];
    }

    bool add_edge(int u, int v){
        check_vertex(u, "library assertion fault: range violation (add_edge).");
        check_vertex(v, "library assertion fault: range violation (add_edge).");

        u = leader(u);
        v = leader(v);

        if(u == v){
            edge_count[u]++;
            return false;
        }

        if(-parent_or_size[u] < -parent_or_size[v]) std::swap(u, v);
        parent_or_size[u] += parent_or_size[v];
        parent_or_size[v] = u;
        edge_count[u] += edge_count[v] + 1;
        group_count--;
        return true;
    }
};
