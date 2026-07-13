#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <queue>
#include <random>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ann_common.hpp"

namespace approximate::nearest_neighbor {

template<class Real, std::size_t Dimension>
class NnDescent {
    static_assert(Dimension > 0, "Dimension must be positive");

public:
    using Point = std::array<Real, Dimension>;

    NnDescent() = default;

    template<class RandomEngine>
    NnDescent(std::vector<Point> points, std::size_t neighbor_count,
              std::size_t iterations, std::size_t candidate_limit,
              RandomEngine& random) {
        reset(std::move(points), neighbor_count, iterations, candidate_limit, random);
    }

    template<class RandomEngine>
    void reset(std::vector<Point> points, std::size_t neighbor_count,
               std::size_t iterations, std::size_t candidate_limit,
               RandomEngine& random) {
        if(iterations == 0 || candidate_limit == 0){
            throw std::invalid_argument("limits must be positive");
        }
        if(points.size() < 2){
            if(neighbor_count != 0){
                throw std::invalid_argument("neighbor_count must be zero");
            }
        }else if(neighbor_count == 0 || neighbor_count >= points.size()){
            throw std::invalid_argument("invalid neighbor_count");
        }
        if(candidate_limit < neighbor_count){
            throw std::invalid_argument("candidate_limit is too small");
        }
        for(const Point& point: points) detail::validate_ann_point(point);

        std::vector<std::vector<AnnNeighbor>> graph(points.size());
        const auto make_neighbor = [&](std::size_t from, std::size_t to) {
            return AnnNeighbor{
                to, detail::ann_squared_distance(points[from], points[to])};
        };

        for(std::size_t index = 0; index < points.size(); ++index){
            const std::size_t population = points.size() - 1;
            std::unordered_set<std::size_t> chosen;
            chosen.reserve(neighbor_count);
            for(std::size_t offset = 0; offset < neighbor_count; ++offset){
                const std::size_t upper = population - neighbor_count + offset;
                std::uniform_int_distribution<std::size_t> distribution(0, upper);
                const std::size_t sampled = distribution(random);
                const std::size_t rank = chosen.contains(sampled) ? upper : sampled;
                chosen.insert(rank);
                const std::size_t neighbor = rank < index ? rank : rank + 1;
                graph[index].push_back(make_neighbor(index, neighbor));
            }
            std::sort(graph[index].begin(), graph[index].end(), detail::neighbor_less);
        }

        for(std::size_t step = 0; step < iterations; ++step){
            std::vector<std::vector<std::size_t>> reverse(points.size());
            for(std::size_t index = 0; index < graph.size(); ++index){
                for(const AnnNeighbor& edge: graph[index]){
                    reverse[edge.index].push_back(index);
                }
            }

            bool changed = false;
            auto next = graph;
            for(std::size_t index = 0; index < graph.size(); ++index){
                std::vector<std::size_t> candidates = reverse[index];
                for(const AnnNeighbor& edge: graph[index]){
                    candidates.push_back(edge.index);
                    for(const AnnNeighbor& second_edge: graph[edge.index]){
                        candidates.push_back(second_edge.index);
                    }
                }
                detail::sort_unique_indices(candidates);
                candidates.erase(
                    std::remove(candidates.begin(), candidates.end(), index),
                    candidates.end());
                if(candidates.size() > candidate_limit){
                    std::shuffle(candidates.begin(), candidates.end(), random);
                    candidates.resize(candidate_limit);
                }
                for(const AnnNeighbor& old: graph[index]){
                    candidates.push_back(old.index);
                }
                detail::sort_unique_indices(candidates);

                std::vector<AnnNeighbor> ranked;
                ranked.reserve(candidates.size());
                for(std::size_t candidate: candidates){
                    ranked.push_back(make_neighbor(index, candidate));
                }
                std::sort(ranked.begin(), ranked.end(), detail::neighbor_less);
                if(ranked.size() > neighbor_count) ranked.resize(neighbor_count);
                if(!same_indices(ranked, graph[index])) changed = true;
                next[index] = std::move(ranked);
            }
            graph = std::move(next);
            if(!changed) break;
        }

        points_ = std::move(points);
        graph_ = std::move(graph);
        neighbor_count_ = neighbor_count;
    }

    [[nodiscard]] std::size_t size() const noexcept { return points_.size(); }
    [[nodiscard]] std::size_t neighbor_count() const noexcept {
        return neighbor_count_;
    }
    [[nodiscard]] const Point& point(std::size_t index) const {
        return points_.at(index);
    }
    [[nodiscard]] const std::vector<AnnNeighbor>& neighbors(std::size_t index) const {
        return graph_.at(index);
    }

    template<class QueryReal>
    [[nodiscard]] std::vector<AnnNeighbor> nearest(
        const std::array<QueryReal, Dimension>& query, std::size_t k,
        std::size_t budget, bool exact_fallback = false) const {
        detail::validate_ann_point(query);
        if(k == 0 || k > points_.size()) throw std::invalid_argument("invalid k");
        if(budget < k){
            throw std::invalid_argument("search_budget must be at least k");
        }
        if(exact_fallback) return detail::exact_knn(points_, query, k);
        budget = std::min(budget, points_.size());

        struct Candidate {
            long double distance;
            std::size_t index;
        };
        const auto compare = [](const Candidate& first, const Candidate& second) {
            return first.distance > second.distance
                || (first.distance == second.distance && first.index > second.index);
        };
        std::priority_queue<Candidate, std::vector<Candidate>, decltype(compare)> queue(
            compare);
        std::unordered_set<std::size_t> seen;
        seen.reserve(budget);
        std::vector<AnnNeighbor> evaluated;
        evaluated.reserve(budget);
        const auto push = [&](std::size_t index) {
            if(seen.contains(index) || evaluated.size() == budget) return;
            seen.insert(index);
            const long double distance =
                detail::ann_squared_distance(points_[index], query);
            queue.push({distance, index});
            evaluated.push_back({index, distance});
        };

        push(0);
        std::size_t restart_cursor = 0;
        while(evaluated.size() < budget){
            if(queue.empty()){
                while(seen.contains(restart_cursor)) ++restart_cursor;
                push(restart_cursor);
            }
            const Candidate current = queue.top();
            queue.pop();
            for(const AnnNeighbor& edge: graph_[current.index]){
                push(edge.index);
                if(evaluated.size() == budget) break;
            }
        }
        std::sort(evaluated.begin(), evaluated.end(), detail::neighbor_less);
        evaluated.resize(k);
        return evaluated;
    }

private:
    static bool same_indices(const std::vector<AnnNeighbor>& first,
                             const std::vector<AnnNeighbor>& second) {
        if(first.size() != second.size()) return false;
        for(std::size_t index = 0; index < first.size(); ++index){
            if(first[index].index != second[index].index) return false;
        }
        return true;
    }

    std::vector<Point> points_;
    std::vector<std::vector<AnnNeighbor>> graph_;
    std::size_t neighbor_count_ = 0;
};

}  // namespace approximate::nearest_neighbor
