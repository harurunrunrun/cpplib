#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_PRIORITY_SEARCH_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_PRIORITY_SEARCH_TREE_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

template<class Coordinate, class Payload = int>
class PrioritySearchTree{
public:
    struct Point{ Coordinate x, y; Payload payload; };
private:
    struct Node{ Point point; Coordinate split; int left = -1, right = -1; };
    std::vector<Node> nodes_;

    int build(std::vector<Point> points){
        if(points.empty()) return -1;
        const auto minimum = std::min_element(points.begin(), points.end(),
            [](const Point& a, const Point& b){ return a.y < b.y; });
        const Point root = *minimum;
        points.erase(minimum);
        std::sort(points.begin(), points.end(), [](const Point& a, const Point& b){ return a.x < b.x; });
        const std::size_t middle = points.size() / 2;
        const Coordinate split = points.empty() ? root.x : points[middle].x;
        std::vector<Point> left(points.begin(), points.begin() + static_cast<std::ptrdiff_t>(middle));
        std::vector<Point> right(points.begin() + static_cast<std::ptrdiff_t>(middle), points.end());
        const int id = static_cast<int>(nodes_.size());
        nodes_.push_back({root, split, -1, -1});
        nodes_[id].left = build(std::move(left));
        nodes_[id].right = build(std::move(right));
        return id;
    }
    void report(int node, const Coordinate& lower_x, const Coordinate& upper_x,
                const Coordinate& upper_y, std::vector<Payload>& answer) const {
        if(node == -1 || !(nodes_[node].point.y < upper_y)) return;
        const Point& point = nodes_[node].point;
        if(!(point.x < lower_x) && point.x < upper_x) answer.push_back(point.payload);
        if(!(nodes_[node].split < lower_x)) report(nodes_[node].left, lower_x, upper_x, upper_y, answer);
        if(nodes_[node].split < upper_x) report(nodes_[node].right, lower_x, upper_x, upper_y, answer);
    }
public:
    PrioritySearchTree() = default;
    explicit PrioritySearchTree(std::vector<Point> points){ nodes_.reserve(points.size()); build(std::move(points)); }
    int size() const noexcept { return static_cast<int>(nodes_.size()); }
    std::vector<Payload> report(const Coordinate& lower_x, const Coordinate& upper_x,
                                const Coordinate& upper_y) const {
        if(upper_x < lower_x) throw std::invalid_argument("PrioritySearchTree: invalid range");
        std::vector<Payload> answer;
        report(nodes_.empty() ? -1 : 0, lower_x, upper_x, upper_y, answer);
        return answer;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_PRIORITY_SEARCH_TREE_HPP_INCLUDED
