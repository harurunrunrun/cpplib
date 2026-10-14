#ifndef CPPLIB_SRC_STRUCTURE_PERSISTENCE_PARTIALLY_PERSISTENT_STORAGE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_PERSISTENCE_PARTIALLY_PERSISTENT_STORAGE_HPP_INCLUDED

#include <limits>
#include <array>
#include "../array/static_or_vector.hpp"
#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

template<class T, int SIZE = -1, int MAX_CHANGE = -1>
struct PartiallyPersistentStorage{
    static_assert(SIZE == -1 || SIZE > 0);
    static_assert(MAX_CHANGE >= -1);

private:
    struct State{
        int change_count = 0;
        cpplib_detail::StaticOrVector<std::optional<T>, SIZE> initial;
        cpplib_detail::StaticOrVector<std::vector<int>, SIZE> history;
        cpplib_detail::StaticOrVector<std::optional<T>, MAX_CHANGE> value;
        cpplib_detail::StaticOrVector<int, MAX_CHANGE> version;
        cpplib_detail::StaticOrVector<int, MAX_CHANGE> index;
    };

    std::unique_ptr<State> state;

public:
    explicit PartiallyPersistentStorage(int n = (SIZE < 0 ? 0 : SIZE)): state(std::make_unique<State>()){
        if(n < 0 || (SIZE >= 0 && n > SIZE)) throw std::out_of_range("invalid storage size");
        state->initial.resize(static_cast<std::size_t>(n));
        state->history.resize(static_cast<std::size_t>(n));
    }
    PartiallyPersistentStorage(const PartiallyPersistentStorage&) = delete;
    PartiallyPersistentStorage& operator=(const PartiallyPersistentStorage&) = delete;
    PartiallyPersistentStorage(PartiallyPersistentStorage&&) = delete;
    PartiallyPersistentStorage& operator=(PartiallyPersistentStorage&&) = delete;

    void initialize(int k, const T& x){
        if(k < 0 || (SIZE >= 0 && SIZE <= k))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (initialize)."
            );
        }
        if constexpr(SIZE < 0){
            T value(x);
            state->initial.ensure(static_cast<std::size_t>(k) + 1);
            state->history.ensure(static_cast<std::size_t>(k) + 1);
            state->initial[k].emplace(std::move(value));
        }else state->initial[k].emplace(x);
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
        if(state->change_count == std::numeric_limits<int>::max() || state->change_count == MAX_CHANGE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: history capacity exceeded (update)."
            );
        }
        const int change = state->change_count;
        std::optional<T> copied;
        if constexpr(MAX_CHANGE < 0) copied.emplace(x);
        state->value.ensure(static_cast<std::size_t>(change) + 1);
        state->version.ensure(static_cast<std::size_t>(change) + 1);
        state->index.ensure(static_cast<std::size_t>(change) + 1);
        state->value[change].emplace(copied ? *copied : x);
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

#endif  // CPPLIB_SRC_STRUCTURE_PERSISTENCE_PARTIALLY_PERSISTENT_STORAGE_HPP_INCLUDED
