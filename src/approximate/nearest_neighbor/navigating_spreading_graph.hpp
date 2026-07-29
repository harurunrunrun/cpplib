#ifndef CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_NAVIGATING_SPREADING_GRAPH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_NAVIGATING_SPREADING_GRAPH_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "ann_common.hpp"

namespace approximate::nearest_neighbor {

template<class Real, std::size_t Dimension>
class NavigatingSpreadingGraph {
    static_assert(Dimension > 0, "Dimension must be positive");

public:
    using Point = std::array<Real, Dimension>;

    NavigatingSpreadingGraph() = default;

    NavigatingSpreadingGraph(
        std::vector<Point> points,
        std::size_t candidate_count,
        std::size_t out_degree
    ) {
        reset(
            std::move(points), candidate_count, out_degree
        );
    }

    void reset(
        std::vector<Point> points,
        std::size_t candidate_count,
        std::size_t out_degree
    ) {
        if(candidate_count == 0 || out_degree == 0){
            throw std::invalid_argument(
                "NSG candidate count and out degree must be positive"
            );
        }
        for(const Point& point : points){
            detail::validate_ann_point(point);
        }
        points_ = std::move(points);
        graph_.assign(points_.size(), {});
        candidate_count_ = candidate_count;
        out_degree_ = out_degree;
        if(points_.empty()){
            navigation_ = absent();
            return;
        }
        choose_navigation_point();
        build_diversified_graph();
        repair_connectivity();
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return points_.size();
    }

    [[nodiscard]] bool empty() const noexcept {
        return points_.empty();
    }

    [[nodiscard]] std::size_t navigation_point() const noexcept {
        return navigation_;
    }

    [[nodiscard]] const Point& point(std::size_t index) const {
        return points_.at(index);
    }

    [[nodiscard]] const std::vector<std::size_t>& outgoing(
        std::size_t index
    ) const {
        return graph_.at(index);
    }

    template<class QueryReal>
    [[nodiscard]] std::vector<AnnNeighbor> nearest(
        const std::array<QueryReal, Dimension>& query,
        std::size_t k,
        std::size_t search_budget = 64
    ) const {
        return search(query, k, search_budget, std::nullopt);
    }

    [[nodiscard]] std::vector<AnnNeighbor> neighbors_of(
        std::size_t index,
        std::size_t k,
        std::size_t search_budget = 64
    ) const {
        if(index >= points_.size()){
            throw std::out_of_range(
                "NSG point index is out of range"
            );
        }
        return search(points_[index], k, search_budget, index);
    }

private:
    static constexpr std::size_t absent() noexcept {
        return std::numeric_limits<std::size_t>::max();
    }

    void choose_navigation_point() {
        std::array<long double, Dimension> centroid{};
        for(const Point& point : points_){
            for(std::size_t coordinate = 0;
                coordinate < Dimension; ++coordinate){
                centroid[coordinate] +=
                    static_cast<long double>(point[coordinate]);
            }
        }
        for(long double& coordinate : centroid){
            coordinate /= static_cast<long double>(points_.size());
        }
        navigation_ = 0;
        long double best = detail::ann_squared_distance(
            points_[0], centroid
        );
        for(std::size_t index = 1; index < points_.size(); ++index){
            const long double candidate =
                detail::ann_squared_distance(
                    points_[index], centroid
                );
            if(candidate < best){
                navigation_ = index;
                best = candidate;
            }
        }
    }

    void build_diversified_graph() {
        if(points_.size() <= 1) return;
        const std::size_t candidates_per_point = std::min(
            candidate_count_, points_.size() - 1
        );
        for(std::size_t source = 0;
            source < points_.size(); ++source){
            std::vector<AnnNeighbor> candidates;
            candidates.reserve(points_.size() - 1);
            for(std::size_t target = 0;
                target < points_.size(); ++target){
                if(source == target) continue;
                candidates.push_back({
                    target,
                    detail::ann_squared_distance(
                        points_[source], points_[target]
                    )
                });
            }
            std::sort(
                candidates.begin(), candidates.end(),
                detail::neighbor_less
            );
            candidates.resize(candidates_per_point);
            for(const AnnNeighbor& candidate : candidates){
                bool occluded = false;
                for(const std::size_t selected : graph_[source]){
                    if(detail::ann_squared_distance(
                           points_[candidate.index],
                           points_[selected]
                       ) < candidate.squared_distance){
                        occluded = true;
                        break;
                    }
                }
                if(occluded) continue;
                graph_[source].push_back(candidate.index);
                if(graph_[source].size() == out_degree_) break;
            }
            if(graph_[source].empty() && !candidates.empty()){
                graph_[source].push_back(candidates.front().index);
            }
        }
    }

    std::vector<unsigned char> reachable() const {
        std::vector<unsigned char> reached(points_.size(), 0);
        if(navigation_ == absent()) return reached;
        std::vector<std::size_t> stack{navigation_};
        reached[navigation_] = 1;
        while(!stack.empty()){
            const std::size_t current = stack.back();
            stack.pop_back();
            for(const std::size_t next : graph_[current]){
                if(reached[next] != 0) continue;
                reached[next] = 1;
                stack.push_back(next);
            }
        }
        return reached;
    }

    void repair_connectivity() {
        if(points_.empty()) return;
        while(true){
            std::vector<unsigned char> reached = reachable();
            if(std::find(
                   reached.begin(), reached.end(),
                   static_cast<unsigned char>(0)
               ) == reached.end()){
                return;
            }
            std::size_t best_source = absent();
            std::size_t best_target = absent();
            long double best_distance =
                std::numeric_limits<long double>::infinity();
            for(std::size_t source = 0;
                source < points_.size(); ++source){
                if(reached[source] == 0) continue;
                for(std::size_t target = 0;
                    target < points_.size(); ++target){
                    if(reached[target] != 0) continue;
                    const long double distance =
                        detail::ann_squared_distance(
                            points_[source], points_[target]
                        );
                    if(distance < best_distance){
                        best_source = source;
                        best_target = target;
                        best_distance = distance;
                    }
                }
            }
            if(best_source == absent()){
                throw std::logic_error(
                    "NSG connectivity repair made no progress"
                );
            }
            graph_[best_source].push_back(best_target);
        }
    }

    template<class QueryReal>
    std::vector<AnnNeighbor> search(
        const std::array<QueryReal, Dimension>& query,
        std::size_t k,
        std::size_t search_budget,
        std::optional<std::size_t> excluded
    ) const {
        detail::validate_ann_point(query);
        if(k == 0) throw std::invalid_argument("k must be positive");
        const std::size_t available =
            points_.size() - (excluded.has_value() ? 1U : 0U);
        if(k > available){
            throw std::invalid_argument(
                "k exceeds the available NSG point count"
            );
        }
        if(search_budget < k){
            throw std::invalid_argument(
                "NSG search budget must be at least k"
            );
        }
        struct Candidate {
            long double distance = 0.0L;
            std::size_t index = 0;
        };
        const auto compare = [](const Candidate& first, const Candidate& second){
            if(first.distance != second.distance){
                return second.distance < first.distance;
            }
            return second.index < first.index;
        };
        std::priority_queue<
            Candidate, std::vector<Candidate>, decltype(compare)
        > queue(compare);
        std::vector<unsigned char> visited(points_.size(), 0);
        std::vector<AnnNeighbor> evaluated;
        queue.push({
            detail::ann_squared_distance(
                points_[navigation_], query
            ),
            navigation_
        });
        visited[navigation_] = 1;
        std::size_t expanded = 0;
        while(!queue.empty() && expanded < search_budget){
            const Candidate current = queue.top();
            queue.pop();
            if(!excluded.has_value() || current.index != *excluded){
                evaluated.push_back({
                    current.index, current.distance
                });
            }
            for(const std::size_t next : graph_[current.index]){
                if(visited[next] != 0) continue;
                visited[next] = 1;
                queue.push({
                    detail::ann_squared_distance(
                        points_[next], query
                    ),
                    next
                });
            }
            ++expanded;
        }
        if(evaluated.size() < k){
            return detail::exact_knn(
                points_, query, k, excluded
            );
        }
        std::sort(
            evaluated.begin(), evaluated.end(),
            detail::neighbor_less
        );
        evaluated.resize(k);
        return evaluated;
    }

    std::vector<Point> points_;
    std::vector<std::vector<std::size_t>> graph_;
    std::size_t navigation_ = absent();
    std::size_t candidate_count_ = 0;
    std::size_t out_degree_ = 0;
};

template<class Real, std::size_t Dimension>
using NsgIndex = NavigatingSpreadingGraph<Real, Dimension>;

}  // namespace approximate::nearest_neighbor

#endif  // CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_NAVIGATING_SPREADING_GRAPH_HPP_INCLUDED
