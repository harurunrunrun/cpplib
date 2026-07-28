#ifndef CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_OFFLINE_3D_DOMINANCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_OFFLINE_3D_DOMINANCE_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <vector>

struct DominancePoint3D{
    long long x, y, z;
};

inline std::vector<long long> offline_3d_dominance(const std::vector<DominancePoint3D>& points){
    struct Node { long long x, y, z; int index, z_index; };
    const int n = static_cast<int>(points.size());
    std::vector<long long> zs;
    zs.reserve(n);
    for(const auto& point: points) zs.push_back(point.z);
    std::sort(zs.begin(), zs.end());
    zs.erase(std::unique(zs.begin(), zs.end()), zs.end());
    std::vector<Node> nodes;
    nodes.reserve(n);
    for(int i = 0; i < n; ++i){
        nodes.push_back({points[i].x, points[i].y, points[i].z, i,
                         static_cast<int>(std::lower_bound(zs.begin(), zs.end(), points[i].z) - zs.begin()) + 1});
    }
    std::sort(nodes.begin(), nodes.end(), [](const Node& a, const Node& b){
        if(a.x != b.x) return a.x < b.x;
        if(a.y != b.y) return a.y < b.y;
        return a.z < b.z;
    });
    std::vector<long long> answer(n);
    std::vector<int> bit(zs.size() + 1);
    auto add = [&](int p, int value){ for(; p < static_cast<int>(bit.size()); p += p & -p) bit[p] += value; };
    auto sum = [&](int p){ int result = 0; for(; p > 0; p -= p & -p) result += bit[p]; return result; };
    std::vector<Node> buffer(n);
    auto solve = [&](auto&& self, int left, int right) -> void {
        if(right - left <= 1) return;
        const int middle = left + (right - left) / 2;
        self(self, left, middle);
        self(self, middle, right);
        int i = left, j = middle, k = left;
        while(j < right){
            while(i < middle && nodes[i].y <= nodes[j].y){
                add(nodes[i].z_index, 1);
                buffer[k++] = nodes[i++];
            }
            answer[nodes[j].index] += sum(nodes[j].z_index);
            buffer[k++] = nodes[j++];
        }
        while(i < middle){ add(nodes[i].z_index, 1); buffer[k++] = nodes[i++]; }
        for(int p = left; p < middle; ++p) add(nodes[p].z_index, -1);
        while(j < right) buffer[k++] = nodes[j++];
        std::copy(buffer.begin() + left, buffer.begin() + right, nodes.begin() + left);
    };
    solve(solve, 0, n);
    // Equal points must count all other equal points, including those in the same CDQ side.
    for(int left = 0; left < n; ){
        int right = left + 1;
        while(right < n && nodes[right].x == nodes[left].x &&
              nodes[right].y == nodes[left].y && nodes[right].z == nodes[left].z) ++right;
        for(int i = left; i < right; ++i) answer[nodes[i].index] += right - i - 1;
        left = right;
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_OFFLINE_QUERY_OFFLINE_3D_DOMINANCE_HPP_INCLUDED
