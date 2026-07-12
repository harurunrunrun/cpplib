#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>

template<class T, int SIZE, int MAX_CHANGE>
struct PartiallyPersistentStorage{
    static_assert(SIZE > 0);
    static_assert(MAX_CHANGE >= 0);

    static constexpr int log = []{
        int value = MAX_CHANGE;
        int result = 1;
        while(value > 1){ value >>= 1; result++; }
        return result;
    }();

private:
    struct State{
        int change_count = 0;
        std::array<std::optional<T>, SIZE> initial;
        std::array<int, SIZE> head;
        std::array<std::optional<T>, MAX_CHANGE> value;
        std::array<int, MAX_CHANGE> version;
        std::array<int, MAX_CHANGE> index;
        std::array<int, MAX_CHANGE> previous;
        std::array<std::array<int, MAX_CHANGE>, log> jump;

        State(){
            head.fill(-1);
            for(auto& level: jump) level.fill(-1);
        }
    };

    std::unique_ptr<State> state;

public:
    PartiallyPersistentStorage(): state(std::make_unique<State>()){}
    PartiallyPersistentStorage(const PartiallyPersistentStorage&) = delete;
    PartiallyPersistentStorage& operator=(const PartiallyPersistentStorage&) = delete;
    PartiallyPersistentStorage(PartiallyPersistentStorage&&) = delete;
    PartiallyPersistentStorage& operator=(PartiallyPersistentStorage&&) = delete;

    void initialize(int k, const T& x){
        if(k < 0 || SIZE <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (initialize)."
            );
        }
        state->initial[k].emplace(x);
    }

    int changes() const{ return state->change_count; }

    const T& current(int k) const{
        int node = state->head[k];
        return node == -1 ? *state->initial[k] : *state->value[node];
    }

    const T& get(int k, int target_version) const{
        int node = state->head[k];
        if(node == -1 || state->version[node] <= target_version){
            return node == -1 ? *state->initial[k] : *state->value[node];
        }
        for(int level = log - 1; level >= 0; level--){
            int ancestor = state->jump[level][node];
            if(ancestor != -1 && state->version[ancestor] > target_version){
                node = ancestor;
            }
        }
        node = state->previous[node];
        return node == -1 ? *state->initial[k] : *state->value[node];
    }

    void write(int k, int current_version, const T& x){
        if(state->change_count == MAX_CHANGE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: history capacity exceeded (update)."
            );
        }
        int node = state->change_count;
        state->value[node].emplace(x);
        state->version[node] = current_version;
        state->index[node] = k;
        state->previous[node] = state->head[k];
        state->jump[0][node] = state->previous[node];
        for(int level = 1; level < log; level++){
            int ancestor = state->jump[level - 1][node];
            state->jump[level][node] =
                ancestor == -1 ? -1 : state->jump[level - 1][ancestor];
        }
        state->head[k] = node;
        state->change_count++;
    }

    void rollback(int snapshot){
        if(snapshot < 0 || state->change_count < snapshot)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (rollback)."
            );
        }
        while(snapshot < state->change_count){
            --state->change_count;
            int node = state->change_count;
            state->head[state->index[node]] = state->previous[node];
            state->value[node].reset();
        }
    }
};
