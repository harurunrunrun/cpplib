#ifndef CPPLIB_SRC_STRUCTURE_TREE_DYNAMIC_FOREST_EULER_TOUR_FOREST_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TREE_DYNAMIC_FOREST_EULER_TOUR_FOREST_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

// Implicit-treap Euler tours. Only vertex tokens carry values; directed edge
// tokens make both sides of a cut contiguous in the cyclic Euler traversal.
template<class T>
class EulerTourForest{
    struct Node{
        int left = -1, right = -1, parent = -1, size = 1, count = 0;
        std::uint64_t priority = 0;
        T value{}, sum{}, lazy{};
        bool vertex = false;
    };
    int n;
    std::vector<Node> nodes;
    std::vector<int> free_nodes;
    std::unordered_map<std::uint64_t, std::pair<int, int>> edges;
    std::uint64_t state = 0x6a09e667f3bcc909ULL;
    std::uint64_t random(){
        std::uint64_t z = (state += 0x9e3779b97f4a7c15ULL);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }
    int size_of(int v) const{ return v == -1 ? 0 : nodes[v].size; }
    int count_of(int v) const{ return v == -1 ? 0 : nodes[v].count; }
    T sum_of(int v) const{ return v == -1 ? T{} : nodes[v].sum; }
    void pull(int v){
        nodes[v].size = 1 + size_of(nodes[v].left) + size_of(nodes[v].right);
        nodes[v].count = nodes[v].vertex + count_of(nodes[v].left) + count_of(nodes[v].right);
        nodes[v].sum = nodes[v].value + sum_of(nodes[v].left) + sum_of(nodes[v].right);
    }
    void apply(int v, const T& x){
        if(v == -1) return;
        nodes[v].sum += x * T(nodes[v].count);
        if(nodes[v].vertex) nodes[v].value += x;
        nodes[v].lazy += x;
    }
    void push(int v){
        if(nodes[v].lazy == T{}) return;
        apply(nodes[v].left, nodes[v].lazy); apply(nodes[v].right, nodes[v].lazy);
        nodes[v].lazy = T{};
    }
    int merge(int a, int b){
        if(a == -1){ if(b != -1) nodes[b].parent = -1; return b; }
        if(b == -1){ nodes[a].parent = -1; return a; }
        if(nodes[a].priority < nodes[b].priority){
            push(a); int c = merge(nodes[a].right, b);
            nodes[a].right = c; nodes[c].parent = a; nodes[a].parent = -1; pull(a); return a;
        }
        push(b); int c = merge(a, nodes[b].left);
        nodes[b].left = c; nodes[c].parent = b; nodes[b].parent = -1; pull(b); return b;
    }
    std::pair<int, int> split(int v, int count){
        if(v == -1) return {-1, -1};
        push(v);
        if(count <= size_of(nodes[v].left)){
            auto [a, b] = split(nodes[v].left, count); nodes[v].left = b;
            if(b != -1) nodes[b].parent = v;
            nodes[v].parent = -1; pull(v); return {a, v};
        }
        auto [a, b] = split(nodes[v].right, count - size_of(nodes[v].left) - 1); nodes[v].right = a;
        if(a != -1) nodes[a].parent = v;
        nodes[v].parent = -1; pull(v); return {v, b};
    }
    int root(int v) const{ while(nodes[v].parent != -1) v = nodes[v].parent; return v; }
    int position(int v) const{
        int p = size_of(nodes[v].left);
        while(nodes[v].parent != -1){ int u = nodes[v].parent; if(nodes[u].right == v) p += 1 + size_of(nodes[u].left); v = u; }
        return p;
    }
    int reroot(int v){ int r = root(v); auto [a, b] = split(r, position(v)); return merge(b, a); }
    int edge_node(){
        int v;
        if(free_nodes.empty()){ v = static_cast<int>(nodes.size()); nodes.push_back({}); }
        else{ v = free_nodes.back(); free_nodes.pop_back(); nodes[v] = Node{}; }
        nodes[v].priority = random(); return v;
    }
    static std::uint64_t key(int u, int v){
        if(u > v) std::swap(u, v);
        return (static_cast<std::uint64_t>(u) << 32) | static_cast<unsigned>(v);
    }
    void check(int v) const{ if(v < 0 || v >= n) throw std::out_of_range("Euler tour vertex"); }
public:
    explicit EulerTourForest(const std::vector<T>& values): n(static_cast<int>(values.size())), nodes(values.size()){
        nodes.reserve(3 * values.size()); edges.reserve(values.size());
        for(int v = 0; v < n; ++v){ nodes[v].vertex = true; nodes[v].count = 1; nodes[v].value = nodes[v].sum = values[v]; nodes[v].priority = random(); }
    }
    int size() const{ return n; }
    bool connected(int u, int v) const{ check(u); check(v); return root(u) == root(v); }
    bool link(int u, int v){
        check(u); check(v); if(root(u) == root(v)) return false;
        int a = reroot(u), b = reroot(v), x = edge_node(), y = edge_node();
        merge(merge(merge(a, x), b), y); edges.emplace(key(u, v), std::pair(x, y)); return true;
    }
    bool cut(int u, int v){
        check(u); check(v); auto it = edges.find(key(u, v)); if(it == edges.end()) return false;
        auto [x, y] = it->second; edges.erase(it);
        int px = position(x), py = position(y); if(px > py){ std::swap(px, py); std::swap(x, y); }
        auto [a, b] = split(root(x), px);
        auto [discard_x, c] = split(b, 1); (void)discard_x;
        auto [middle, d] = split(c, py - px - 1); (void)middle;
        auto [discard_y, tail] = split(d, 1); (void)discard_y;
        merge(tail, a); free_nodes.push_back(x); free_nodes.push_back(y); return true;
    }
    void vertex_add(int v, const T& x){
        check(v); std::vector<int> path; for(int u = v; u != -1; u = nodes[u].parent) path.push_back(u);
        for(auto it = path.rbegin(); it != path.rend(); ++it) push(*it);
        nodes[v].value += x; for(int u: path) pull(u);
    }
    T component_sum(int v) const{ check(v); return nodes[root(v)].sum; }
    void component_add(int v, const T& x){ check(v); apply(root(v), x); }
    T subtree_sum(int v, int parent){
        if(!cut(v, parent)) throw std::invalid_argument("Euler tour subtree parent is not adjacent");
        T answer = component_sum(v); link(v, parent); return answer;
    }
    void subtree_add(int v, int parent, const T& x){
        if(!cut(v, parent)) throw std::invalid_argument("Euler tour subtree parent is not adjacent");
        component_add(v, x); link(v, parent);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TREE_DYNAMIC_FOREST_EULER_TOUR_FOREST_HPP_INCLUDED
