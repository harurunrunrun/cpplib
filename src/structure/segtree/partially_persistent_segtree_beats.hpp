#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_SEGTREE_BEATS_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_SEGTREE_BEATS_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../other/partially_persistent_storage.hpp"
#include "detail/segtree_beats_node.hpp"

template<class T, int MAX_SIZE, int MAX_VERSIONS, int MAX_CHANGES>
struct PartiallyPersistentSegmentTreeBeats{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_VERSIONS >= 0);
    static_assert(MAX_CHANGES >= 0);

    static constexpr int sz = []{
        int result = 1;
        while(result < MAX_SIZE) result <<= 1;
        return result;
    }();

private:
    using Value = segtree_beats_detail::Node<T>;
    int _n = 0;
    int version_count = 1;
    PartiallyPersistentStorage<Value, 2 * sz, MAX_CHANGES> data;

    void write(int index, const Value& value){ data.write(index, version_count, value); }

    bool needs_inherit(const Value& parent, const Value& child) const{
        return child.length != 0 && (parent.add != T{} ||
            parent.max_value < child.max_value || child.min_value < parent.min_value);
    }

    void push(int index){
        Value parent = data.current(index);
        for(int child: {2 * index, 2 * index + 1}){
            Value value = data.current(child);
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
        write(index, segtree_beats_detail::merge(data.current(2 * index), data.current(2 * index + 1)));
    }

    void chmin_impl(int index, int left, int right, int ql, int qr, const T& value){
        Value current = data.current(index);
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
        Value current = data.current(index);
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
        Value current = data.current(index);
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

    Value effective(int version, int index, const Value* parent) const{
        Value result = data.get(index, version);
        if(parent != nullptr) segtree_beats_detail::inherit(*parent, result);
        return result;
    }

    T sum_impl(int version, int index, int left, int right, int ql, int qr, const Value* parent) const{
        if(right <= ql || qr <= left) return T{};
        Value current = effective(version, index, parent);
        if(current.length == 0) return T{};
        if(ql <= left && right <= qr) return current.sum;
        int middle = left + (right - left) / 2;
        return sum_impl(version, 2 * index, left, middle, ql, qr, &current) +
            sum_impl(version, 2 * index + 1, middle, right, ql, qr, &current);
    }

    T min_impl(int version, int index, int left, int right, int ql, int qr, const Value* parent) const{
        Value current = effective(version, index, parent);
        if(ql <= left && right <= qr) return current.min_value;
        int middle = left + (right - left) / 2;
        if(qr <= middle) return min_impl(version, 2 * index, left, middle, ql, qr, &current);
        if(middle <= ql) return min_impl(version, 2 * index + 1, middle, right, ql, qr, &current);
        return std::min(
            min_impl(version, 2 * index, left, middle, ql, qr, &current),
            min_impl(version, 2 * index + 1, middle, right, ql, qr, &current)
        );
    }

    T max_impl(int version, int index, int left, int right, int ql, int qr, const Value* parent) const{
        Value current = effective(version, index, parent);
        if(ql <= left && right <= qr) return current.max_value;
        int middle = left + (right - left) / 2;
        if(qr <= middle) return max_impl(version, 2 * index, left, middle, ql, qr, &current);
        if(middle <= ql) return max_impl(version, 2 * index + 1, middle, right, ql, qr, &current);
        return std::max(
            max_impl(version, 2 * index, left, middle, ql, qr, &current),
            max_impl(version, 2 * index + 1, middle, right, ql, qr, &current)
        );
    }

    void check_version(int version, const char* message) const{
        if(version < 0 || version_count <= version)[[unlikely]] throw std::runtime_error(message);
    }
    void check_range(int left, int right, const char* message) const{
        if(left < 0 || right < left || _n < right)[[unlikely]] throw std::runtime_error(message);
    }

    template<class Function>
    int update(Function&& function){
        if(version_count > MAX_VERSIONS)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (update).");
        }
        int snapshot = data.changes();
        try{ std::forward<Function>(function)(); }
        catch(...){ data.rollback(snapshot); throw; }
        return version_count++;
    }

    void initialize(int n, const T* values){
        if(n < 0 || MAX_SIZE < n)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (constructor).");
        _n = n;
        auto initial = std::make_unique<std::array<Value, 2 * sz>>();
        for(int position = 0; position < _n; ++position){
            (*initial)[static_cast<std::size_t>(position + sz)] = segtree_beats_detail::leaf(values == nullptr ? T{} : values[position]);
        }
        for(int index = sz - 1; index > 0; --index){
            (*initial)[static_cast<std::size_t>(index)] = segtree_beats_detail::merge(
                (*initial)[static_cast<std::size_t>(2 * index)],
                (*initial)[static_cast<std::size_t>(2 * index + 1)]
            );
        }
        for(int index = 0; index < 2 * sz; ++index) data.initialize(index, (*initial)[static_cast<std::size_t>(index)]);
    }

public:
    explicit PartiallyPersistentSegmentTreeBeats(int n = MAX_SIZE){ initialize(n, nullptr); }
    explicit PartiallyPersistentSegmentTreeBeats(const std::vector<T>& values){ initialize(static_cast<int>(values.size()), values.data()); }
    template<std::size_t N>
    explicit PartiallyPersistentSegmentTreeBeats(const std::array<T, N>& values){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
        initialize(static_cast<int>(N), values.data());
    }

    int size() const{ return _n; }
    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }
    int changes_used() const{ return data.changes(); }

    int range_chmin(int left, int right, const T& value){
        check_range(left, right, "library assertion fault: range violation (range_chmin).");
        return update([&]{ chmin_impl(1, 0, sz, left, right, value); });
    }
    int range_chmax(int left, int right, const T& value){
        check_range(left, right, "library assertion fault: range violation (range_chmax).");
        return update([&]{ chmax_impl(1, 0, sz, left, right, value); });
    }
    int range_add(int left, int right, const T& value){
        check_range(left, right, "library assertion fault: range violation (range_add).");
        return update([&]{ add_impl(1, 0, sz, left, right, value); });
    }
    int set(int position, const T& value){
        if(position < 0 || _n <= position)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (set).");
        return update([&]{ set_impl(1, 0, sz, position, value); });
    }

    T range_sum(int version, int left, int right) const{
        check_version(version, "library assertion fault: range violation (range_sum).");
        check_range(left, right, "library assertion fault: range violation (range_sum).");
        return sum_impl(version, 1, 0, sz, left, right, nullptr);
    }
    T range_min(int version, int left, int right) const{
        check_version(version, "library assertion fault: range violation (range_min).");
        check_range(left, right, "library assertion fault: range violation (range_min).");
        if(left == right)[[unlikely]] throw std::runtime_error("library assertion fault: empty range (range_min).");
        return min_impl(version, 1, 0, sz, left, right, nullptr);
    }
    T range_max(int version, int left, int right) const{
        check_version(version, "library assertion fault: range violation (range_max).");
        check_range(left, right, "library assertion fault: range violation (range_max).");
        if(left == right)[[unlikely]] throw std::runtime_error("library assertion fault: empty range (range_max).");
        return max_impl(version, 1, 0, sz, left, right, nullptr);
    }
    T get(int version, int position) const{
        if(position < 0 || _n <= position)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (get).");
        return range_sum(version, position, position + 1);
    }
    T all_sum(int version) const{
        check_version(version, "library assertion fault: range violation (all_sum).");
        return data.get(1, version).sum;
    }
    T all_min(int version) const{
        check_version(version, "library assertion fault: range violation (all_min).");
        if(_n == 0)[[unlikely]] throw std::runtime_error("library assertion fault: empty range (all_min).");
        return data.get(1, version).min_value;
    }
    T all_max(int version) const{
        check_version(version, "library assertion fault: range violation (all_max).");
        if(_n == 0)[[unlikely]] throw std::runtime_error("library assertion fault: empty range (all_max).");
        return data.get(1, version).max_value;
    }

    T range_sum(int left, int right) const{ return range_sum(latest_version(), left, right); }
    T range_min(int left, int right) const{ return range_min(latest_version(), left, right); }
    T range_max(int left, int right) const{ return range_max(latest_version(), left, right); }
    T get(int position) const{ return get(latest_version(), position); }
    T all_sum() const{ return all_sum(latest_version()); }
    T all_min() const{ return all_min(latest_version()); }
    T all_max() const{ return all_max(latest_version()); }
};

template<class T, int MAX_SIZE, int MAX_VERSIONS, int MAX_CHANGES>
using PartiallyPersistentSegtreeBeats = PartiallyPersistentSegmentTreeBeats<T, MAX_SIZE, MAX_VERSIONS, MAX_CHANGES>;

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PARTIALLY_PERSISTENT_SEGTREE_BEATS_HPP_INCLUDED
