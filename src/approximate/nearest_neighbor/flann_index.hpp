#ifndef CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_FLANN_INDEX_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_FLANN_INDEX_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include "ann_common.hpp"

namespace approximate::nearest_neighbor {

template<class Real, std::size_t Dimension>
class FlannIndex {
    static_assert(Dimension > 0, "Dimension must be positive");

public:
    using Point = std::array<Real, Dimension>;

    FlannIndex() = default;

    template<class UniformRandomBitGenerator>
    FlannIndex(
        std::vector<Point> points,
        std::size_t tree_count,
        std::size_t leaf_size,
        UniformRandomBitGenerator& random
    ) {
        reset(
            std::move(points), tree_count, leaf_size, random
        );
    }

    template<class UniformRandomBitGenerator>
    void reset(
        std::vector<Point> points,
        std::size_t tree_count,
        std::size_t leaf_size,
        UniformRandomBitGenerator& random
    ) {
        if(tree_count == 0 || leaf_size == 0){
            throw std::invalid_argument(
                "FLANN tree count and leaf size must be positive"
            );
        }
        for(const Point& point : points){
            detail::validate_ann_point(point);
        }
        FlannIndex replacement;
        replacement.points_ = std::move(points);
        replacement.leaf_size_ = leaf_size;
        replacement.trees_.resize(tree_count);
        std::vector<std::size_t> indices(
            replacement.points_.size()
        );
        std::iota(indices.begin(), indices.end(), std::size_t{0});
        for(Tree& tree : replacement.trees_){
            tree.root = replacement.build(
                tree, indices, random
            );
        }
        *this = std::move(replacement);
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return points_.size();
    }

    [[nodiscard]] bool empty() const noexcept {
        return points_.empty();
    }

    [[nodiscard]] std::size_t tree_count() const noexcept {
        return trees_.size();
    }

    [[nodiscard]] std::size_t leaf_size() const noexcept {
        return leaf_size_;
    }

    [[nodiscard]] const Point& point(std::size_t index) const {
        return points_.at(index);
    }

    template<class QueryReal>
    [[nodiscard]] std::vector<AnnNeighbor> nearest(
        const std::array<QueryReal, Dimension>& query,
        std::size_t k,
        std::size_t checks = 64
    ) const {
        return search(query, k, checks, std::nullopt);
    }

    [[nodiscard]] std::vector<AnnNeighbor> neighbors_of(
        std::size_t index,
        std::size_t k,
        std::size_t checks = 64
    ) const {
        if(index >= points_.size()){
            throw std::out_of_range(
                "FLANN point index is out of range"
            );
        }
        return search(points_[index], k, checks, index);
    }

private:
    static constexpr std::size_t absent() noexcept {
        return std::numeric_limits<std::size_t>::max();
    }

    struct Node {
        std::size_t axis = 0;
        long double threshold = 0.0L;
        std::size_t left = absent();
        std::size_t right = absent();
        std::vector<std::size_t> points;
    };

    struct Tree {
        std::vector<Node> nodes;
        std::size_t root = absent();
    };

    template<class UniformRandomBitGenerator>
    std::size_t build(
        Tree& tree,
        std::vector<std::size_t> indices,
        UniformRandomBitGenerator& random
    ) {
        if(indices.empty()) return absent();
        const std::size_t node = tree.nodes.size();
        tree.nodes.emplace_back();
        if(indices.size() <= leaf_size_){
            std::sort(indices.begin(), indices.end());
            tree.nodes[node].points = std::move(indices);
            return node;
        }

        std::array<long double, Dimension> variance{};
        for(std::size_t coordinate = 0;
            coordinate < Dimension; ++coordinate){
            long double mean = 0.0L;
            for(const std::size_t index : indices){
                mean += static_cast<long double>(
                    points_[index][coordinate]
                );
            }
            mean /= static_cast<long double>(indices.size());
            for(const std::size_t index : indices){
                const long double difference =
                    static_cast<long double>(
                        points_[index][coordinate]
                    ) - mean;
                variance[coordinate] += difference * difference;
            }
        }
        std::array<std::size_t, Dimension> axes{};
        std::iota(axes.begin(), axes.end(), std::size_t{0});
        std::stable_sort(
            axes.begin(), axes.end(),
            [&](std::size_t first, std::size_t second){
                return variance[second] < variance[first];
            }
        );
        const std::size_t choice_count =
            std::min<std::size_t>(Dimension, 5);
        std::uniform_int_distribution<std::size_t> choose_axis(
            0, choice_count - 1
        );
        const std::size_t axis = axes[choose_axis(random)];
        const std::size_t middle = indices.size() / 2;
        std::nth_element(
            indices.begin(),
            indices.begin() + static_cast<std::ptrdiff_t>(middle),
            indices.end(),
            [&](std::size_t first, std::size_t second){
                if(points_[first][axis] != points_[second][axis]){
                    return points_[first][axis]
                        < points_[second][axis];
                }
                return first < second;
            }
        );
        long double left_max =
            -std::numeric_limits<long double>::infinity();
        long double right_min =
            std::numeric_limits<long double>::infinity();
        for(std::size_t position = 0;
            position < middle; ++position){
            left_max = std::max(
                left_max,
                static_cast<long double>(
                    points_[indices[position]][axis]
                )
            );
        }
        for(std::size_t position = middle;
            position < indices.size(); ++position){
            right_min = std::min(
                right_min,
                static_cast<long double>(
                    points_[indices[position]][axis]
                )
            );
        }
        tree.nodes[node].axis = axis;
        tree.nodes[node].threshold =
            std::midpoint(left_max, right_min);
        std::vector<std::size_t> left(
            indices.begin(),
            indices.begin() + static_cast<std::ptrdiff_t>(middle)
        );
        std::vector<std::size_t> right(
            indices.begin() + static_cast<std::ptrdiff_t>(middle),
            indices.end()
        );
        const std::size_t left_child =
            build(tree, std::move(left), random);
        const std::size_t right_child =
            build(tree, std::move(right), random);
        tree.nodes[node].left = left_child;
        tree.nodes[node].right = right_child;
        return node;
    }

    template<class QueryReal>
    std::vector<AnnNeighbor> search(
        const std::array<QueryReal, Dimension>& query,
        std::size_t k,
        std::size_t checks,
        std::optional<std::size_t> excluded
    ) const {
        detail::validate_ann_point(query);
        if(k == 0) throw std::invalid_argument("k must be positive");
        const std::size_t available =
            points_.size() - (excluded.has_value() ? 1U : 0U);
        if(k > available){
            throw std::invalid_argument(
                "k exceeds the available FLANN point count"
            );
        }
        if(checks < k){
            throw std::invalid_argument(
                "FLANN checks must be at least k"
            );
        }
        checks = std::min(checks, available);
        struct Branch {
            long double lower_bound = 0.0L;
            std::size_t tree = 0;
            std::size_t node = absent();
        };
        const auto compare = [](const Branch& first, const Branch& second){
            if(first.lower_bound != second.lower_bound){
                return second.lower_bound < first.lower_bound;
            }
            return std::tie(first.tree, first.node)
                > std::tie(second.tree, second.node);
        };
        std::priority_queue<
            Branch, std::vector<Branch>, decltype(compare)
        > queue(compare);
        for(std::size_t tree = 0; tree < trees_.size(); ++tree){
            if(trees_[tree].root != absent()){
                queue.push({0.0L, tree, trees_[tree].root});
            }
        }
        std::vector<unsigned char> visited(points_.size(), 0);
        std::vector<AnnNeighbor> candidates;
        candidates.reserve(checks);
        while(!queue.empty() && candidates.size() < checks){
            const Branch branch = queue.top();
            queue.pop();
            const Node& node =
                trees_[branch.tree].nodes[branch.node];
            if(!node.points.empty()){
                for(const std::size_t index : node.points){
                    if(candidates.size() == checks) break;
                    if(visited[index] != 0
                       || (excluded.has_value() && index == *excluded)){
                        continue;
                    }
                    visited[index] = 1;
                    candidates.push_back({
                        index,
                        detail::ann_squared_distance(
                            points_[index], query
                        )
                    });
                }
                continue;
            }
            const long double difference =
                static_cast<long double>(query[node.axis])
                - node.threshold;
            const std::size_t near_child =
                difference <= 0.0L ? node.left : node.right;
            const std::size_t far_child =
                difference <= 0.0L ? node.right : node.left;
            if(near_child != absent()){
                queue.push({
                    branch.lower_bound, branch.tree, near_child
                });
            }
            if(far_child != absent()){
                queue.push({
                    std::max(
                        branch.lower_bound,
                        difference * difference
                    ),
                    branch.tree,
                    far_child
                });
            }
        }
        if(candidates.size() < k){
            return detail::exact_knn(
                points_, query, k, excluded
            );
        }
        std::sort(
            candidates.begin(), candidates.end(),
            detail::neighbor_less
        );
        candidates.resize(k);
        return candidates;
    }

    std::vector<Point> points_;
    std::vector<Tree> trees_;
    std::size_t leaf_size_ = 0;
};

}  // namespace approximate::nearest_neighbor

#endif  // CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_FLANN_INDEX_HPP_INCLUDED
