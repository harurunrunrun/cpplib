#ifndef CPPLIB_SRC_STRUCTURE_SPATIAL_OFFLINE_INDEXED_RECTANGLE_AFFINE_SUM_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_SPATIAL_OFFLINE_INDEXED_RECTANGLE_AFFINE_SUM_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

// Coordinates of every point are registered before construction. Point IDs are
// independent even at equal coordinates; inactive points ignore past actions.
template<class T>
class OfflineIndexedRectangleAffineSum{
public:
    using coordinate_type = std::int64_t;
private:
    struct Node{
        int left = -1, right = -1, parent = -1, count = 0;
        coordinate_type x, y, xl, xr, yl, yr;
        bool active = false;
        T value{}, sum{}, multiplier{1}, addend{};
    };
    std::vector<Node> nodes;
    int root = -1;
    int build(std::vector<int>& ids, int l, int r, int depth){
        if(l == r) return -1;
        int mid = (l + r) / 2;
        std::nth_element(ids.begin() + l, ids.begin() + mid, ids.begin() + r, [&](int a, int b){
            return depth & 1 ? std::pair(nodes[a].y, nodes[a].x) < std::pair(nodes[b].y, nodes[b].x)
                             : std::pair(nodes[a].x, nodes[a].y) < std::pair(nodes[b].x, nodes[b].y);
        });
        int v = ids[mid]; auto& n = nodes[v];
        n.left = build(ids, l, mid, depth + 1); n.right = build(ids, mid + 1, r, depth + 1);
        n.xl = n.xr = n.x; n.yl = n.yr = n.y;
        for(int c: {n.left, n.right}) if(c != -1){
            nodes[c].parent = v;
            n.xl = std::min(n.xl, nodes[c].xl); n.xr = std::max(n.xr, nodes[c].xr);
            n.yl = std::min(n.yl, nodes[c].yl); n.yr = std::max(n.yr, nodes[c].yr);
        }
        return v;
    }
    void pull(int v){
        auto& n = nodes[v]; n.count = n.active; n.sum = n.active ? n.value : T{};
        for(int c: {n.left, n.right}) if(c != -1){ n.count += nodes[c].count; n.sum += nodes[c].sum; }
    }
    void apply(int v, const T& a, const T& b){
        if(v == -1 || nodes[v].count == 0) return;
        auto& n = nodes[v];
        if(n.active) n.value = a * n.value + b;
        n.sum = a * n.sum + b * T(n.count);
        n.multiplier = a * n.multiplier; n.addend = a * n.addend + b;
    }
    void push(int v){
        auto& n = nodes[v];
        if(n.multiplier == T{1} && n.addend == T{}) return;
        apply(n.left, n.multiplier, n.addend); apply(n.right, n.multiplier, n.addend);
        n.multiplier = T{1}; n.addend = T{};
    }
    static bool outside(const Node& n, coordinate_type l, coordinate_type d, coordinate_type r, coordinate_type u){
        return n.xr < l || r <= n.xl || n.yr < d || u <= n.yl;
    }
    static bool inside(const Node& n, coordinate_type l, coordinate_type d, coordinate_type r, coordinate_type u){
        return l <= n.xl && n.xr < r && d <= n.yl && n.yr < u;
    }
    void affine(int v, coordinate_type l, coordinate_type d, coordinate_type r, coordinate_type u, const T& a, const T& b){
        if(v == -1 || nodes[v].count == 0 || outside(nodes[v], l, d, r, u)) return;
        if(inside(nodes[v], l, d, r, u)){ apply(v, a, b); return; }
        push(v); auto& n = nodes[v];
        if(n.active && l <= n.x && n.x < r && d <= n.y && n.y < u) n.value = a * n.value + b;
        affine(n.left, l, d, r, u, a, b); affine(n.right, l, d, r, u, a, b); pull(v);
    }
    T sum(int v, coordinate_type l, coordinate_type d, coordinate_type r, coordinate_type u){
        if(v == -1 || nodes[v].count == 0 || outside(nodes[v], l, d, r, u)) return T{};
        if(inside(nodes[v], l, d, r, u)) return nodes[v].sum;
        push(v); auto& n = nodes[v];
        T answer = n.active && l <= n.x && n.x < r && d <= n.y && n.y < u ? n.value : T{};
        return answer + sum(n.left, l, d, r, u) + sum(n.right, l, d, r, u);
    }
    void check_rectangle(coordinate_type l, coordinate_type d, coordinate_type r, coordinate_type u) const{
        if(l > r || d > u) throw std::invalid_argument("indexed rectangle bounds");
    }
public:
    explicit OfflineIndexedRectangleAffineSum(const std::vector<std::pair<coordinate_type, coordinate_type>>& coordinates): nodes(coordinates.size()){
        std::vector<int> ids(nodes.size()); std::iota(ids.begin(), ids.end(), 0);
        for(int i = 0; i < static_cast<int>(nodes.size()); ++i){ nodes[i].x = coordinates[i].first; nodes[i].y = coordinates[i].second; }
        root = build(ids, 0, static_cast<int>(ids.size()), 0);
    }
    int size() const{ return static_cast<int>(nodes.size()); }
    // Activate a registered point, or replace an already active point's weight.
    void point_set(int id, const T& value){
        if(id < 0 || id >= size()) throw std::out_of_range("indexed rectangle point");
        std::vector<int> path;
        for(int v = id; v != -1; v = nodes[v].parent) path.push_back(v);
        for(auto i = path.rbegin(); i != path.rend(); ++i) push(*i);
        nodes[id].active = true; nodes[id].value = value;
        for(int v: path) pull(v);
    }
    bool point_erase(int id){
        if(id < 0 || id >= size()) throw std::out_of_range("indexed rectangle point");
        if(!nodes[id].active) return false;
        std::vector<int> path;
        for(int v = id; v != -1; v = nodes[v].parent) path.push_back(v);
        for(auto i = path.rbegin(); i != path.rend(); ++i) push(*i);
        nodes[id].active = false; nodes[id].value = T{};
        for(int v: path) pull(v);
        return true;
    }
    void rectangle_affine(coordinate_type l, coordinate_type d, coordinate_type r, coordinate_type u, const T& a, const T& b){
        check_rectangle(l, d, r, u); affine(root, l, d, r, u, a, b);
    }
    T rectangle_sum(coordinate_type l, coordinate_type d, coordinate_type r, coordinate_type u){
        check_rectangle(l, d, r, u); return sum(root, l, d, r, u);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_SPATIAL_OFFLINE_INDEXED_RECTANGLE_AFFINE_SUM_HPP_INCLUDED
