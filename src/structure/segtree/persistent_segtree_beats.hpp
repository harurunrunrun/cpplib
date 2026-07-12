#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <vector>
#include "detail/segtree_beats_node.hpp"

template<class T, int MAX_SIZE, int MAX_NODES, int MAX_VERSIONS>
struct PersistentSegmentTreeBeats{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_NODES > 0);
    static_assert(MAX_VERSIONS >= 0);

    static constexpr int sz = []{
        int result = 1;
        while(result < MAX_SIZE) result <<= 1;
        return result;
    }();

private:
    using Value = segtree_beats_detail::Node<T>;
    struct TreeNode{
        Value value;
        int left = -1;
        int right = -1;
    };

    int _n = 0;
    int node_count = 0;
    int version_count = 1;
    std::array<TreeNode, MAX_NODES> nodes;
    std::array<int, static_cast<std::size_t>(MAX_VERSIONS) + 1> roots{};

    int allocate(const TreeNode& value){
        if(node_count == MAX_NODES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: node capacity exceeded (update).");
        }
        nodes[static_cast<std::size_t>(node_count)] = value;
        return node_count++;
    }

    int build(int left, int right, const T* values){
        if(right - left == 1){
            TreeNode result;
            if(left < _n) result.value = segtree_beats_detail::leaf(values == nullptr ? T{} : values[left]);
            return allocate(result);
        }
        int middle = left + (right - left) / 2;
        TreeNode result;
        result.left = build(left, middle, values);
        result.right = build(middle, right, values);
        result.value = segtree_beats_detail::merge(
            nodes[static_cast<std::size_t>(result.left)].value,
            nodes[static_cast<std::size_t>(result.right)].value
        );
        return allocate(result);
    }

    int clone(int source){ return allocate(nodes[static_cast<std::size_t>(source)]); }

    bool child_needs_inherit(const Value& parent, const Value& child) const{
        return child.length != 0 && (parent.add != T{} ||
            parent.max_value < child.max_value || child.min_value < parent.min_value);
    }

    void push(int node){
        TreeNode& current = nodes[static_cast<std::size_t>(node)];
        for(int direction = 0; direction < 2; ++direction){
            int source = direction == 0 ? current.left : current.right;
            if(source == -1 || !child_needs_inherit(current.value, nodes[static_cast<std::size_t>(source)].value)) continue;
            int copied = clone(source);
            segtree_beats_detail::inherit(current.value, nodes[static_cast<std::size_t>(copied)].value);
            if(direction == 0) current.left = copied;
            else current.right = copied;
        }
        current.value.add = T{};
    }

    void pull(int node){
        TreeNode& current = nodes[static_cast<std::size_t>(node)];
        current.value = segtree_beats_detail::merge(
            nodes[static_cast<std::size_t>(current.left)].value,
            nodes[static_cast<std::size_t>(current.right)].value
        );
    }

    int chmin_impl(int source, int left, int right, int ql, int qr, const T& value){
        const Value& original = nodes[static_cast<std::size_t>(source)].value;
        if(right <= ql || qr <= left || original.length == 0 || original.max_value <= value) return source;
        if(ql <= left && right <= qr &&
           (original.max_count == original.length || original.second_max_value < value)){
            int result = clone(source);
            segtree_beats_detail::chmin(nodes[static_cast<std::size_t>(result)].value, value);
            return result;
        }
        int result = clone(source);
        push(result);
        int middle = left + (right - left) / 2;
        TreeNode& current = nodes[static_cast<std::size_t>(result)];
        current.left = chmin_impl(current.left, left, middle, ql, qr, value);
        current.right = chmin_impl(current.right, middle, right, ql, qr, value);
        pull(result);
        return result;
    }

    int chmax_impl(int source, int left, int right, int ql, int qr, const T& value){
        const Value& original = nodes[static_cast<std::size_t>(source)].value;
        if(right <= ql || qr <= left || original.length == 0 || value <= original.min_value) return source;
        if(ql <= left && right <= qr &&
           (original.min_count == original.length || value < original.second_min_value)){
            int result = clone(source);
            segtree_beats_detail::chmax(nodes[static_cast<std::size_t>(result)].value, value);
            return result;
        }
        int result = clone(source);
        push(result);
        int middle = left + (right - left) / 2;
        TreeNode& current = nodes[static_cast<std::size_t>(result)];
        current.left = chmax_impl(current.left, left, middle, ql, qr, value);
        current.right = chmax_impl(current.right, middle, right, ql, qr, value);
        pull(result);
        return result;
    }

    int add_impl(int source, int left, int right, int ql, int qr, const T& value){
        const Value& original = nodes[static_cast<std::size_t>(source)].value;
        if(right <= ql || qr <= left || original.length == 0) return source;
        if(ql <= left && right <= qr){
            int result = clone(source);
            segtree_beats_detail::add(nodes[static_cast<std::size_t>(result)].value, value);
            return result;
        }
        int result = clone(source);
        push(result);
        int middle = left + (right - left) / 2;
        TreeNode& current = nodes[static_cast<std::size_t>(result)];
        current.left = add_impl(current.left, left, middle, ql, qr, value);
        current.right = add_impl(current.right, middle, right, ql, qr, value);
        pull(result);
        return result;
    }

    int set_impl(int source, int left, int right, int position, const T& value){
        int result = clone(source);
        if(right - left == 1){
            nodes[static_cast<std::size_t>(result)].value = segtree_beats_detail::leaf(value);
            return result;
        }
        push(result);
        int middle = left + (right - left) / 2;
        TreeNode& current = nodes[static_cast<std::size_t>(result)];
        if(position < middle) current.left = set_impl(current.left, left, middle, position, value);
        else current.right = set_impl(current.right, middle, right, position, value);
        pull(result);
        return result;
    }

    Value effective(int node, const Value* parent) const{
        Value result = nodes[static_cast<std::size_t>(node)].value;
        if(parent != nullptr) segtree_beats_detail::inherit(*parent, result);
        return result;
    }

    T sum_impl(int node, int left, int right, int ql, int qr, const Value* parent) const{
        if(right <= ql || qr <= left) return T{};
        Value current = effective(node, parent);
        if(current.length == 0) return T{};
        if(ql <= left && right <= qr) return current.sum;
        int middle = left + (right - left) / 2;
        return sum_impl(nodes[static_cast<std::size_t>(node)].left, left, middle, ql, qr, &current) +
            sum_impl(nodes[static_cast<std::size_t>(node)].right, middle, right, ql, qr, &current);
    }

    T min_impl(int node, int left, int right, int ql, int qr, const Value* parent) const{
        Value current = effective(node, parent);
        if(ql <= left && right <= qr) return current.min_value;
        int middle = left + (right - left) / 2;
        if(qr <= middle) return min_impl(nodes[static_cast<std::size_t>(node)].left, left, middle, ql, qr, &current);
        if(middle <= ql) return min_impl(nodes[static_cast<std::size_t>(node)].right, middle, right, ql, qr, &current);
        return std::min(
            min_impl(nodes[static_cast<std::size_t>(node)].left, left, middle, ql, qr, &current),
            min_impl(nodes[static_cast<std::size_t>(node)].right, middle, right, ql, qr, &current)
        );
    }

    T max_impl(int node, int left, int right, int ql, int qr, const Value* parent) const{
        Value current = effective(node, parent);
        if(ql <= left && right <= qr) return current.max_value;
        int middle = left + (right - left) / 2;
        if(qr <= middle) return max_impl(nodes[static_cast<std::size_t>(node)].left, left, middle, ql, qr, &current);
        if(middle <= ql) return max_impl(nodes[static_cast<std::size_t>(node)].right, middle, right, ql, qr, &current);
        return std::max(
            max_impl(nodes[static_cast<std::size_t>(node)].left, left, middle, ql, qr, &current),
            max_impl(nodes[static_cast<std::size_t>(node)].right, middle, right, ql, qr, &current)
        );
    }

    void check_version(int version, const char* message) const{
        if(version < 0 || version_count <= version)[[unlikely]] throw std::runtime_error(message);
    }
    void check_range(int left, int right, const char* message) const{
        if(left < 0 || right < left || _n < right)[[unlikely]] throw std::runtime_error(message);
    }

    template<class Function>
    int update_version(int version, Function&& function){
        check_version(version, "library assertion fault: range violation (update).");
        if(version_count > MAX_VERSIONS)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (update).");
        }
        int snapshot = node_count;
        int root;
        try{ root = function(roots[static_cast<std::size_t>(version)]); }
        catch(...){ node_count = snapshot; throw; }
        int result = version_count++;
        roots[static_cast<std::size_t>(result)] = root;
        return result;
    }

    void initialize(int n, const T* values){
        if(n < 0 || MAX_SIZE < n)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (constructor).");
        _n = n;
        roots[0] = build(0, sz, values);
    }

public:
    explicit PersistentSegmentTreeBeats(int n = MAX_SIZE){ initialize(n, nullptr); }
    explicit PersistentSegmentTreeBeats(const std::vector<T>& values){ initialize(static_cast<int>(values.size()), values.data()); }
    template<std::size_t N>
    explicit PersistentSegmentTreeBeats(const std::array<T, N>& values){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
        initialize(static_cast<int>(N), values.data());
    }

    int size() const{ return _n; }
    int versions() const{ return version_count; }
    int latest_version() const{ return version_count - 1; }
    int nodes_used() const{ return node_count; }

    int fork(int version){ return update_version(version, [](int root){ return root; }); }
    int range_chmin(int version, int left, int right, const T& value){
        check_range(left, right, "library assertion fault: range violation (range_chmin).");
        return update_version(version, [&](int root){ return chmin_impl(root, 0, sz, left, right, value); });
    }
    int range_chmax(int version, int left, int right, const T& value){
        check_range(left, right, "library assertion fault: range violation (range_chmax).");
        return update_version(version, [&](int root){ return chmax_impl(root, 0, sz, left, right, value); });
    }
    int range_add(int version, int left, int right, const T& value){
        check_range(left, right, "library assertion fault: range violation (range_add).");
        return update_version(version, [&](int root){ return add_impl(root, 0, sz, left, right, value); });
    }
    int set(int version, int position, const T& value){
        if(position < 0 || _n <= position)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (set).");
        return update_version(version, [&](int root){ return set_impl(root, 0, sz, position, value); });
    }

    T range_sum(int version, int left, int right) const{
        check_version(version, "library assertion fault: range violation (range_sum).");
        check_range(left, right, "library assertion fault: range violation (range_sum).");
        return sum_impl(roots[static_cast<std::size_t>(version)], 0, sz, left, right, nullptr);
    }
    T range_min(int version, int left, int right) const{
        check_version(version, "library assertion fault: range violation (range_min).");
        check_range(left, right, "library assertion fault: range violation (range_min).");
        if(left == right)[[unlikely]] throw std::runtime_error("library assertion fault: empty range (range_min).");
        return min_impl(roots[static_cast<std::size_t>(version)], 0, sz, left, right, nullptr);
    }
    T range_max(int version, int left, int right) const{
        check_version(version, "library assertion fault: range violation (range_max).");
        check_range(left, right, "library assertion fault: range violation (range_max).");
        if(left == right)[[unlikely]] throw std::runtime_error("library assertion fault: empty range (range_max).");
        return max_impl(roots[static_cast<std::size_t>(version)], 0, sz, left, right, nullptr);
    }
    T get(int version, int position) const{
        if(position < 0 || _n <= position)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (get).");
        return range_sum(version, position, position + 1);
    }
    T all_sum(int version) const{
        check_version(version, "library assertion fault: range violation (all_sum).");
        return nodes[static_cast<std::size_t>(roots[static_cast<std::size_t>(version)])].value.sum;
    }
    T all_min(int version) const{
        check_version(version, "library assertion fault: range violation (all_min).");
        if(_n == 0)[[unlikely]] throw std::runtime_error("library assertion fault: empty range (all_min).");
        return nodes[static_cast<std::size_t>(roots[static_cast<std::size_t>(version)])].value.min_value;
    }
    T all_max(int version) const{
        check_version(version, "library assertion fault: range violation (all_max).");
        if(_n == 0)[[unlikely]] throw std::runtime_error("library assertion fault: empty range (all_max).");
        return nodes[static_cast<std::size_t>(roots[static_cast<std::size_t>(version)])].value.max_value;
    }
};

template<class T, int MAX_SIZE, int MAX_NODES, int MAX_VERSIONS>
using PersistentSegtreeBeats = PersistentSegmentTreeBeats<T, MAX_SIZE, MAX_NODES, MAX_VERSIONS>;
