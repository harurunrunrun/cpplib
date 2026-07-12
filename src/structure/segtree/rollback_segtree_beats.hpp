#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>
#include "detail/segtree_beats_node.hpp"

template<class T, int MAX_SIZE, int MAX_CHANGES, int MAX_OPERATIONS = MAX_CHANGES>
struct RollbackSegmentTreeBeats{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_CHANGES >= 0);
    static_assert(MAX_OPERATIONS >= 0);
    using Snapshot = int;

    static constexpr int sz = []{
        int result = 1;
        while(result < MAX_SIZE) result <<= 1;
        return result;
    }();

private:
    using Value = segtree_beats_detail::Node<T>;
    struct Change{ int index = 0; Value value; };

    int _n = 0;
    int change_count = 0;
    int operation_count = 0;
    std::array<Value, 2 * sz> data{};
    std::array<Change, MAX_CHANGES> history{};
    std::array<int, MAX_OPERATIONS> operation{};

    void write(int index, const Value& value){
        if(change_count == MAX_CHANGES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: history capacity exceeded (update).");
        }
        history[static_cast<std::size_t>(change_count)] = {index, data[static_cast<std::size_t>(index)]};
        ++change_count;
        data[static_cast<std::size_t>(index)] = value;
    }

    void restore_changes(int snapshot){
        while(snapshot < change_count){
            --change_count;
            const Change& change = history[static_cast<std::size_t>(change_count)];
            data[static_cast<std::size_t>(change.index)] = change.value;
        }
    }

    bool needs_inherit(const Value& parent, const Value& child) const{
        return child.length != 0 && (parent.add != T{} ||
            parent.max_value < child.max_value || child.min_value < parent.min_value);
    }

    void push(int index){
        Value parent = data[static_cast<std::size_t>(index)];
        for(int child: {2 * index, 2 * index + 1}){
            Value value = data[static_cast<std::size_t>(child)];
            if(needs_inherit(parent, value)){
                segtree_beats_detail::inherit(parent, value);
                write(child, value);
            }
        }
        if(parent.add != T{}){
            parent.add = T{};
            write(index, parent);
        }
    }

    void pull(int index){
        write(index, segtree_beats_detail::merge(
            data[static_cast<std::size_t>(2 * index)],
            data[static_cast<std::size_t>(2 * index + 1)]
        ));
    }

    void chmin_impl(int index, int left, int right, int ql, int qr, const T& value){
        Value current = data[static_cast<std::size_t>(index)];
        if(right <= ql || qr <= left || current.length == 0 || current.max_value <= value) return;
        if(ql <= left && right <= qr &&
           (current.max_count == current.length || current.second_max_value < value)){
            segtree_beats_detail::chmin(current, value);
            write(index, current);
            return;
        }
        push(index);
        int middle = left + (right - left) / 2;
        chmin_impl(2 * index, left, middle, ql, qr, value);
        chmin_impl(2 * index + 1, middle, right, ql, qr, value);
        pull(index);
    }

    void chmax_impl(int index, int left, int right, int ql, int qr, const T& value){
        Value current = data[static_cast<std::size_t>(index)];
        if(right <= ql || qr <= left || current.length == 0 || value <= current.min_value) return;
        if(ql <= left && right <= qr &&
           (current.min_count == current.length || value < current.second_min_value)){
            segtree_beats_detail::chmax(current, value);
            write(index, current);
            return;
        }
        push(index);
        int middle = left + (right - left) / 2;
        chmax_impl(2 * index, left, middle, ql, qr, value);
        chmax_impl(2 * index + 1, middle, right, ql, qr, value);
        pull(index);
    }

    void add_impl(int index, int left, int right, int ql, int qr, const T& value){
        Value current = data[static_cast<std::size_t>(index)];
        if(right <= ql || qr <= left || current.length == 0) return;
        if(ql <= left && right <= qr){
            segtree_beats_detail::add(current, value);
            write(index, current);
            return;
        }
        push(index);
        int middle = left + (right - left) / 2;
        add_impl(2 * index, left, middle, ql, qr, value);
        add_impl(2 * index + 1, middle, right, ql, qr, value);
        pull(index);
    }

    void set_impl(int index, int left, int right, int position, const T& value){
        if(right - left == 1){ write(index, segtree_beats_detail::leaf(value)); return; }
        push(index);
        int middle = left + (right - left) / 2;
        if(position < middle) set_impl(2 * index, left, middle, position, value);
        else set_impl(2 * index + 1, middle, right, position, value);
        pull(index);
    }

    Value effective(int index, const Value* parent) const{
        Value result = data[static_cast<std::size_t>(index)];
        if(parent != nullptr) segtree_beats_detail::inherit(*parent, result);
        return result;
    }

    T sum_impl(int index, int left, int right, int ql, int qr, const Value* parent) const{
        if(right <= ql || qr <= left) return T{};
        Value current = effective(index, parent);
        if(current.length == 0) return T{};
        if(ql <= left && right <= qr) return current.sum;
        int middle = left + (right - left) / 2;
        return sum_impl(2 * index, left, middle, ql, qr, &current) +
            sum_impl(2 * index + 1, middle, right, ql, qr, &current);
    }

    T min_impl(int index, int left, int right, int ql, int qr, const Value* parent) const{
        Value current = effective(index, parent);
        if(ql <= left && right <= qr) return current.min_value;
        int middle = left + (right - left) / 2;
        if(qr <= middle) return min_impl(2 * index, left, middle, ql, qr, &current);
        if(middle <= ql) return min_impl(2 * index + 1, middle, right, ql, qr, &current);
        return std::min(
            min_impl(2 * index, left, middle, ql, qr, &current),
            min_impl(2 * index + 1, middle, right, ql, qr, &current)
        );
    }

    T max_impl(int index, int left, int right, int ql, int qr, const Value* parent) const{
        Value current = effective(index, parent);
        if(ql <= left && right <= qr) return current.max_value;
        int middle = left + (right - left) / 2;
        if(qr <= middle) return max_impl(2 * index, left, middle, ql, qr, &current);
        if(middle <= ql) return max_impl(2 * index + 1, middle, right, ql, qr, &current);
        return std::max(
            max_impl(2 * index, left, middle, ql, qr, &current),
            max_impl(2 * index + 1, middle, right, ql, qr, &current)
        );
    }

    void check_range(int left, int right, const char* message) const{
        if(left < 0 || right < left || _n < right)[[unlikely]] throw std::runtime_error(message);
    }

    template<class Function>
    void update(Function&& function){
        if(operation_count == MAX_OPERATIONS)[[unlikely]]{
            throw std::runtime_error("library assertion fault: operation capacity exceeded (update).");
        }
        int snapshot = change_count;
        try{ std::forward<Function>(function)(); }
        catch(...){ restore_changes(snapshot); throw; }
        operation[static_cast<std::size_t>(operation_count++)] = snapshot;
    }

    void initialize(int n, const T* values){
        if(n < 0 || MAX_SIZE < n)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (constructor).");
        _n = n;
        for(int position = 0; position < _n; ++position){
            data[static_cast<std::size_t>(position + sz)] = segtree_beats_detail::leaf(values == nullptr ? T{} : values[position]);
        }
        for(int index = sz - 1; index > 0; --index){
            data[static_cast<std::size_t>(index)] = segtree_beats_detail::merge(
                data[static_cast<std::size_t>(2 * index)],
                data[static_cast<std::size_t>(2 * index + 1)]
            );
        }
    }

public:
    explicit RollbackSegmentTreeBeats(int n = MAX_SIZE){ initialize(n, nullptr); }
    explicit RollbackSegmentTreeBeats(const std::vector<T>& values){ initialize(static_cast<int>(values.size()), values.data()); }
    template<std::size_t N>
    explicit RollbackSegmentTreeBeats(const std::array<T, N>& values){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
        initialize(static_cast<int>(N), values.data());
    }

    int size() const{ return _n; }
    int operations() const{ return operation_count; }
    int changes_used() const{ return change_count; }
    Snapshot snapshot() const{ return operation_count; }

    void rollback(Snapshot target){
        if(target < 0 || operation_count < target)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (rollback).");
        }
        while(target < operation_count){
            --operation_count;
            restore_changes(operation[static_cast<std::size_t>(operation_count)]);
        }
    }

    void range_chmin(int left, int right, const T& value){
        check_range(left, right, "library assertion fault: range violation (range_chmin).");
        update([&]{ chmin_impl(1, 0, sz, left, right, value); });
    }
    void range_chmax(int left, int right, const T& value){
        check_range(left, right, "library assertion fault: range violation (range_chmax).");
        update([&]{ chmax_impl(1, 0, sz, left, right, value); });
    }
    void range_add(int left, int right, const T& value){
        check_range(left, right, "library assertion fault: range violation (range_add).");
        update([&]{ add_impl(1, 0, sz, left, right, value); });
    }
    void set(int position, const T& value){
        if(position < 0 || _n <= position)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (set).");
        update([&]{ set_impl(1, 0, sz, position, value); });
    }

    T range_sum(int left, int right) const{
        check_range(left, right, "library assertion fault: range violation (range_sum).");
        return sum_impl(1, 0, sz, left, right, nullptr);
    }
    T range_min(int left, int right) const{
        check_range(left, right, "library assertion fault: range violation (range_min).");
        if(left == right)[[unlikely]] throw std::runtime_error("library assertion fault: empty range (range_min).");
        return min_impl(1, 0, sz, left, right, nullptr);
    }
    T range_max(int left, int right) const{
        check_range(left, right, "library assertion fault: range violation (range_max).");
        if(left == right)[[unlikely]] throw std::runtime_error("library assertion fault: empty range (range_max).");
        return max_impl(1, 0, sz, left, right, nullptr);
    }
    T get(int position) const{
        if(position < 0 || _n <= position)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (get).");
        return range_sum(position, position + 1);
    }
    T all_sum() const{ return data[1].sum; }
    T all_min() const{
        if(_n == 0)[[unlikely]] throw std::runtime_error("library assertion fault: empty range (all_min).");
        return data[1].min_value;
    }
    T all_max() const{
        if(_n == 0)[[unlikely]] throw std::runtime_error("library assertion fault: empty range (all_max).");
        return data[1].max_value;
    }
};

template<class T, int MAX_SIZE, int MAX_CHANGES, int MAX_OPERATIONS = MAX_CHANGES>
using RollbackSegtreeBeats = RollbackSegmentTreeBeats<T, MAX_SIZE, MAX_CHANGES, MAX_OPERATIONS>;
