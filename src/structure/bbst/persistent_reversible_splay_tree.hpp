#pragma once

#include <array>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<auto Monoid, int MAX_NODE, int MAX_VERSION>
struct PersistentReversibleSplayTree{
    static_assert(MAX_NODE > 0);
    static_assert(MAX_VERSION > 0);
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

private:
    struct Node{
        int left = -1;
        int right = -1;
        int size = 0;
        bool reversed = false;
        std::uint64_t priority = 0;
        S value{};
        S aggregate{};
        S rev_aggregate{};
    };

    std::array<Node, MAX_NODE> nodes{};
    std::array<int, MAX_VERSION> roots{};
    int used = 0;
    int version_count = 1;

    static std::uint64_t splitmix64(std::uint64_t x){
        x += 0x9e3779b97f4a7c15ULL;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    int size_or_zero(int node) const{
        return node == -1 ? 0 : nodes[static_cast<std::size_t>(node)].size;
    }

    S aggregate_or_e(int node) const{
        return node == -1 ? Monoid.e() : nodes[static_cast<std::size_t>(node)].aggregate;
    }

    S rev_aggregate_or_e(int node) const{
        return node == -1 ? Monoid.e() : nodes[static_cast<std::size_t>(node)].rev_aggregate;
    }

    void pull(int node){
        auto& current = nodes[static_cast<std::size_t>(node)];
        current.size = 1 + size_or_zero(current.left) + size_or_zero(current.right);
        current.aggregate = Monoid.op(
            aggregate_or_e(current.left),
            Monoid.op(current.value, aggregate_or_e(current.right))
        );
        current.rev_aggregate = Monoid.op(
            rev_aggregate_or_e(current.right),
            Monoid.op(current.value, rev_aggregate_or_e(current.left))
        );
    }

    int copy_node(int node){
        if(node == -1) return -1;
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentReversibleSplayTree).");
        }
        nodes[static_cast<std::size_t>(used)] = nodes[static_cast<std::size_t>(node)];
        return used++;
    }

    int new_node(const S& value){
        if(used == MAX_NODE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (PersistentReversibleSplayTree).");
        }
        int node = used++;
        nodes[static_cast<std::size_t>(node)] =
            {-1, -1, 1, false, splitmix64(static_cast<std::uint64_t>(node) + 1), value, value, value};
        return node;
    }

    void toggle(int node){
        if(node == -1) return;
        auto& current = nodes[static_cast<std::size_t>(node)];
        std::swap(current.left, current.right);
        std::swap(current.aggregate, current.rev_aggregate);
        current.reversed = !current.reversed;
    }

    void push(int node){
        if(node == -1 || !nodes[static_cast<std::size_t>(node)].reversed) return;
        auto& current = nodes[static_cast<std::size_t>(node)];
        if(current.left != -1){
            current.left = copy_node(current.left);
            toggle(current.left);
        }
        if(current.right != -1){
            current.right = copy_node(current.right);
            toggle(current.right);
        }
        current.reversed = false;
    }

    std::pair<int, int> split(int root, int k){
        if(root == -1) return {-1, -1};
        int node = copy_node(root);
        push(node);
        int left_size = size_or_zero(nodes[static_cast<std::size_t>(node)].left);
        if(k <= left_size){
            auto [left, right] = split(nodes[static_cast<std::size_t>(node)].left, k);
            nodes[static_cast<std::size_t>(node)].left = right;
            pull(node);
            return {left, node};
        }else{
            auto [left, right] = split(nodes[static_cast<std::size_t>(node)].right, k - left_size - 1);
            nodes[static_cast<std::size_t>(node)].right = left;
            pull(node);
            return {node, right};
        }
    }

    int merge(int left, int right){
        if(left == -1) return right;
        if(right == -1) return left;
        if(nodes[static_cast<std::size_t>(left)].priority < nodes[static_cast<std::size_t>(right)].priority){
            int node = copy_node(left);
            push(node);
            nodes[static_cast<std::size_t>(node)].right = merge(nodes[static_cast<std::size_t>(node)].right, right);
            pull(node);
            return node;
        }else{
            int node = copy_node(right);
            push(node);
            nodes[static_cast<std::size_t>(node)].left = merge(left, nodes[static_cast<std::size_t>(node)].left);
            pull(node);
            return node;
        }
    }

    int build(const std::vector<S>& values, int l, int r){
        if(l == r) return -1;
        int m = (l + r) >> 1;
        int node = new_node(values[static_cast<std::size_t>(m)]);
        nodes[static_cast<std::size_t>(node)].left = build(values, l, m);
        nodes[static_cast<std::size_t>(node)].right = build(values, m + 1, r);
        pull(node);
        return node;
    }

    int register_root(int root){
        if(version_count == MAX_VERSION)[[unlikely]]{
            throw std::runtime_error("library assertion fault: version capacity exceeded (PersistentReversibleSplayTree).");
        }
        roots[static_cast<std::size_t>(version_count)] = root;
        return version_count++;
    }

    void check_version(int version) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PersistentReversibleSplayTree).");
        }
    }

    void check_insert_position(int version, int p) const{
        check_version(version);
        if(p < 0 || size(version) < p)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PersistentReversibleSplayTree).");
        }
    }

    void check_position(int version, int p) const{
        check_version(version);
        if(p < 0 || size(version) <= p)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PersistentReversibleSplayTree).");
        }
    }

    void check_range(int version, int l, int r) const{
        check_version(version);
        if(l < 0 || r < l || size(version) < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (PersistentReversibleSplayTree).");
        }
    }

    S get_rec(int node, int k, bool flip) const{
        const auto& current = nodes[static_cast<std::size_t>(node)];
        int left = flip ? current.right : current.left;
        int right = flip ? current.left : current.right;
        bool child_flip = flip ^ current.reversed;
        int left_size = size_or_zero(left);
        if(k < left_size) return get_rec(left, k, child_flip);
        if(k == left_size) return current.value;
        return get_rec(right, k - left_size - 1, child_flip);
    }

    S prod_rec(int node, int nl, int nr, int ql, int qr, bool flip) const{
        if(node == -1 || qr <= nl || nr <= ql) return Monoid.e();
        if(ql <= nl && nr <= qr){
            return flip ? nodes[static_cast<std::size_t>(node)].rev_aggregate
                        : nodes[static_cast<std::size_t>(node)].aggregate;
        }
        const auto& current = nodes[static_cast<std::size_t>(node)];
        int left = flip ? current.right : current.left;
        int right = flip ? current.left : current.right;
        bool child_flip = flip ^ current.reversed;
        int left_size = size_or_zero(left);
        int mid = nl + left_size;
        S result = prod_rec(left, nl, mid, ql, qr, child_flip);
        if(ql <= mid && mid < qr) result = Monoid.op(result, current.value);
        return Monoid.op(result, prod_rec(right, mid + 1, nr, ql, qr, child_flip));
    }

public:
    PersistentReversibleSplayTree(){
        roots.fill(-1);
    }

    explicit PersistentReversibleSplayTree(const std::vector<S>& values) : PersistentReversibleSplayTree(){
        roots[0] = build(values, 0, static_cast<int>(values.size()));
    }

    int versions() const{
        return version_count;
    }

    int size(int version = 0) const{
        check_version(version);
        return size_or_zero(roots[static_cast<std::size_t>(version)]);
    }

    bool empty(int version = 0) const{
        return size(version) == 0;
    }

    int insert(int p, const S& value, int version = 0){
        check_insert_position(version, p);
        auto [left, right] = split(roots[static_cast<std::size_t>(version)], p);
        return register_root(merge(merge(left, new_node(value)), right));
    }

    int push_front(const S& value, int version = 0){
        return insert(0, value, version);
    }

    int push_back(const S& value, int version = 0){
        return insert(size(version), value, version);
    }

    int erase(int p, int version = 0){
        check_position(version, p);
        auto [left, right] = split(roots[static_cast<std::size_t>(version)], p);
        auto [mid, tail] = split(right, 1);
        (void)mid;
        return register_root(merge(left, tail));
    }

    int set(int p, const S& value, int version = 0){
        check_position(version, p);
        auto [left, right] = split(roots[static_cast<std::size_t>(version)], p);
        auto [mid, tail] = split(right, 1);
        nodes[static_cast<std::size_t>(mid)].value = value;
        nodes[static_cast<std::size_t>(mid)].aggregate = value;
        nodes[static_cast<std::size_t>(mid)].rev_aggregate = value;
        nodes[static_cast<std::size_t>(mid)].reversed = false;
        return register_root(merge(merge(left, mid), tail));
    }

    S get(int p, int version = 0) const{
        check_position(version, p);
        return get_rec(roots[static_cast<std::size_t>(version)], p, false);
    }

    int reverse(int l, int r, int version = 0){
        check_range(version, l, r);
        auto [left, right] = split(roots[static_cast<std::size_t>(version)], l);
        auto [mid, tail] = split(right, r - l);
        toggle(mid);
        return register_root(merge(merge(left, mid), tail));
    }

    S prod(int l, int r, int version = 0) const{
        check_range(version, l, r);
        return prod_rec(roots[static_cast<std::size_t>(version)], 0, size(version), l, r, false);
    }

    S all_prod(int version = 0) const{
        check_version(version);
        return aggregate_or_e(roots[static_cast<std::size_t>(version)]);
    }

    std::vector<S> to_vector(int version = 0) const{
        check_version(version);
        std::vector<S> result;
        result.reserve(static_cast<std::size_t>(size(version)));
        for(int i = 0; i < size(version); i++) result.push_back(get(i, version));
        return result;
    }
};
