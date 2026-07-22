#ifndef CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_OFFLINE_SEGMENT_ADD_GET_MIN_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_OFFLINE_SEGMENT_ADD_GET_MIN_HPP_INCLUDED

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <vector>

#include "segment_li_chao_tree.hpp"

class OfflineSegmentAddGetMin{
    struct Segment{
        long long left;
        long long right;
        long long slope;
        long long intercept;
    };
    struct Operation{
        bool is_query;
        Segment segment;
        long long x;
    };

    std::vector<Segment> initial_segments_;
    std::vector<Operation> operations_;
    std::size_t query_count_ = 0;

    static Segment checked_segment(
        long long left,
        long long right,
        long long slope,
        long long intercept
    ){
        if(right < left)[[unlikely]]{
            throw std::invalid_argument(
                "OfflineSegmentAddGetMin segment range is reversed"
            );
        }
        return {left, right, slope, intercept};
    }

public:
    void add_initial_segment(
        long long left,
        long long right,
        long long slope,
        long long intercept
    ){
        initial_segments_.push_back(
            checked_segment(left, right, slope, intercept)
        );
    }

    void add_segment(
        long long left,
        long long right,
        long long slope,
        long long intercept
    ){
        operations_.push_back({
            false,
            checked_segment(left, right, slope, intercept),
            0
        });
    }

    std::size_t get_min(long long x){
        const std::size_t index = query_count_++;
        operations_.push_back({true, {}, x});
        return index;
    }

    std::size_t query_count() const noexcept{ return query_count_; }

    std::vector<std::optional<long long>> solve() const{
        std::vector<long long> coordinates;
        coordinates.reserve(query_count_);
        for(const Operation& operation: operations_){
            if(operation.is_query) coordinates.push_back(operation.x);
        }

        SegmentLiChaoTree tree(std::move(coordinates));
        for(const Segment& segment: initial_segments_){
            tree.add_segment(
                segment.left,
                segment.right,
                segment.slope,
                segment.intercept
            );
        }

        std::vector<std::optional<long long>> answer;
        answer.reserve(query_count_);
        for(const Operation& operation: operations_){
            if(operation.is_query){
                answer.push_back(tree.query(operation.x));
            }else{
                const Segment& segment = operation.segment;
                tree.add_segment(
                    segment.left,
                    segment.right,
                    segment.slope,
                    segment.intercept
                );
            }
        }
        return answer;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_OFFLINE_SEGMENT_ADD_GET_MIN_HPP_INCLUDED
