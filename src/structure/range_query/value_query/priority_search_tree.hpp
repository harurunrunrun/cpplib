#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_PRIORITY_SEARCH_TREE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_PRIORITY_SEARCH_TREE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

template<class Coordinate, class Payload = int>
class PrioritySearchTree{
public:
    struct Point{
        Coordinate x;
        Coordinate y;
        Payload payload;
    };

private:
    struct Node{
        Point point;
        Coordinate split;
        Coordinate minimum_x;
        Coordinate maximum_x;
        int left = -1;
        int right = -1;
    };

    class RangeMinimum{
        const std::vector<Point>& points_;
        std::size_t leaf_count_ = 1;
        std::vector<int> tree_;

        int minimum_index(int first, int second) const{
            if(first == -1) return second;
            if(second == -1) return first;
            const Point& first_point =
                points_[static_cast<std::size_t>(first)];
            const Point& second_point =
                points_[static_cast<std::size_t>(second)];
            if(first_point.y < second_point.y) return first;
            if(second_point.y < first_point.y) return second;
            return std::min(first, second);
        }

    public:
        explicit RangeMinimum(const std::vector<Point>& points)
            : points_(points){
            while(leaf_count_ < points.size()) leaf_count_ *= 2;
            tree_.assign(2 * leaf_count_, -1);
            for(std::size_t index = 0; index < points.size(); ++index){
                tree_[leaf_count_ + index] = static_cast<int>(index);
            }
            for(std::size_t node = leaf_count_; node-- > 1;){
                tree_[node] = minimum_index(
                    tree_[2 * node], tree_[2 * node + 1]
                );
            }
        }

        int query(std::size_t left, std::size_t right) const{
            int left_result = -1;
            int right_result = -1;
            left += leaf_count_;
            right += leaf_count_;
            while(left < right){
                if((left & 1U) != 0){
                    left_result =
                        minimum_index(left_result, tree_[left]);
                    ++left;
                }
                if((right & 1U) != 0){
                    --right;
                    right_result =
                        minimum_index(tree_[right], right_result);
                }
                left /= 2;
                right /= 2;
            }
            return minimum_index(left_result, right_result);
        }

        void erase(std::size_t index){
            std::size_t node = leaf_count_ + index;
            tree_[node] = -1;
            while(node > 1){
                node /= 2;
                tree_[node] = minimum_index(
                    tree_[2 * node], tree_[2 * node + 1]
                );
            }
        }
    };

    std::vector<Node> nodes_;

    int build(
        const std::vector<Point>& points,
        RangeMinimum& range_minimum,
        std::size_t left,
        std::size_t right
    ){
        const int point_index = range_minimum.query(left, right);
        if(point_index == -1) return -1;

        const std::size_t middle = left + (right - left) / 2;
        const Point root = points[static_cast<std::size_t>(point_index)];
        const Coordinate split = points[middle].x;
        range_minimum.erase(static_cast<std::size_t>(point_index));

        const int node = static_cast<int>(nodes_.size());
        nodes_.push_back({
            root, split, root.x, root.x, -1, -1
        });
        nodes_[static_cast<std::size_t>(node)].left =
            build(points, range_minimum, left, middle);
        nodes_[static_cast<std::size_t>(node)].right =
            build(points, range_minimum, middle, right);
        const int children[] = {
            nodes_[static_cast<std::size_t>(node)].left,
            nodes_[static_cast<std::size_t>(node)].right
        };
        for(const int child: children){
            if(child == -1) continue;
            const Node& child_node =
                nodes_[static_cast<std::size_t>(child)];
            Node& current = nodes_[static_cast<std::size_t>(node)];
            if(child_node.minimum_x < current.minimum_x){
                current.minimum_x = child_node.minimum_x;
            }
            if(current.maximum_x < child_node.maximum_x){
                current.maximum_x = child_node.maximum_x;
            }
        }
        return node;
    }

    void report(
        int node,
        const Coordinate& lower_x,
        const Coordinate& upper_x,
        const Coordinate& upper_y,
        std::vector<Payload>& answer
    ) const{
        if(node == -1) return;
        const Node& current = nodes_[static_cast<std::size_t>(node)];
        if(!(current.point.y < upper_y)
            || current.maximum_x < lower_x
            || !(current.minimum_x < upper_x)) return;
        if(!(current.point.x < lower_x) && current.point.x < upper_x){
            answer.push_back(current.point.payload);
        }
        if(!(current.split < lower_x)){
            report(
                current.left, lower_x, upper_x, upper_y, answer
            );
        }
        if(current.split < upper_x){
            report(
                current.right, lower_x, upper_x, upper_y, answer
            );
        }
    }

public:
    PrioritySearchTree() = default;

    explicit PrioritySearchTree(std::vector<Point> points){
        if(points.size()
            > static_cast<std::size_t>(std::numeric_limits<int>::max())){
            throw std::length_error(
                "PrioritySearchTree: too many points"
            );
        }
        nodes_.reserve(points.size());
        std::sort(
            points.begin(), points.end(),
            [](const Point& first, const Point& second){
                return first.x < second.x;
            }
        );
        RangeMinimum range_minimum(points);
        (void)build(points, range_minimum, 0, points.size());
    }

    int size() const noexcept{
        return static_cast<int>(nodes_.size());
    }

    std::vector<Payload> report(
        const Coordinate& lower_x,
        const Coordinate& upper_x,
        const Coordinate& upper_y
    ) const{
        if(upper_x < lower_x){
            throw std::invalid_argument(
                "PrioritySearchTree: invalid range"
            );
        }
        std::vector<Payload> answer;
        report(
            nodes_.empty() ? -1 : 0,
            lower_x,
            upper_x,
            upper_y,
            answer
        );
        return answer;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_PRIORITY_SEARCH_TREE_HPP_INCLUDED
