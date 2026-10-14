#ifndef CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_PERSISTENT_RANGE_AFFINE_SUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_PERSISTENT_RANGE_AFFINE_SUM_HPP_INCLUDED

#include <stdexcept>
#include <vector>

template<class T>
class PersistentRangeAffineSum{
    struct Node{ int left = -1, right = -1; T sum{}, multiplier{1}, addend{}; };
    int n;
    std::vector<Node> nodes;
    std::vector<int> roots;
    int clone(int v){ nodes.push_back(nodes[v]); return static_cast<int>(nodes.size()) - 1; }
    int build(const std::vector<T>& a, int l, int r){
        int v = static_cast<int>(nodes.size()); nodes.push_back({});
        if(r - l == 1) nodes[v].sum = a[l];
        else{
            int m = (l + r) / 2;
            int x = build(a, l, m), y = build(a, m, r);
            nodes[v].left = x; nodes[v].right = y; pull(v);
        }
        return v;
    }
    void pull(int v){ nodes[v].sum = nodes[nodes[v].left].sum + nodes[nodes[v].right].sum; }
    void apply_node(int v, int length, const T& a, const T& b){
        nodes[v].sum = a * nodes[v].sum + b * T(length);
        nodes[v].multiplier = a * nodes[v].multiplier;
        nodes[v].addend = a * nodes[v].addend + b;
    }
    void push(int v, int l, int r){
        if(nodes[v].multiplier == T{1} && nodes[v].addend == T{}) return;
        const T a = nodes[v].multiplier, b = nodes[v].addend;
        const int m = (l + r) / 2;
        int x = clone(nodes[v].left), y = clone(nodes[v].right);
        apply_node(x, m - l, a, b); apply_node(y, r - m, a, b);
        nodes[v].left = x; nodes[v].right = y;
        nodes[v].multiplier = T{1}; nodes[v].addend = T{};
    }
    int update(int v, int l, int r, int ql, int qr, const T& a, const T& b){
        if(qr <= l || r <= ql) return v;
        int w = clone(v);
        if(ql <= l && r <= qr){ apply_node(w, r - l, a, b); return w; }
        push(w, l, r);
        int m = (l + r) / 2;
        int x = update(nodes[w].left, l, m, ql, qr, a, b);
        int y = update(nodes[w].right, m, r, ql, qr, a, b);
        nodes[w].left = x; nodes[w].right = y; pull(w);
        return w;
    }
    int copy(int dst, int src, int l, int r, int ql, int qr, const T& a, const T& b){
        if(qr <= l || r <= ql) return dst;
        if(ql <= l && r <= qr){
            if(a == T{1} && b == T{}) return src;
            int w = clone(src); apply_node(w, r - l, a, b); return w;
        }
        const T sa = a * nodes[src].multiplier, sb = a * nodes[src].addend + b;
        const int sx = nodes[src].left, sy = nodes[src].right;
        int w = clone(dst); push(w, l, r);
        int m = (l + r) / 2;
        int x = copy(nodes[w].left, sx, l, m, ql, qr, sa, sb);
        int y = copy(nodes[w].right, sy, m, r, ql, qr, sa, sb);
        nodes[w].left = x; nodes[w].right = y; pull(w);
        return w;
    }
    T sum(int v, int l, int r, int ql, int qr, const T& a, const T& b) const{
        if(qr <= l || r <= ql) return T{};
        if(ql <= l && r <= qr) return a * nodes[v].sum + b * T(r - l);
        const T na = a * nodes[v].multiplier, nb = a * nodes[v].addend + b;
        int m = (l + r) / 2;
        return sum(nodes[v].left, l, m, ql, qr, na, nb) + sum(nodes[v].right, m, r, ql, qr, na, nb);
    }
    void check(int version, int l, int r) const{
        if(version < 0 || version >= versions() || l < 0 || l > r || r > n)
            throw std::out_of_range("persistent affine version or interval");
    }
public:
    explicit PersistentRangeAffineSum(const std::vector<T>& a): n(static_cast<int>(a.size())){
        nodes.reserve(2 * a.size()); roots.push_back(n ? build(a, 0, n) : -1);
    }
    int size() const{ return n; }
    int versions() const{ return static_cast<int>(roots.size()); }
    int affine(int version, int l, int r, const T& a, const T& b){
        check(version, l, r);
        int root = l == r ? roots[version] : update(roots[version], 0, n, l, r, a, b);
        roots.push_back(root); return versions() - 1;
    }
    int copy_range(int destination, int source, int l, int r){
        check(destination, l, r); check(source, l, r);
        int root = l == r ? roots[destination] : copy(roots[destination], roots[source], 0, n, l, r, T{1}, T{});
        roots.push_back(root); return versions() - 1;
    }
    T range_sum(int version, int l, int r) const{
        check(version, l, r);
        return l == r ? T{} : sum(roots[version], 0, n, l, r, T{1}, T{});
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SEGTREE_PERSISTENT_PERSISTENT_RANGE_AFFINE_SUM_HPP_INCLUDED
