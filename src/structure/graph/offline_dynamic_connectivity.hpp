#ifndef CPPLIB_SRC_STRUCTURE_GRAPH_OFFLINE_DYNAMIC_CONNECTIVITY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_GRAPH_OFFLINE_DYNAMIC_CONNECTIVITY_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <map>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../dsu/rollback_dsu.hpp"

template<int MAX_SIZE, int MAX_TIME, int MAX_HISTORY>
struct OfflineDynamicConnectivity{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_TIME > 0);
    static_assert(MAX_HISTORY >= 0);

    struct Edge{
        int u;
        int v;
    };

private:
    static constexpr int SEG_SIZE = MAX_TIME * 4 + 5;

    struct State{
        std::array<std::vector<Edge>, SEG_SIZE> segment;
        std::map<std::pair<int, int>, std::vector<int>> active;
    };

    int _n = 0;
    int _time_size = 0;
    bool built = false;
    std::unique_ptr<State> state;
    std::unique_ptr<RollbackDSU<MAX_SIZE, MAX_HISTORY>> dsu;

    void check_vertex(int v, const char* message) const{
        if(v < 0 || _n <= v)[[unlikely]] throw std::runtime_error(message);
    }
    void check_time(int t, const char* message) const{
        if(t < 0 || _time_size <= t)[[unlikely]] throw std::runtime_error(message);
    }
    void check_not_built(const char* message) const{
        if(built)[[unlikely]] throw std::runtime_error(message);
    }
    std::pair<int, int> normalize(int u, int v) const{
        check_vertex(u, "library assertion fault: range violation (edge).");
        check_vertex(v, "library assertion fault: range violation (edge).");
        if(v < u) std::swap(u, v);
        return {u, v};
    }
    void add_interval(int ql, int qr, Edge edge, int node, int nl, int nr){
        if(qr <= nl || nr <= ql) return;
        if(ql <= nl && nr <= qr){
            state->segment[node].push_back(edge);
            return;
        }
        int mid = (nl + nr) >> 1;
        add_interval(ql, qr, edge, node << 1, nl, mid);
        add_interval(ql, qr, edge, node << 1 | 1, mid, nr);
    }
    void add_interval(int l, int r, int u, int v){
        if(l >= r || u == v) return;
        add_interval(l, r, Edge{u, v}, 1, 0, _time_size);
    }
    void build(){
        if(built) return;
        for(auto& item: state->active){
            int u = item.first.first;
            int v = item.first.second;
            for(int l: item.second) add_interval(l, _time_size, u, v);
        }
        state->active.clear();
        built = true;
    }

    template<class F>
    void dfs(int node, int l, int r, F& f){
        int snapshot = dsu->snapshot();
        for(const Edge& edge: state->segment[node]){
            dsu->merge(edge.u, edge.v);
        }
        if(r - l == 1){
            f(l, *dsu);
        }else{
            int mid = (l + r) >> 1;
            dfs(node << 1, l, mid, f);
            dfs(node << 1 | 1, mid, r, f);
        }
        dsu->rollback(snapshot);
    }

public:
    explicit OfflineDynamicConnectivity(int n = MAX_SIZE, int time_size = MAX_TIME):
        _n(n), _time_size(time_size), state(nullptr), dsu(nullptr){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        if(time_size < 0 || MAX_TIME < time_size)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        state = std::make_unique<State>();
    }

    OfflineDynamicConnectivity(const OfflineDynamicConnectivity&) = delete;
    OfflineDynamicConnectivity& operator=(const OfflineDynamicConnectivity&) = delete;
    OfflineDynamicConnectivity(OfflineDynamicConnectivity&&) = default;
    OfflineDynamicConnectivity& operator=(OfflineDynamicConnectivity&&) = default;

    int size() const{ return _n; }
    int time_size() const{ return _time_size; }

    void add_edge(int time, int u, int v){
        check_not_built("library assertion fault: already built (add_edge).");
        check_time(time, "library assertion fault: range violation (add_edge).");
        auto edge = normalize(u, v);
        state->active[edge].push_back(time);
    }
    void erase_edge(int time, int u, int v){
        check_not_built("library assertion fault: already built (erase_edge).");
        check_time(time, "library assertion fault: range violation (erase_edge).");
        auto edge = normalize(u, v);
        auto it = state->active.find(edge);
        if(it == state->active.end() || it->second.empty())[[unlikely]]{
            throw std::runtime_error("library assertion fault: edge is not active (erase_edge).");
        }
        int l = it->second.back();
        it->second.pop_back();
        add_interval(l, time, edge.first, edge.second);
        if(it->second.empty()) state->active.erase(it);
    }

    template<class F>
    void run(F&& f){
        build();
        dsu = std::make_unique<RollbackDSU<MAX_SIZE, MAX_HISTORY>>(_n);
        if(_time_size == 0) return;
        dfs(1, 0, _time_size, f);
        dsu.reset();
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_GRAPH_OFFLINE_DYNAMIC_CONNECTIVITY_HPP_INCLUDED
