#pragma once

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <utility>
#include <vector>

namespace convex_layers_detail{

struct Point{
    __int128_t x;
    __int128_t y;
};

inline __int128_t cross(const Point& first, const Point& second, const Point& third){
    return (second.x - first.x) * (third.y - first.y)
        - (second.y - first.y) * (third.x - first.x);
}

class DecrementalLeftHull{
    struct Node{
        int range_left = 0;
        int range_right = 0;
        int bridge_left = 0;
        int bridge_right = 0;
        int left_child = -1;
        int right_child = -1;
    };

public:
    explicit DecrementalLeftHull(const std::vector<Point>& points)
        : points_(points), nodes_(2 * points.size()), root_(points.empty() ? -1 : 0){
        if(!points.empty()) build(0, 0, static_cast<int>(points.size()));
    }

    std::vector<int> hull_indices() const{
        if(root_ == -1) return {};
        std::vector<int> result;
        collect(root_, 0, static_cast<int>(points_.size()) - 1, result);
        return result;
    }

    void erase(int index){
        root_ = erase_range(root_, index, index + 1);
    }

private:
    bool is_leaf(int node) const{
        return nodes_[static_cast<std::size_t>(node)].left_child == -1
            && nodes_[static_cast<std::size_t>(node)].right_child == -1;
    }

    void pull(int node){
        const int original_left = nodes_[static_cast<std::size_t>(node)].left_child;
        const int original_right = nodes_[static_cast<std::size_t>(node)].right_child;
        int left = original_left;
        int right = original_right;
        const __int128_t split_y = points_[static_cast<std::size_t>(
            nodes_[static_cast<std::size_t>(right)].range_left
        )].y;
        while(!is_leaf(left) || !is_leaf(right)){
            const int first = nodes_[static_cast<std::size_t>(left)].bridge_left;
            const int second = nodes_[static_cast<std::size_t>(left)].bridge_right;
            const int third = nodes_[static_cast<std::size_t>(right)].bridge_left;
            const int fourth = nodes_[static_cast<std::size_t>(right)].bridge_right;
            if(first != second && cross(
                points_[static_cast<std::size_t>(first)],
                points_[static_cast<std::size_t>(second)],
                points_[static_cast<std::size_t>(third)]
            ) > 0){
                left = nodes_[static_cast<std::size_t>(left)].left_child;
            }else if(third != fourth && cross(
                points_[static_cast<std::size_t>(second)],
                points_[static_cast<std::size_t>(third)],
                points_[static_cast<std::size_t>(fourth)]
            ) > 0){
                right = nodes_[static_cast<std::size_t>(right)].right_child;
            }else if(first == second){
                right = nodes_[static_cast<std::size_t>(right)].left_child;
            }else if(third == fourth){
                left = nodes_[static_cast<std::size_t>(left)].right_child;
            }else{
                const __int128_t first_area = cross(
                    points_[static_cast<std::size_t>(first)],
                    points_[static_cast<std::size_t>(second)],
                    points_[static_cast<std::size_t>(third)]
                );
                const __int128_t second_area = cross(
                    points_[static_cast<std::size_t>(second)],
                    points_[static_cast<std::size_t>(first)],
                    points_[static_cast<std::size_t>(fourth)]
                );
                assert(first_area + second_area >= 0);
                if(first_area + second_area == 0
                    || first_area * points_[static_cast<std::size_t>(fourth)].y
                        + second_area * points_[static_cast<std::size_t>(third)].y
                        < split_y * (first_area + second_area)){
                    left = nodes_[static_cast<std::size_t>(left)].right_child;
                }else{
                    right = nodes_[static_cast<std::size_t>(right)].left_child;
                }
            }
        }
        nodes_[static_cast<std::size_t>(node)].bridge_left =
            nodes_[static_cast<std::size_t>(left)].range_left;
        nodes_[static_cast<std::size_t>(node)].bridge_right =
            nodes_[static_cast<std::size_t>(right)].range_left;
    }

    void build(int node, int left, int right){
        Node& current = nodes_[static_cast<std::size_t>(node)];
        current.range_left = left;
        current.range_right = right;
        if(right - left == 1){
            current.bridge_left = left;
            current.bridge_right = left;
            return;
        }
        const int middle = (left + right) / 2;
        current.left_child = node + 1;
        current.right_child = node + 2 * (middle - left);
        build(current.left_child, left, middle);
        build(current.right_child, middle, right);
        pull(node);
    }

    int erase_range(int node, int left, int right){
        Node& current = nodes_[static_cast<std::size_t>(node)];
        if(right <= current.range_left || current.range_right <= left) return node;
        if(left <= current.range_left && current.range_right <= right) return -1;
        current.left_child = erase_range(current.left_child, left, right);
        current.right_child = erase_range(current.right_child, left, right);
        if(current.left_child == -1) return current.right_child;
        if(current.right_child == -1) return current.left_child;
        pull(node);
        return node;
    }

    void collect(int node, int left, int right, std::vector<int>& result) const{
        const Node& current = nodes_[static_cast<std::size_t>(node)];
        if(is_leaf(node)){
            result.push_back(current.range_left);
        }else if(right <= current.bridge_left){
            collect(current.left_child, left, right, result);
        }else if(left >= current.bridge_right){
            collect(current.right_child, left, right, result);
        }else{
            collect(current.left_child, left, current.bridge_left, result);
            collect(current.right_child, current.bridge_right, right, result);
        }
    }

    const std::vector<Point>& points_;
    std::vector<Node> nodes_;
    int root_;
};

}  // namespace convex_layers_detail

template<std::integral Coordinate>
std::vector<int> convex_layers(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
){
    if(points.empty()) return {};

    std::vector<std::size_t> order(points.size());
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](std::size_t left, std::size_t right){
        if(points[left].second != points[right].second){
            return points[left].second < points[right].second;
        }
        if(points[left].first != points[right].first){
            return points[left].first < points[right].first;
        }
        return left < right;
    });

    std::vector<convex_layers_detail::Point> unique_points;
    std::vector<std::vector<std::size_t>> original_indices;
    for(std::size_t index: order){
        if(unique_points.empty()
            || unique_points.back().x != static_cast<__int128_t>(points[index].first)
            || unique_points.back().y != static_cast<__int128_t>(points[index].second)){
            unique_points.push_back({
                static_cast<__int128_t>(points[index].first),
                static_cast<__int128_t>(points[index].second)
            });
            original_indices.emplace_back();
        }
        original_indices.back().push_back(index);
    }

    convex_layers_detail::DecrementalLeftHull left_hull(unique_points);
    std::vector<convex_layers_detail::Point> reflected;
    reflected.reserve(unique_points.size());
    for(std::size_t index = unique_points.size(); index-- > 0;){
        reflected.push_back({-unique_points[index].x, -unique_points[index].y});
    }
    convex_layers_detail::DecrementalLeftHull right_hull(reflected);

    std::vector<int> unique_answer(unique_points.size(), 0);
    std::size_t removed = 0;
    for(int layer = 1; removed < unique_points.size(); ++layer){
        std::vector<int> boundary = left_hull.hull_indices();
        const std::vector<int> reflected_boundary = right_hull.hull_indices();
        boundary.reserve(boundary.size() + reflected_boundary.size());
        for(int index: reflected_boundary){
            boundary.push_back(static_cast<int>(unique_points.size()) - 1 - index);
        }
        std::sort(boundary.begin(), boundary.end());
        boundary.erase(std::unique(boundary.begin(), boundary.end()), boundary.end());
        assert(!boundary.empty());
        for(int index: boundary){
            unique_answer[static_cast<std::size_t>(index)] = layer;
            left_hull.erase(index);
            right_hull.erase(static_cast<int>(unique_points.size()) - 1 - index);
        }
        removed += boundary.size();
    }

    std::vector<int> answer(points.size());
    for(std::size_t index = 0; index < unique_points.size(); ++index){
        for(std::size_t original: original_indices[index]){
            answer[original] = unique_answer[index];
        }
    }
    return answer;
}
