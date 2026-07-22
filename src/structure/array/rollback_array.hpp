#ifndef CPPLIB_SRC_STRUCTURE_ARRAY_ROLLBACK_ARRAY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_ARRAY_ROLLBACK_ARRAY_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <vector>

template<class T, int MAX_SIZE, int MAX_UPDATE>
struct RollbackArray{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_UPDATE >= 0);

    using Snapshot = int;

private:
    struct History{
        int index;
        int old_value_ref;
    };

    int _n;
    int _history_size = 0;

    std::array<std::optional<T>, MAX_SIZE> initial;
    std::array<std::optional<T>, MAX_UPDATE> updated_values;
    std::array<int, MAX_SIZE> current_value_ref;
    std::array<History, MAX_UPDATE> history;

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

    explicit RollbackArray(int n = MAX_SIZE): _n(n){
        if(_n < 0 || MAX_SIZE < _n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        for(int k = 0; k < _n; k++){
            initial[k].emplace();
            initialize_ref(k);
        }
    }

    RollbackArray(int n, const T& value): _n(n){
        if(_n < 0 || MAX_SIZE < _n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        for(int k = 0; k < _n; k++){
            initial[k].emplace(value);
            initialize_ref(k);
        }
    }

    explicit RollbackArray(const std::vector<T>& v): _n((int)v.size()){
        if(v.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        for(int k = 0; k < _n; k++){
            initial[k].emplace(v[static_cast<std::size_t>(k)]);
            initialize_ref(k);
        }
    }

    template<std::size_t N>
    explicit RollbackArray(const std::array<T, N>& v): _n((int)N){
        if(N > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
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
        if(_history_size == MAX_UPDATE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation (set)."
            );
        }

        updated_values[_history_size].emplace(value);
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
