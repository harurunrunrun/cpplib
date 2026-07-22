#ifndef CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_HNSW_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_HNSW_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <queue>
#include <random>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ann_common.hpp"

namespace approximate::nearest_neighbor {

template<class Real, std::size_t Dimension>
class HnswIndex {
    static_assert(Dimension > 0, "Dimension must be positive");

public:
    using Point = std::array<Real, Dimension>;

    explicit HnswIndex(std::size_t max_neighbors = 16,
                       std::size_t ef_construction = 200,
                       std::uint64_t seed = 0)
        : max_neighbors_(max_neighbors),
          ef_construction_(ef_construction),
          random_(seed) {
        if(max_neighbors < 2){
            throw std::invalid_argument("max_neighbors must be at least two");
        }
        if(ef_construction < max_neighbors){
            throw std::invalid_argument("ef_construction is too small");
        }
    }

    [[nodiscard]] std::size_t size() const noexcept { return nodes_.size(); }
    [[nodiscard]] bool empty() const noexcept { return nodes_.empty(); }
    [[nodiscard]] const Point& point(std::size_t index) const {
        return nodes_.at(index).point;
    }

    void clear() noexcept {
        nodes_.clear();
        entry_ = absent();
        max_level_ = 0;
    }

    std::size_t insert(const Point& point) {
        detail::validate_ann_point(point);
        const std::size_t level = random_level();
        const std::size_t index = nodes_.size();
        nodes_.push_back(
            Node{point, std::vector<std::vector<std::size_t>>(level + 1)});
        if(index == 0){
            entry_ = 0;
            max_level_ = level;
            return 0;
        }

        struct LinkBackup {
            std::size_t index;
            std::size_t level;
            std::vector<std::size_t> links;
        };
        std::vector<LinkBackup> backups;
        try {
            std::size_t current = entry_;
            for(std::size_t current_level = max_level_; current_level > level;
                --current_level){
                const auto found = search_layer(point, {current}, 1, current_level);
                current = found.front().index;
            }

            const std::size_t first_level = std::min(level, max_level_);
            for(std::size_t offset = 0; offset <= first_level; ++offset){
                const std::size_t current_level = first_level - offset;
                auto found =
                    search_layer(point, {current}, ef_construction_, current_level);
                if(found.size() > max_neighbors_) found.resize(max_neighbors_);
                for(const AnnNeighbor& neighbor: found){
                    backups.push_back({neighbor.index, current_level,
                                       nodes_[neighbor.index].links[current_level]});
                    nodes_[index].links[current_level].push_back(neighbor.index);
                    nodes_[neighbor.index].links[current_level].push_back(index);
                    prune(neighbor.index, current_level);
                }
                if(!found.empty()) current = found.front().index;
            }
        }catch(...){
            for(auto backup = backups.rbegin(); backup != backups.rend(); ++backup){
                nodes_[backup->index].links[backup->level] = std::move(backup->links);
            }
            nodes_.pop_back();
            throw;
        }

        if(level > max_level_){
            entry_ = index;
            max_level_ = level;
        }
        return index;
    }

    template<class QueryReal>
    [[nodiscard]] std::vector<AnnNeighbor> nearest(
        const std::array<QueryReal, Dimension>& query, std::size_t k,
        std::size_t ef_search, bool exact_fallback = false) const {
        detail::validate_ann_point(query);
        if(k == 0 || k > nodes_.size()) throw std::invalid_argument("invalid k");
        if(ef_search < k){
            throw std::invalid_argument("ef_search must be at least k");
        }
        if(exact_fallback) return exact(query, k, std::nullopt);

        std::size_t current = entry_;
        for(std::size_t level = max_level_; level > 0; --level){
            current = search_layer(query, {current}, 1, level).front().index;
        }
        auto result = search_layer(query, {current}, ef_search, 0);
        if(result.size() < k) return exact(query, k, std::nullopt);
        if(result.size() > k) result.resize(k);
        return result;
    }

    [[nodiscard]] std::vector<AnnNeighbor> neighbors_of(
        std::size_t index, std::size_t k, std::size_t ef_search,
        bool exact_fallback = false) const {
        if(index >= nodes_.size()){
            throw std::out_of_range("point index is out of range");
        }
        if(k == 0 || k >= nodes_.size()) throw std::invalid_argument("invalid k");
        if(ef_search <= k){
            throw std::invalid_argument("ef_search must exceed k");
        }
        if(exact_fallback) return exact(nodes_[index].point, k, index);

        auto result = nearest(
            nodes_[index].point, std::min(k + 1, nodes_.size()), ef_search, false);
        result.erase(std::remove_if(result.begin(), result.end(),
            [&](const AnnNeighbor& neighbor) { return neighbor.index == index; }),
            result.end());
        if(result.size() > k) result.resize(k);
        return result;
    }

private:
    static constexpr std::size_t absent() noexcept {
        return std::numeric_limits<std::size_t>::max();
    }

    struct Node {
        Point point;
        std::vector<std::vector<std::size_t>> links;
    };

    std::size_t random_level() {
        std::size_t level = 0;
        std::uniform_int_distribution<std::size_t> distribution(
            0, max_neighbors_ - 1);
        while(level < 32 && distribution(random_) == 0) ++level;
        return level;
    }

    template<class QueryReal>
    std::vector<AnnNeighbor> search_layer(
        const std::array<QueryReal, Dimension>& query,
        const std::vector<std::size_t>& entries, std::size_t ef,
        std::size_t level) const {
        const auto closer = [](const AnnNeighbor& first, const AnnNeighbor& second) {
            return detail::neighbor_less(second, first);
        };
        const auto farther = [](const AnnNeighbor& first, const AnnNeighbor& second) {
            return detail::neighbor_less(first, second);
        };
        std::priority_queue<AnnNeighbor, std::vector<AnnNeighbor>, decltype(closer)>
            candidates(closer);
        std::priority_queue<AnnNeighbor, std::vector<AnnNeighbor>, decltype(farther)>
            best(farther);

        std::unordered_set<std::size_t> seen;
        seen.reserve(entries.size());
        for(std::size_t entry: entries){
            const AnnNeighbor value{
                entry, detail::ann_squared_distance(nodes_[entry].point, query)};
            if(seen.insert(entry).second){
                candidates.push(value);
                best.push(value);
            }
        }

        while(!candidates.empty()){
            const AnnNeighbor current = candidates.top();
            candidates.pop();
            if(best.size() >= ef && detail::neighbor_less(best.top(), current)) break;
            if(level >= nodes_[current.index].links.size()) continue;
            for(std::size_t next: nodes_[current.index].links[level]){
                if(!seen.insert(next).second) continue;
                const AnnNeighbor value{
                    next, detail::ann_squared_distance(nodes_[next].point, query)};
                if(best.size() < ef || detail::neighbor_less(value, best.top())){
                    candidates.push(value);
                    best.push(value);
                    if(best.size() > ef) best.pop();
                }
            }
        }

        std::vector<AnnNeighbor> result;
        result.reserve(best.size());
        while(!best.empty()){
            result.push_back(best.top());
            best.pop();
        }
        std::sort(result.begin(), result.end(), detail::neighbor_less);
        return result;
    }

    void prune(std::size_t index, std::size_t level) {
        auto& links = nodes_[index].links[level];
        detail::sort_unique_indices(links);
        std::vector<AnnNeighbor> ranked;
        ranked.reserve(links.size());
        for(std::size_t next: links){
            if(next != index){
                ranked.push_back({
                    next,
                    detail::ann_squared_distance(nodes_[index].point,
                                                 nodes_[next].point)});
            }
        }
        std::sort(ranked.begin(), ranked.end(), detail::neighbor_less);
        if(ranked.size() > max_neighbors_) ranked.resize(max_neighbors_);
        links.clear();
        links.reserve(ranked.size());
        for(const AnnNeighbor& neighbor: ranked) links.push_back(neighbor.index);
    }

    template<class QueryReal>
    std::vector<AnnNeighbor> exact(
        const std::array<QueryReal, Dimension>& query, std::size_t k,
        std::optional<std::size_t> excluded) const {
        std::vector<AnnNeighbor> result;
        result.reserve(nodes_.size() - (excluded.has_value() ? 1U : 0U));
        for(std::size_t index = 0; index < nodes_.size(); ++index){
            if(excluded.has_value() && index == *excluded) continue;
            result.push_back({
                index, detail::ann_squared_distance(nodes_[index].point, query)});
        }
        std::sort(result.begin(), result.end(), detail::neighbor_less);
        result.resize(k);
        return result;
    }

    std::size_t max_neighbors_;
    std::size_t ef_construction_;
    std::mt19937_64 random_;
    std::vector<Node> nodes_;
    std::size_t entry_ = absent();
    std::size_t max_level_ = 0;
};

}  // namespace approximate::nearest_neighbor

#endif  // CPPLIB_SRC_APPROXIMATE_NEAREST_NEIGHBOR_HNSW_HPP_INCLUDED
