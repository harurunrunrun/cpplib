#ifndef CPPLIB_SRC_STRUCTURE_ARRAY_ROLLBACK_ARRAY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_ARRAY_ROLLBACK_ARRAY_HPP_INCLUDED

#include <limits>
#include <array>
#include "static_or_vector.hpp"
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <vector>

template<class T, int MAX_SIZE = -1, int MAX_UPDATE = -1>
struct RollbackArray{
    static_assert(MAX_SIZE == -1 || MAX_SIZE > 0);
    static_assert(MAX_UPDATE >= -1);

    using Snapshot = int;

private:
    struct History{
        int index;
        int old_value_ref;
    };

    int _n;
    int _history_size = 0;

    cpplib_detail::StaticOrVector<std::optional<T>, MAX_SIZE> initial;
    cpplib_detail::StaticOrVector<std::optional<T>, MAX_UPDATE> updated_values;
    cpplib_detail::StaticOrVector<int, MAX_SIZE> current_value_ref;
    cpplib_detail::StaticOrVector<History, MAX_UPDATE> history;

    static int initial_value_ref(int k){
        return -k - 1;
    }

    void initialize_ref(int k){
        current_value_ref[k] = initial_value_ref(k);
    }

    void undo_one(){
        --_history_size;
        const History& entry = history[_history_size];
        current_value_ref[entry.index] = entry.old_value_ref;
        updated_values[_history_size].reset();
    }

public:
    RollbackArray(const RollbackArray&) = delete;
    RollbackArray& operator=(const RollbackArray&) = delete;
    RollbackArray(RollbackArray&&) = delete;
    RollbackArray& operator=(RollbackArray&&) = delete;

    explicit RollbackArray(int n = (MAX_SIZE < 0 ? 0 : MAX_SIZE)): _n(n){
        if(_n < 0 || (MAX_SIZE >= 0 && MAX_SIZE < _n))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        initial.resize(static_cast<std::size_t>(_n));
        current_value_ref.resize(static_cast<std::size_t>(_n));
        for(int k = 0; k < _n; k++){
            initial[k].emplace();
            initialize_ref(k);
        }
    }

    RollbackArray(int n, const T& value): _n(n){
        if(_n < 0 || (MAX_SIZE >= 0 && MAX_SIZE < _n))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        initial.resize(static_cast<std::size_t>(_n));
        current_value_ref.resize(static_cast<std::size_t>(_n));
        for(int k = 0; k < _n; k++){
            initial[k].emplace(value);
            initialize_ref(k);
        }
    }

    explicit RollbackArray(const std::vector<T>& v): _n(cpplib_detail::checked_int_size(v.size())){
        if(MAX_SIZE >= 0 && v.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        initial.resize(static_cast<std::size_t>(_n));
        current_value_ref.resize(static_cast<std::size_t>(_n));
        for(int k = 0; k < _n; k++){
            initial[k].emplace(v[static_cast<std::size_t>(k)]);
            initialize_ref(k);
        }
    }

    template<std::size_t N>
    explicit RollbackArray(const std::array<T, N>& v): _n(cpplib_detail::checked_int_size(N)){
        if(MAX_SIZE >= 0 && N > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        initial.resize(static_cast<std::size_t>(_n));
        current_value_ref.resize(static_cast<std::size_t>(_n));
        for(int k = 0; k < _n; k++){
            initial[k].emplace(v[static_cast<std::size_t>(k)]);
            initialize_ref(k);
        }
    }

    int size() const{
        return _n;
    }

    int history_size() const{
        return _history_size;
    }

    bool can_undo() const{
        return _history_size != 0;
    }

    void set(int k, const T& value){
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (set)."
            );
        }
        if(_history_size == std::numeric_limits<int>::max() || _history_size == MAX_UPDATE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation (set)."
            );
        }

        std::optional<T> copied;
        if constexpr(MAX_UPDATE < 0) copied.emplace(value);
        updated_values.ensure(static_cast<std::size_t>(_history_size) + 1);
        history.ensure(static_cast<std::size_t>(_history_size) + 1);
        updated_values[_history_size].emplace(copied ? *copied : value);
        history[_history_size] = {k, current_value_ref[k]};
        current_value_ref[k] = _history_size;
        _history_size++;
    }

    const T& get(int k) const{
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (get)."
            );
        }

        int value_ref = current_value_ref[k];
        if(value_ref < 0){
            return *initial[-value_ref - 1];
        }
        return *updated_values[value_ref];
    }

    void undo(){
        if(!can_undo())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: undo history is empty (undo)."
            );
        }
        undo_one();
    }

    Snapshot snapshot() const{
        return _history_size;
    }

    void rollback(Snapshot snapshot){
        if(snapshot < 0 || _history_size < snapshot)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (rollback)."
            );
        }
        while(snapshot < _history_size){
            undo_one();
        }
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_ARRAY_ROLLBACK_ARRAY_HPP_INCLUDED
