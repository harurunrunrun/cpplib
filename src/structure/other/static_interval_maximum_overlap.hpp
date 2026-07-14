#pragma once

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "sparse_table.hpp"
#include "../types/common_monoids.hpp"

template<class Coordinate, int MAX_ENDPOINTS>
class StaticIntervalMaximumOverlap{
    static constexpr MaxMonoid<int> maximum_{};
    std::vector<Coordinate> starts_;
    std::vector<Coordinate> ends_;
    std::vector<Coordinate> coordinates_;
    std::unique_ptr<SparseTable<maximum_, MAX_ENDPOINTS>> table_;

public:
    explicit StaticIntervalMaximumOverlap(
        std::vector<std::pair<Coordinate, Coordinate>> closed_intervals
    ){
        starts_.reserve(closed_intervals.size());
        ends_.reserve(closed_intervals.size());
        coordinates_.reserve(closed_intervals.size() * 2);
        for(auto [left, right]: closed_intervals){
            if(right < left) std::swap(left, right);
            starts_.push_back(left);
            ends_.push_back(right);
            coordinates_.push_back(left);
            coordinates_.push_back(right);
        }
        std::sort(starts_.begin(), starts_.end());
        std::sort(ends_.begin(), ends_.end());
        std::sort(coordinates_.begin(), coordinates_.end());
        coordinates_.erase(
            std::unique(coordinates_.begin(), coordinates_.end()),
            coordinates_.end()
        );
        std::vector<int> overlap;
        overlap.reserve(coordinates_.size());
        for(const Coordinate& coordinate: coordinates_){
            overlap.push_back(
                static_cast<int>(
                    std::upper_bound(starts_.begin(), starts_.end(), coordinate)
                    - starts_.begin()
                )
                - static_cast<int>(
                    std::lower_bound(ends_.begin(), ends_.end(), coordinate)
                    - ends_.begin()
                )
            );
        }
        if(!overlap.empty()){
            table_ = std::make_unique<
                SparseTable<maximum_, MAX_ENDPOINTS>
            >(overlap);
        }
    }

    int interval_count() const{ return static_cast<int>(starts_.size()); }

    int maximum_overlap(Coordinate left, Coordinate right) const{
        if(right < left) std::swap(left, right);
        int answer = static_cast<int>(
            std::upper_bound(starts_.begin(), starts_.end(), left)
            - starts_.begin()
        ) - static_cast<int>(
            std::lower_bound(ends_.begin(), ends_.end(), left)
            - ends_.begin()
        );
        const int first = static_cast<int>(
            std::upper_bound(coordinates_.begin(), coordinates_.end(), left)
            - coordinates_.begin()
        );
        const int last = static_cast<int>(
            std::upper_bound(coordinates_.begin(), coordinates_.end(), right)
            - coordinates_.begin()
        );
        if(first < last) answer = std::max(answer, table_->prod(first, last));
        return answer;
    }
};
