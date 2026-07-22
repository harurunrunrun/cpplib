#ifndef CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_RANDOM_PROJECTION_FOREST_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_RANDOM_PROJECTION_FOREST_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <optional>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "ann_common.hpp"

namespace approximate::nearest_neighbor {

template<class Real, std::size_t Dimension>
class RandomProjectionForest {
    static_assert(Dimension > 0, "Dimension must be positive");

public:
    using Point = std::array<Real, Dimension>;
    RandomProjectionForest() = default;

    template<class RandomEngine>
    RandomProjectionForest(std::vector<Point> points, std::size_t tree_count,
                           std::size_t leaf_size, RandomEngine& random) {
        reset(std::move(points), tree_count, leaf_size, random);
    }

    template<class RandomEngine>
    void reset(std::vector<Point> points, std::size_t tree_count,
               std::size_t leaf_size, RandomEngine& random) {
        if(tree_count == 0) throw std::invalid_argument("tree_count must be positive");
        if(leaf_size == 0) throw std::invalid_argument("leaf_size must be positive");
        for(const Point& point: points) detail::validate_ann_point(point);

        RandomProjectionForest replacement;
        replacement.points_ = std::move(points);
        replacement.leaf_size_ = leaf_size;
        replacement.trees_.assign(tree_count, Tree{});
        std::vector<std::size_t> indices(replacement.points_.size());
        std::iota(indices.begin(), indices.end(), std::size_t{0});
        for(Tree& tree: replacement.trees_){
            std::size_t node_bound = 0;
            if(!replacement.points_.empty()){
                const std::size_t minimum_leaf_size =
                    replacement.leaf_size_ / 2 + replacement.leaf_size_ % 2;
                const std::size_t leaf_bound =
                    1 + (replacement.points_.size() - 1) / minimum_leaf_size;
                const std::size_t maximum = std::numeric_limits<std::size_t>::max();
                node_bound = leaf_bound <= maximum / 2 + maximum % 2
                    ? leaf_bound * 2 - 1 : replacement.points_.size();
            }
            tree.nodes.reserve(std::min(node_bound, tree.nodes.max_size()));
            tree.root = replacement.build(tree, indices, random);
        }
        points_.swap(replacement.points_);
        trees_.swap(replacement.trees_);
        leaf_size_ = replacement.leaf_size_;
    }

    [[nodiscard]] std::size_t size() const noexcept { return points_.size(); }
    [[nodiscard]] bool empty() const noexcept { return points_.empty(); }
    [[nodiscard]] std::size_t tree_count() const noexcept { return trees_.size(); }
    [[nodiscard]] std::size_t leaf_size() const noexcept { return leaf_size_; }
    [[nodiscard]] const Point& point(std::size_t index) const { return points_.at(index); }

    template<class QueryReal>
    [[nodiscard]] std::vector<AnnNeighbor> nearest(
        const std::array<QueryReal, Dimension>& query, std::size_t k,
        bool exact_fallback = false) const {
        return search(query, k, std::nullopt, exact_fallback);
    }

    [[nodiscard]] std::vector<AnnNeighbor> neighbors_of(
        std::size_t index, std::size_t k, bool exact_fallback = false) const {
        if(index >= points_.size()) throw std::out_of_range("point index is out of range");
        return search(points_[index], k, index, exact_fallback);
    }

private:
    static constexpr std::size_t absent() noexcept {
        return std::numeric_limits<std::size_t>::max();
    }
    struct Node {
        std::array<long double, Dimension> normal{};
        long double threshold = 0.0L;
        std::size_t left = absent();
        std::size_t right = absent();
        std::vector<std::size_t> points;
    };
    struct Tree { std::vector<Node> nodes; std::size_t root = absent(); };

    template<class RandomEngine>
    std::size_t build(Tree& tree, std::vector<std::size_t> indices, RandomEngine& random) {
        if(indices.empty()) return absent();
        const std::size_t node_index = tree.nodes.size();
        tree.nodes.push_back(Node{});
        if(indices.size() <= leaf_size_){
            std::sort(indices.begin(), indices.end());
            tree.nodes[node_index].points = std::move(indices);
            return node_index;
        }
        long double norm = 0.0L;
        for(long double& value: tree.nodes[node_index].normal){
            const auto sample = random() - RandomEngine::min();
            value = sample % 2 == 0 ? -1.0L : 1.0L;
            norm = std::hypot(norm, value);
        }
        for(long double& value: tree.nodes[node_index].normal) value /= norm;
        std::vector<std::pair<long double, std::size_t>> projected;
        projected.reserve(indices.size());
        for(std::size_t index: indices){
            projected.emplace_back(projection(points_[index], tree.nodes[node_index].normal), index);
        }
        std::sort(projected.begin(), projected.end(), [](const auto& first, const auto& second) {
            return first.first < second.first
                || (first.first == second.first && first.second < second.second);
        });
        const std::size_t middle = projected.size() / 2;
        tree.nodes[node_index].threshold =
            std::midpoint(projected[middle - 1].first, projected[middle].first);
        std::vector<std::size_t> left;
        std::vector<std::size_t> right;
        for(std::size_t rank = 0; rank < projected.size(); ++rank){
            (rank < middle ? left : right).push_back(projected[rank].second);
        }
        const std::size_t left_child = build(tree, std::move(left), random);
        const std::size_t right_child = build(tree, std::move(right), random);
        tree.nodes[node_index].left = left_child;
        tree.nodes[node_index].right = right_child;
        return node_index;
    }

    template<class QueryReal>
    std::vector<AnnNeighbor> search(const std::array<QueryReal, Dimension>& query,
                                    std::size_t k, std::optional<std::size_t> excluded,
                                    bool exact_fallback) const {
        detail::validate_ann_point(query);
        if(k == 0) throw std::invalid_argument("k must be positive");
        if(excluded.has_value() && *excluded >= points_.size()){
            throw std::out_of_range("excluded point index is out of range");
        }
        const std::size_t available = points_.size() - (excluded.has_value() ? 1U : 0U);
        if(k > available) throw std::invalid_argument("k exceeds the available point count");
        if(exact_fallback) return detail::exact_knn(points_, query, k, excluded);
        std::vector<std::size_t> candidates;
        for(const Tree& tree: trees_) collect(tree, tree.root, query, candidates);
        detail::sort_unique_indices(candidates);
        std::vector<AnnNeighbor> result;
        result.reserve(candidates.size());
        for(std::size_t index: candidates){
            if(excluded.has_value() && index == *excluded) continue;
            result.push_back({index, detail::ann_squared_distance(points_[index], query)});
        }
        if(result.size() < k) return detail::exact_knn(points_, query, k, excluded);
        std::sort(result.begin(), result.end(), detail::neighbor_less);
        if(result.size() > k) result.resize(k);
        return result;
    }

    template<class QueryReal>
    void collect(const Tree& tree, std::size_t node_index,
                 const std::array<QueryReal, Dimension>& query,
                 std::vector<std::size_t>& result) const {
        if(node_index == absent()) return;
        const Node& node = tree.nodes[node_index];
        if(!node.points.empty()){
            result.insert(result.end(), node.points.begin(), node.points.end());
            return;
        }
        const long double value = projection(query, node.normal);
        if(value <= node.threshold) collect(tree, node.left, query, result);
        if(value >= node.threshold) collect(tree, node.right, query, result);
    }

    template<class Value>
    static long double projection(const std::array<Value, Dimension>& point,
                                  const std::array<long double, Dimension>& normal) {
        long double result = 0.0L;
        for(std::size_t coordinate = 0; coordinate < Dimension; ++coordinate){
            result += static_cast<long double>(point[coordinate]) * normal[coordinate];
        }
        if(!std::isfinite(result)) throw std::overflow_error("projection overflow");
        return result;
    }

    std::vector<Point> points_;
    std::vector<Tree> trees_;
    std::size_t leaf_size_ = 0;
};

}  // namespace approximate::nearest_neighbor

#endif  // CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_RANDOM_PROJECTION_FOREST_HPP_INCLUDED
