#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_ORTHOGONAL_RANGE_REPORTING_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_ORTHOGONAL_RANGE_REPORTING_HPP_INCLUDED

#include <algorithm>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

template<class Coordinate>
class OrthogonalRangeReporting{
    struct Point{
        Coordinate x;
        Coordinate y;
        int input_index;
    };
    struct YEntry{
        Coordinate y;
        int input_index;
    };

    std::vector<Point> points_;
    std::vector<std::vector<YEntry>> tree_;
    std::size_t leaf_count_ = 1;

    static bool y_less(const YEntry& left, const YEntry& right){
        if(left.y < right.y) return true;
        if(right.y < left.y) return false;
        return left.input_index < right.input_index;
    }

    void append(
        std::size_t node,
        const Coordinate& lower_y,
        const Coordinate& upper_y,
        std::vector<int>& result
    ) const {
        const auto& values = tree_[node];
        const auto first = std::lower_bound(
            values.begin(), values.end(), lower_y,
            [](const YEntry& entry, const Coordinate& bound){
                return entry.y < bound;
            }
        );
        const auto last = std::lower_bound(
            values.begin(), values.end(), upper_y,
            [](const YEntry& entry, const Coordinate& bound){
                return entry.y < bound;
            }
        );
        for(auto iterator = first; iterator != last; ++iterator){
            result.push_back(iterator->input_index);
        }
    }

public:
    OrthogonalRangeReporting() = default;

    explicit OrthogonalRangeReporting(
        const std::vector<std::pair<Coordinate, Coordinate>>& points
    ){
        if(points.size() > static_cast<std::size_t>(
            std::numeric_limits<int>::max()
        ))[[unlikely]]{
            throw std::length_error(
                "too many points (OrthogonalRangeReporting)"
            );
        }
        points_.reserve(points.size());
        for(int index = 0; index < static_cast<int>(points.size()); ++index){
            points_.push_back({
                points[static_cast<std::size_t>(index)].first,
                points[static_cast<std::size_t>(index)].second,
                index
            });
        }
        std::sort(points_.begin(), points_.end(),
            [](const Point& left, const Point& right){
                if(left.x < right.x) return true;
                if(right.x < left.x) return false;
                if(left.y < right.y) return true;
                if(right.y < left.y) return false;
                return left.input_index < right.input_index;
            }
        );
        while(leaf_count_ < points_.size()) leaf_count_ *= 2;
        tree_.resize(leaf_count_ * 2);
        for(std::size_t index = 0; index < points_.size(); ++index){
            tree_[leaf_count_ + index].push_back({
                points_[index].y, points_[index].input_index
            });
        }
        for(std::size_t node = leaf_count_; node-- > 1; ){
            auto& destination = tree_[node];
            const auto& left = tree_[node * 2];
            const auto& right = tree_[node * 2 + 1];
            destination.reserve(left.size() + right.size());
            std::merge(
                left.begin(), left.end(), right.begin(), right.end(),
                std::back_inserter(destination), y_less
            );
        }
    }

    int size() const noexcept {
        return static_cast<int>(points_.size());
    }

    std::vector<int> report(
        const Coordinate& lower_x,
        const Coordinate& upper_x,
        const Coordinate& lower_y,
        const Coordinate& upper_y
    ) const {
        if(upper_x < lower_x || upper_y < lower_y)[[unlikely]]{
            throw std::invalid_argument(
                "OrthogonalRangeReporting: invalid rectangle"
            );
        }
        std::size_t left = static_cast<std::size_t>(
            std::lower_bound(
                points_.begin(), points_.end(), lower_x,
                [](const Point& point, const Coordinate& bound){
                    return point.x < bound;
                }
            ) - points_.begin()
        );
        std::size_t right = static_cast<std::size_t>(
            std::lower_bound(
                points_.begin(), points_.end(), upper_x,
                [](const Point& point, const Coordinate& bound){
                    return point.x < bound;
                }
            ) - points_.begin()
        );
        left += leaf_count_;
        right += leaf_count_;
        std::vector<int> result;
        while(left < right){
            if((left & 1U) != 0U) append(left++, lower_y, upper_y, result);
            if((right & 1U) != 0U) append(--right, lower_y, upper_y, result);
            left /= 2;
            right /= 2;
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_VALUE_QUERY_ORTHOGONAL_RANGE_REPORTING_HPP_INCLUDED
