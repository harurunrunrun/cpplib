#pragma once

#include <array>
#include <stdexcept>
#include <utility>

template<class T, int MAX_SIZE, int MAX_HISTORY>
struct RollbackWeightedDSU{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_HISTORY >= 0);

private:
    struct Change{
        int u;
        int parent_u;
        T diff_u;
        int v;
        int parent_v;
        T diff_v;
        int groups;
    };

    int _n = 0;
    int group_count = 0;
    int history_size = 0;
    std::array<int, MAX_SIZE> parent_or_size{};
    std::array<T, MAX_SIZE> diff_weight{};
    std::array<Change, MAX_HISTORY> history{};

    void check_vertex(int v, const char* message) const{
        if(v < 0 || _n <= v)[[unlikely]] throw std::runtime_error(message);
    }

    void save(int u, int v){
        if(history_size == MAX_HISTORY)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (history).");
        }
        history[history_size++] = {
            u, parent_or_size[static_cast<std::size_t>(u)], diff_weight[static_cast<std::size_t>(u)],
            v, parent_or_size[static_cast<std::size_t>(v)], diff_weight[static_cast<std::size_t>(v)],
            group_count
        };
    }

    std::pair<int, T> leader_weight_unchecked(int v) const{
        T weight = T();
        while(parent_or_size[static_cast<std::size_t>(v)] >= 0){
            weight = weight + diff_weight[static_cast<std::size_t>(v)];
            v = parent_or_size[static_cast<std::size_t>(v)];
        }
        return {v, weight};
    }

public:
    explicit RollbackWeightedDSU(int n = MAX_SIZE): _n(n), group_count(n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        for(int i = 0; i < _n; i++){
            parent_or_size[static_cast<std::size_t>(i)] = -1;
            diff_weight[static_cast<std::size_t>(i)] = T();
        }
    }

    int size() const{ return _n; }
    int groups() const{ return group_count; }
    int snapshot() const{ return history_size; }

    int leader(int v) const{
        check_vertex(v, "library assertion fault: range violation (leader).");
        return leader_weight_unchecked(v).first;
    }

    T weight(int v) const{
        check_vertex(v, "library assertion fault: range violation (weight).");
        return leader_weight_unchecked(v).second;
    }

    bool same(int u, int v) const{
        check_vertex(u, "library assertion fault: range violation (same).");
        check_vertex(v, "library assertion fault: range violation (same).");
        return leader_weight_unchecked(u).first == leader_weight_unchecked(v).first;
    }

    int component_size(int v) const{
        check_vertex(v, "library assertion fault: range violation (component_size).");
        return -parent_or_size[static_cast<std::size_t>(leader(v))];
    }

    T diff(int u, int v) const{
        check_vertex(u, "library assertion fault: range violation (diff).");
        check_vertex(v, "library assertion fault: range violation (diff).");
        return weight(v) - weight(u);
    }

    bool merge(int u, int v, T w){
        check_vertex(u, "library assertion fault: range violation (merge).");
        check_vertex(v, "library assertion fault: range violation (merge).");
        auto [root_u, weight_u] = leader_weight_unchecked(u);
        auto [root_v, weight_v] = leader_weight_unchecked(v);
        T root_v_to_root_u = w + weight_u - weight_v;
        save(root_u, root_v);
        if(root_u == root_v){
            return root_v_to_root_u == T();
        }
        if(-parent_or_size[static_cast<std::size_t>(root_u)] <
           -parent_or_size[static_cast<std::size_t>(root_v)]){
            std::swap(root_u, root_v);
            root_v_to_root_u = T() - root_v_to_root_u;
        }
        parent_or_size[static_cast<std::size_t>(root_u)] += parent_or_size[static_cast<std::size_t>(root_v)];
        parent_or_size[static_cast<std::size_t>(root_v)] = root_u;
        diff_weight[static_cast<std::size_t>(root_v)] = root_v_to_root_u;
        group_count--;
        return true;
    }

    void undo(){
        if(history_size == 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: history is empty (undo).");
        }
        Change change = history[--history_size];
        parent_or_size[static_cast<std::size_t>(change.u)] = change.parent_u;
        diff_weight[static_cast<std::size_t>(change.u)] = change.diff_u;
        parent_or_size[static_cast<std::size_t>(change.v)] = change.parent_v;
        diff_weight[static_cast<std::size_t>(change.v)] = change.diff_v;
        group_count = change.groups;
    }

    void rollback(int snapshot_id){
        if(snapshot_id < 0 || history_size < snapshot_id)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (rollback).");
        }
        while(history_size > snapshot_id) undo();
    }
};
