#pragma once

#include <array>
#include <stdexcept>
#include <utility>

template<int MAX_SIZE, int MAX_HISTORY>
struct RollbackDSU{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_HISTORY >= 0);

private:
    struct Change{
        int u;
        int parent_u;
        int v;
        int parent_v;
        int groups;
    };

    int _n = 0;
    int group_count = 0;
    int history_size = 0;
    std::array<int, MAX_SIZE> parent_or_size{};
    std::array<Change, MAX_HISTORY> history{};

    void check_vertex(int v, const char* message) const{
        if(v < 0 || _n <= v)[[unlikely]] throw std::runtime_error(message);
    }
    void save(int u, int v){
        if(history_size == MAX_HISTORY)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (history).");
        }
        history[history_size++] = {u, parent_or_size[u], v, parent_or_size[v], group_count};
    }
    int leader_unchecked(int v) const{
        if(parent_or_size[v] < 0) return v;
        return leader_unchecked(parent_or_size[v]);
    }

public:
    explicit RollbackDSU(int n = MAX_SIZE): _n(n), group_count(n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        for(int i = 0; i < _n; i++) parent_or_size[i] = -1;
    }

    int size() const{ return _n; }
    int groups() const{ return group_count; }
    int snapshot() const{ return history_size; }

    int leader(int v) const{
        check_vertex(v, "library assertion fault: range violation (leader).");
        return leader_unchecked(v);
    }
    bool same(int u, int v) const{
        check_vertex(u, "library assertion fault: range violation (same).");
        check_vertex(v, "library assertion fault: range violation (same).");
        return leader_unchecked(u) == leader_unchecked(v);
    }
    int component_size(int v) const{
        check_vertex(v, "library assertion fault: range violation (component_size).");
        return -parent_or_size[leader_unchecked(v)];
    }
    bool merge(int u, int v){
        check_vertex(u, "library assertion fault: range violation (merge).");
        check_vertex(v, "library assertion fault: range violation (merge).");
        u = leader_unchecked(u);
        v = leader_unchecked(v);
        if(u == v){
            save(u, v);
            return false;
        }
        if(-parent_or_size[u] < -parent_or_size[v]) std::swap(u, v);
        save(u, v);
        parent_or_size[u] += parent_or_size[v];
        parent_or_size[v] = u;
        group_count--;
        return true;
    }
    void undo(){
        if(history_size == 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: history is empty (undo).");
        }
        Change c = history[--history_size];
        parent_or_size[c.u] = c.parent_u;
        parent_or_size[c.v] = c.parent_v;
        group_count = c.groups;
    }
    void rollback(int snapshot_id){
        if(snapshot_id < 0 || history_size < snapshot_id)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (rollback).");
        }
        while(history_size > snapshot_id) undo();
    }
};
