#ifndef CPPLIB_SRC_STRUCTURE_OTHER_PARTIALLY_PERSISTENT_STORAGE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_PARTIALLY_PERSISTENT_STORAGE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

template<class T, int SIZE, int MAX_CHANGE>
struct PartiallyPersistentStorage{
    static_assert(SIZE > 0);
    static_assert(MAX_CHANGE >= 0);

private:
    struct State{
        int change_count = 0;
        std::array<std::optional<T>, SIZE> initial;
        std::array<std::vector<int>, SIZE> history;
        std::array<std::optional<T>, MAX_CHANGE> value;
        std::array<int, MAX_CHANGE> version;
        std::array<int, MAX_CHANGE> index;
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
        const auto& history = state->history[k];
        return history.empty() ?
            *state->initial[k] : *state->value[history.back()];
    }

    const T& get(int k, int target_version) const{
        const auto& history = state->history[k];
        if(history.empty()) return *state->initial[k];
        const int newest = history.back();
        if(state->version[newest] <= target_version){
            return *state->value[newest];
        }
        std::size_t left = 0;
        std::size_t right = history.size();
        while(left < right){
            const std::size_t middle = left + (right - left) / 2;
            const int change = history[middle];
            if(state->version[change] <= target_version) left = middle + 1;
            else right = middle;
        }
        return left == 0 ? *state->initial[k] : *state->value[history[left - 1]];
    }

    void write(int k, int current_version, const T& x){
        if(state->change_count == MAX_CHANGE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: history capacity exceeded (update)."
            );
        }
        const int change = state->change_count;
        state->value[change].emplace(x);
        state->version[change] = current_version;
        state->index[change] = k;
        try{
            state->history[k].push_back(change);
        }catch(...){
            state->value[change].reset();
            throw;
        }
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
            const int change = state->change_count;
            state->history[state->index[change]].pop_back();
            state->value[change].reset();
        }
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_PARTIALLY_PERSISTENT_STORAGE_HPP_INCLUDED
