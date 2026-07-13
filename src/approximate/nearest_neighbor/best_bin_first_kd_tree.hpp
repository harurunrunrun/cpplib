#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace approximate::nearest_neighbor {

struct KdTreeNeighbor {
    std::size_t index = 0;
    long double squared_distance = 0.0L;
    std::size_t checked_points = 0;
};

template<class T, std::size_t Dimension>
class BestBinFirstKdTree {
    static_assert(Dimension > 0, "Dimension must be positive");

public:
    using Point = std::array<T, Dimension>;

    BestBinFirstKdTree() = default;
    explicit BestBinFirstKdTree(std::vector<Point> points) { reset(std::move(points)); }

    void reset(std::vector<Point> points) {
        for(const auto& point: points) validate_point(point);
        points_ = std::move(points);
        nodes_.clear();
        root_ = absent();
        std::vector<std::size_t> order(points_.size());
        for(std::size_t index = 0; index < order.size(); ++index) order[index] = index;
        nodes_.reserve(points_.size());
        root_ = build(order, 0, order.size(), 0);
    }

    [[nodiscard]] std::size_t size() const noexcept { return points_.size(); }
    [[nodiscard]] bool empty() const noexcept { return points_.empty(); }
    [[nodiscard]] const Point& point(std::size_t index) const { return points_.at(index); }

    template<class QueryT>
    [[nodiscard]] std::optional<KdTreeNeighbor> nearest(
        const std::array<QueryT, Dimension>& query,
        std::size_t max_checks = std::numeric_limits<std::size_t>::max()
    ) const {
        validate_point(query);
        if(max_checks == 0) throw std::invalid_argument("max_checks must be positive");
        if(root_ == absent()) return std::nullopt;
        std::array<long double, Dimension> long_query{};
        for(std::size_t coordinate = 0; coordinate < Dimension; ++coordinate){
            long_query[coordinate] = static_cast<long double>(query[coordinate]);
        }
        struct Branch { long double lower_bound; std::size_t node; };
        const auto compare = [](const Branch& first, const Branch& second) {
            if(first.lower_bound != second.lower_bound){
                return first.lower_bound > second.lower_bound;
            }
            return first.node > second.node;
        };
        std::priority_queue<Branch, std::vector<Branch>, decltype(compare)> queue(compare);
        queue.push(Branch{box_squared_distance(long_query, nodes_[root_]), root_});
        KdTreeNeighbor result{};
        bool found = false;
        while(!queue.empty() && result.checked_points < max_checks){
            const Branch branch = queue.top();
            queue.pop();
            if(found && branch.lower_bound > result.squared_distance) break;
            const Node& node = nodes_[branch.node];
            const long double candidate = point_squared_distance(long_query, points_[node.point_index]);
            ++result.checked_points;
            if(!found || candidate < result.squared_distance
               || (candidate == result.squared_distance && node.point_index < result.index)){
                found = true;
                result.index = node.point_index;
                result.squared_distance = candidate;
            }
            if(node.left != absent()){
                const long double lower_bound = box_squared_distance(long_query, nodes_[node.left]);
                if(!found || lower_bound <= result.squared_distance){
                    queue.push(Branch{lower_bound, node.left});
                }
            }
            if(node.right != absent()){
                const long double lower_bound = box_squared_distance(long_query, nodes_[node.right]);
                if(!found || lower_bound <= result.squared_distance){
                    queue.push(Branch{lower_bound, node.right});
                }
            }
        }
        return found ? std::optional<KdTreeNeighbor>(result) : std::nullopt;
    }

private:
    struct Node {
        std::size_t point_index = 0;
        std::size_t left = absent();
        std::size_t right = absent();
        std::array<long double, Dimension> minimum{};
        std::array<long double, Dimension> maximum{};
    };

    static constexpr std::size_t absent() noexcept {
        return std::numeric_limits<std::size_t>::max();
    }

    template<class PointT>
    static void validate_point(const PointT& point) {
        for(const auto& coordinate: point){
            if(!std::isfinite(static_cast<long double>(coordinate))){
                throw std::invalid_argument("point contains a non-finite coordinate");
            }
        }
    }

    std::size_t build(
        std::vector<std::size_t>& order,
        std::size_t first,
        std::size_t last,
        std::size_t depth
    ) {
        if(first == last) return absent();
        const std::size_t axis = depth % Dimension;
        const std::size_t middle = first + (last - first) / 2;
        std::nth_element(
            order.begin() + static_cast<std::ptrdiff_t>(first),
            order.begin() + static_cast<std::ptrdiff_t>(middle),
            order.begin() + static_cast<std::ptrdiff_t>(last),
            [&](std::size_t left, std::size_t right) {
                if(points_[left][axis] != points_[right][axis]){
                    return points_[left][axis] < points_[right][axis];
                }
                return left < right;
            }
        );
        const std::size_t node_index = nodes_.size();
        nodes_.push_back(Node{});
        nodes_[node_index].point_index = order[middle];
        nodes_[node_index].left = build(order, first, middle, depth + 1);
        nodes_[node_index].right = build(order, middle + 1, last, depth + 1);
        for(std::size_t coordinate = 0; coordinate < Dimension; ++coordinate){
            const long double value = static_cast<long double>(
                points_[nodes_[node_index].point_index][coordinate]
            );
            nodes_[node_index].minimum[coordinate] = value;
            nodes_[node_index].maximum[coordinate] = value;
            if(nodes_[node_index].left != absent()){
                nodes_[node_index].minimum[coordinate] = std::min(
                    nodes_[node_index].minimum[coordinate],
                    nodes_[nodes_[node_index].left].minimum[coordinate]
                );
                nodes_[node_index].maximum[coordinate] = std::max(
                    nodes_[node_index].maximum[coordinate],
                    nodes_[nodes_[node_index].left].maximum[coordinate]
                );
            }
            if(nodes_[node_index].right != absent()){
                nodes_[node_index].minimum[coordinate] = std::min(
                    nodes_[node_index].minimum[coordinate],
                    nodes_[nodes_[node_index].right].minimum[coordinate]
                );
                nodes_[node_index].maximum[coordinate] = std::max(
                    nodes_[node_index].maximum[coordinate],
                    nodes_[nodes_[node_index].right].maximum[coordinate]
                );
            }
        }
        return node_index;
    }

    template<class PointT>
    static long double point_squared_distance(
        const std::array<long double, Dimension>& query,
        const PointT& point
    ) {
        long double result = 0.0L;
        for(std::size_t coordinate = 0; coordinate < Dimension; ++coordinate){
            const long double difference = query[coordinate]
                - static_cast<long double>(point[coordinate]);
            result += difference * difference;
        }
        if(!std::isfinite(result)) throw std::overflow_error("KD-tree distance overflow");
        return result;
    }

    static long double box_squared_distance(
        const std::array<long double, Dimension>& query,
        const Node& node
    ) {
        long double result = 0.0L;
        for(std::size_t coordinate = 0; coordinate < Dimension; ++coordinate){
            long double difference = 0.0L;
            if(query[coordinate] < node.minimum[coordinate]){
                difference = node.minimum[coordinate] - query[coordinate];
            }else if(query[coordinate] > node.maximum[coordinate]){
                difference = query[coordinate] - node.maximum[coordinate];
            }
            result += difference * difference;
        }
        if(!std::isfinite(result)) throw std::overflow_error("KD-tree box distance overflow");
        return result;
    }

    std::vector<Point> points_;
    std::vector<Node> nodes_;
    std::size_t root_ = absent();
};

}  // namespace approximate::nearest_neighbor
