#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DEDUPLICATE_POINTS_3D_HPP
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DEDUPLICATE_POINTS_3D_HPP

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../math/exact_integer.hpp"

#include "base.hpp"
#include "is_finite.hpp"
#include "relative_equal.hpp"

namespace deduplicate_points_3d_detail{

using CellCoordinate = ExactInteger;
using Cell = std::array<CellCoordinate, 3>;

struct CellLess{
    bool operator()(const Cell& left, const Cell& right) const{
        for(std::size_t index = 0; index < 3; ++index){
            if(left[index] != right[index]){
                return left[index] < right[index];
            }
        }
        return false;
    }
};

struct ExactDyadic{
    CellCoordinate significand = 0;
    int exponent = 0;
};

inline ExactDyadic exact_dyadic(long double value){
    if(value == 0.0L) return {};
    const bool negative = std::signbit(value);
    int exponent = 0;
    long double fraction = std::frexp(std::abs(value), &exponent);
    constexpr int digits = std::numeric_limits<long double>::digits;
    CellCoordinate significand = 0;
    for(int bit = 0; bit < digits; ++bit){
        fraction = std::ldexp(fraction, 1);
        significand <<= 1;
        if(fraction >= 1.0L){
            significand += 1;
            fraction -= 1.0L;
        }
    }
    if(negative) significand = -significand;
    return {std::move(significand), exponent - digits};
}

struct PositiveDivision{
    CellCoordinate quotient;
    CellCoordinate remainder;
};

inline PositiveDivision positive_divmod(
    CellCoordinate numerator,
    const CellCoordinate& positive_denominator
){
    if(positive_denominator <= 0)[[unlikely]]{
        throw std::domain_error(
            "3D point cell division requires a positive denominator"
        );
    }
    if(numerator < positive_denominator){
        return {0, std::move(numerator)};
    }
    if(positive_denominator.bit_length()
        <= std::numeric_limits<std::uint64_t>::digits){
        const std::uint64_t divisor =
            positive_denominator.checked_to<std::uint64_t>();
        auto [quotient, remainder] = numerator.divmod(divisor);
        return {std::move(quotient), CellCoordinate(remainder)};
    }

    const std::size_t shift =
        numerator.bit_length() - positive_denominator.bit_length();
    CellCoordinate shifted_denominator = positive_denominator << shift;
    CellCoordinate bit = CellCoordinate(1) << shift;
    CellCoordinate quotient = 0;
    for(std::size_t remaining_shift = shift;; --remaining_shift){
        if(numerator >= shifted_denominator){
            numerator -= shifted_denominator;
            quotient += bit;
        }
        if(remaining_shift == 0) break;
        shifted_denominator >>= 1;
        bit >>= 1;
    }
    return {std::move(quotient), std::move(numerator)};
}

inline CellCoordinate floor_ratio(
    const ExactDyadic& value,
    const ExactDyadic& positive_denominator
){
    if(value.significand == 0) return 0;
    CellCoordinate numerator = value.significand;
    const bool negative = numerator < 0;
    if(negative) numerator = -numerator;
    CellCoordinate denominator = positive_denominator.significand;
    const long long exponent_difference =
        static_cast<long long>(value.exponent)
        - positive_denominator.exponent;
    if(exponent_difference >= 0){
        numerator <<= static_cast<std::size_t>(exponent_difference);
    }else{
        denominator <<= static_cast<std::size_t>(-exponent_difference);
    }
    PositiveDivision division = positive_divmod(
        std::move(numerator), denominator
    );
    if(negative && !division.remainder.is_zero()) division.quotient += 1;
    return negative ? -division.quotient : division.quotient;
}

inline Cell point_cell(const Point3& point, const ExactDyadic& cell_width){
    return {
        floor_ratio(exact_dyadic(point.x), cell_width),
        floor_ratio(exact_dyadic(point.y), cell_width),
        floor_ratio(exact_dyadic(point.z), cell_width),
    };
}

inline bool equivalent(
    const Point3& left,
    const Point3& right,
    long double relative_tolerance,
    long double absolute_tolerance
){
    return relative_equal(
        left.x, right.x, relative_tolerance, absolute_tolerance
    ) && relative_equal(
        left.y, right.y, relative_tolerance, absolute_tolerance
    ) && relative_equal(
        left.z, right.z, relative_tolerance, absolute_tolerance
    );
}

inline long double coordinate(const Point3& point, std::size_t axis){
    if(axis == 0) return point.x;
    if(axis == 1) return point.y;
    return point.z;
}

inline std::vector<Point3> exact_deduplicate(
    const std::vector<Point3>& points
){
    std::vector<std::size_t> order(points.size());
    for(std::size_t index = 0; index < points.size(); ++index){
        order[index] = index;
    }
    std::sort(
        order.begin(), order.end(),
        [&](std::size_t left, std::size_t right){
            if(points[left].x != points[right].x){
                return points[left].x < points[right].x;
            }
            if(points[left].y != points[right].y){
                return points[left].y < points[right].y;
            }
            if(points[left].z != points[right].z){
                return points[left].z < points[right].z;
            }
            return left < right;
        }
    );
    std::vector<unsigned char> keep(points.size(), 0);
    for(std::size_t begin = 0; begin < order.size();){
        keep[order[begin]] = 1;
        std::size_t end = begin + 1;
        while(end < order.size()
            && points[order[begin]].x == points[order[end]].x
            && points[order[begin]].y == points[order[end]].y
            && points[order[begin]].z == points[order[end]].z){
            ++end;
        }
        begin = end;
    }
    std::vector<Point3> result;
    result.reserve(points.size());
    for(std::size_t index = 0; index < points.size(); ++index){
        if(keep[index]) result.push_back(points[index]);
    }
    return result;
}

inline std::vector<Point3> absolute_deduplicate(
    const std::vector<Point3>& points,
    long double absolute_tolerance
){
    const ExactDyadic cell_width = exact_dyadic(absolute_tolerance);
    std::map<Cell, std::size_t, CellLess> representatives;
    std::vector<Point3> result;
    result.reserve(points.size());
    for(const Point3& point: points){
        const Cell cell = point_cell(point, cell_width);
        bool duplicate = false;
        for(int dx = -1; dx <= 1 && !duplicate; ++dx){
            for(int dy = -1; dy <= 1 && !duplicate; ++dy){
                for(int dz = -1; dz <= 1; ++dz){
                    Cell neighbor = cell;
                    neighbor[0] += dx;
                    neighbor[1] += dy;
                    neighbor[2] += dz;
                    const auto iterator = representatives.find(neighbor);
                    if(iterator != representatives.end()
                        && equivalent(
                            point, result[iterator->second],
                            0.0L, absolute_tolerance
                        )){
                        duplicate = true;
                        break;
                    }
                }
            }
        }
        if(!duplicate){
            representatives.emplace(cell, result.size());
            result.push_back(point);
        }
    }
    return result;
}

inline std::vector<Point3> quadratic_deduplicate(
    const std::vector<Point3>& points,
    long double relative_tolerance,
    long double absolute_tolerance
){
    std::vector<Point3> result;
    result.reserve(points.size());
    for(const Point3& point: points){
        bool duplicate = false;
        for(const Point3& representative: result){
            if(equivalent(
                point, representative,
                relative_tolerance, absolute_tolerance
            )){
                duplicate = true;
                break;
            }
        }
        if(!duplicate) result.push_back(point);
    }
    return result;
}

using RankInterval = std::pair<std::size_t, std::size_t>;

inline bool relative_bound_overflows(
    long double fixed,
    long double candidate,
    long double relative_tolerance
){
    const long double scale =
        std::max(std::abs(fixed), std::abs(candidate));
    return !geometry3d_is_finite(relative_tolerance * scale);
}

inline void add_exact_coordinate_intervals(
    std::vector<RankInterval>& intervals,
    const std::vector<long double>& values,
    long double fixed,
    long double relative_tolerance,
    long double absolute_tolerance
){
    const auto add_monotone = [&](
        std::size_t begin,
        std::size_t end,
        const auto& matches,
        bool nondecreasing
    ){
        if(begin >= end) return;
        std::size_t first = begin;
        std::size_t last = end;
        if(nondecreasing){
            while(first < last){
                const std::size_t middle = first + (last - first) / 2;
                if(matches(middle)) last = middle;
                else first = middle + 1;
            }
            if(first < end) intervals.emplace_back(first, end);
        }else{
            while(first < last){
                const std::size_t middle = first + (last - first) / 2;
                if(matches(middle)) first = middle + 1;
                else last = middle;
            }
            if(begin < first) intervals.emplace_back(begin, first);
        }
    };

    const auto absolute_matches = [&](std::size_t index){
        return relative_equal(
            fixed, values[index], 0.0L, absolute_tolerance
        );
    };
    const std::size_t fixed_rank = static_cast<std::size_t>(
        std::lower_bound(values.begin(), values.end(), fixed)
        - values.begin()
    );
    add_monotone(0, fixed_rank, absolute_matches, true);
    add_monotone(
        fixed_rank, values.size(), absolute_matches, false
    );

    const std::size_t negative_end = static_cast<std::size_t>(
        std::lower_bound(values.begin(), values.end(), 0.0L)
        - values.begin()
    );
    const auto bound_overflows = [&](std::size_t index){
        return relative_bound_overflows(
            fixed, values[index], relative_tolerance
        );
    };
    std::size_t finite_begin = 0;
    {
        std::size_t first = 0;
        std::size_t last = negative_end;
        while(first < last){
            const std::size_t middle = first + (last - first) / 2;
            if(bound_overflows(middle)) first = middle + 1;
            else last = middle;
        }
        finite_begin = first;
    }
    std::size_t finite_end = values.size();
    {
        std::size_t first = negative_end;
        std::size_t last = values.size();
        while(first < last){
            const std::size_t middle = first + (last - first) / 2;
            if(bound_overflows(middle)) last = middle;
            else first = middle + 1;
        }
        finite_end = first;
    }
    if(finite_begin > 0) intervals.emplace_back(0, finite_begin);
    if(finite_end < values.size()){
        intervals.emplace_back(finite_end, values.size());
    }

    const long double magnitude = std::abs(fixed);
    const std::size_t left_split = std::clamp(
        static_cast<std::size_t>(
            std::lower_bound(values.begin(), values.end(), -magnitude)
            - values.begin()
        ),
        finite_begin, finite_end
    );
    const std::size_t right_split = std::clamp(
        static_cast<std::size_t>(
            std::upper_bound(values.begin(), values.end(), magnitude)
            - values.begin()
        ),
        finite_begin, finite_end
    );
    const auto relative_matches = [&](std::size_t index){
        return relative_equal(
            fixed, values[index], relative_tolerance, 0.0L
        );
    };
    const bool fixed_is_negative = fixed < 0.0L;
    add_monotone(
        finite_begin, left_split,
        relative_matches, fixed_is_negative
    );
    add_monotone(
        left_split, right_split,
        relative_matches, !fixed_is_negative
    );
    add_monotone(
        right_split, finite_end,
        relative_matches, fixed_is_negative
    );
}

inline std::vector<RankInterval> coordinate_intervals(
    const std::vector<long double>& values,
    long double fixed,
    long double relative_tolerance,
    long double absolute_tolerance
){
    if(values.empty()) return {};
    if(relative_bound_overflows(
        fixed, fixed, relative_tolerance
    )){
        return {{0, values.size()}};
    }

    std::vector<RankInterval> intervals;
    intervals.reserve(7);
    add_exact_coordinate_intervals(
        intervals, values, fixed,
        relative_tolerance, absolute_tolerance
    );

    std::sort(intervals.begin(), intervals.end());
    std::vector<RankInterval> merged;
    for(const RankInterval interval: intervals){
        if(interval.first >= interval.second) continue;
        if(merged.empty() || merged.back().second < interval.first){
            merged.push_back(interval);
        }else{
            merged.back().second =
                std::max(merged.back().second, interval.second);
        }
    }
    return merged;
}

struct RankedPoint3{
    std::size_t x = 0;
    std::size_t y = 0;
    std::size_t z = 0;
};

class Fenwick2D{
    std::vector<std::pair<std::size_t, std::size_t>> registered_;
    std::vector<std::size_t> y_values_;
    std::vector<std::vector<std::size_t>> z_values_;
    std::vector<std::vector<std::size_t>> tree_;

    static std::size_t lowbit(std::size_t value){
        return value & (~value + 1);
    }

public:
    void register_point(std::size_t y, std::size_t z){
        registered_.emplace_back(y, z);
    }

    void build(){
        y_values_.reserve(registered_.size());
        for(const auto [y, z]: registered_){
            static_cast<void>(z);
            y_values_.push_back(y);
        }
        std::sort(y_values_.begin(), y_values_.end());
        y_values_.erase(
            std::unique(y_values_.begin(), y_values_.end()),
            y_values_.end()
        );
        z_values_.resize(y_values_.size() + 1);
        for(const auto [y, z]: registered_){
            std::size_t index = static_cast<std::size_t>(
                std::lower_bound(y_values_.begin(), y_values_.end(), y)
                - y_values_.begin()
            ) + 1;
            for(; index < z_values_.size(); index += lowbit(index)){
                z_values_[index].push_back(z);
            }
        }
        tree_.resize(z_values_.size());
        for(std::size_t index = 1; index < z_values_.size(); ++index){
            auto& values = z_values_[index];
            std::sort(values.begin(), values.end());
            values.erase(std::unique(values.begin(), values.end()), values.end());
            tree_[index].assign(values.size() + 1, 0);
        }
        registered_.clear();
        registered_.shrink_to_fit();
    }

    void add(std::size_t y, std::size_t z){
        std::size_t y_index = static_cast<std::size_t>(
            std::lower_bound(y_values_.begin(), y_values_.end(), y)
            - y_values_.begin()
        ) + 1;
        for(; y_index < z_values_.size(); y_index += lowbit(y_index)){
            const auto& values = z_values_[y_index];
            std::size_t z_index = static_cast<std::size_t>(
                std::lower_bound(values.begin(), values.end(), z)
                - values.begin()
            ) + 1;
            auto& tree = tree_[y_index];
            for(; z_index < tree.size(); z_index += lowbit(z_index)){
                ++tree[z_index];
            }
        }
    }

    std::size_t prefix(
        std::size_t y_count,
        std::size_t z_count
    ) const{
        std::size_t y_index = static_cast<std::size_t>(
            std::lower_bound(
                y_values_.begin(), y_values_.end(), y_count
            ) - y_values_.begin()
        );
        std::size_t result = 0;
        for(; y_index > 0; y_index -= lowbit(y_index)){
            const auto& values = z_values_[y_index];
            std::size_t z_index = static_cast<std::size_t>(
                std::lower_bound(values.begin(), values.end(), z_count)
                - values.begin()
            );
            const auto& tree = tree_[y_index];
            for(; z_index > 0; z_index -= lowbit(z_index)){
                result += tree[z_index];
            }
        }
        return result;
    }
};

class DynamicOrthogonalRangeEmptiness3D{
    std::vector<Fenwick2D> outer_;

    static std::size_t lowbit(std::size_t value){
        return value & (~value + 1);
    }

    std::size_t prefix(
        std::size_t x_count,
        std::size_t y_count,
        std::size_t z_count
    ) const{
        std::size_t result = 0;
        for(std::size_t index = x_count;
            index > 0; index -= lowbit(index)){
            result += outer_[index].prefix(y_count, z_count);
        }
        return result;
    }

public:
    DynamicOrthogonalRangeEmptiness3D(
        const std::vector<RankedPoint3>& points,
        std::size_t x_size
    ): outer_(x_size + 1){
        for(const RankedPoint3 point: points){
            for(std::size_t index = point.x + 1;
                index < outer_.size(); index += lowbit(index)){
                outer_[index].register_point(point.y, point.z);
            }
        }
        for(std::size_t index = 1; index < outer_.size(); ++index){
            outer_[index].build();
        }
    }

    void add(const RankedPoint3& point){
        for(std::size_t index = point.x + 1;
            index < outer_.size(); index += lowbit(index)){
            outer_[index].add(point.y, point.z);
        }
    }

    bool any(
        const RankInterval& x,
        const RankInterval& y,
        const RankInterval& z
    ) const{
        const std::size_t positive =
            prefix(x.second, y.second, z.second)
            + prefix(x.first, y.first, z.second)
            + prefix(x.first, y.second, z.first)
            + prefix(x.second, y.first, z.first);
        const std::size_t negative =
            prefix(x.first, y.second, z.second)
            + prefix(x.second, y.first, z.second)
            + prefix(x.second, y.second, z.first)
            + prefix(x.first, y.first, z.first);
        return positive > negative;
    }
};

}  // namespace deduplicate_points_3d_detail

inline std::vector<Point3> deduplicate_points_3d(
    const std::vector<Point3>& points,
    long double relative_tolerance = 0.0L,
    long double absolute_tolerance = GEOMETRY3D_EPS
){
    if(!geometry3d_is_finite(relative_tolerance)
        || !geometry3d_is_finite(absolute_tolerance)
        || relative_tolerance < 0 || absolute_tolerance < 0)[[unlikely]]{
        throw std::invalid_argument("invalid 3D point deduplication tolerance");
    }
    for(const Point3& point: points){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("non-finite point in 3D point set");
        }
    }
    if(relative_tolerance != 0.0L){
        return deduplicate_points_3d_detail::quadratic_deduplicate(
            points, relative_tolerance, absolute_tolerance
        );
    }
    if(absolute_tolerance == 0.0L){
        return deduplicate_points_3d_detail::exact_deduplicate(points);
    }
    return deduplicate_points_3d_detail::absolute_deduplicate(
        points, absolute_tolerance
    );
}

inline std::vector<Point3> deduplicate_points_3d_relative_range_tree(
    const std::vector<Point3>& points,
    long double relative_tolerance,
    long double absolute_tolerance = GEOMETRY3D_EPS
){
    if(!geometry3d_is_finite(relative_tolerance)
        || !geometry3d_is_finite(absolute_tolerance)
        || relative_tolerance <= 0.0L
        || absolute_tolerance < 0.0L)[[unlikely]]{
        throw std::invalid_argument(
            "invalid relative 3D point deduplication tolerance"
        );
    }
    for(const Point3& point: points){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("non-finite point in 3D point set");
        }
    }
    if(points.empty()) return {};

    std::array<std::vector<long double>, 3> coordinates;
    for(const Point3& point: points){
        coordinates[0].push_back(point.x);
        coordinates[1].push_back(point.y);
        coordinates[2].push_back(point.z);
    }
    for(auto& values: coordinates){
        std::sort(values.begin(), values.end());
        values.erase(std::unique(values.begin(), values.end()), values.end());
    }

    std::vector<deduplicate_points_3d_detail::RankedPoint3> ranked;
    ranked.reserve(points.size());
    for(const Point3& point: points){
        ranked.push_back({
            static_cast<std::size_t>(
                std::lower_bound(
                    coordinates[0].begin(), coordinates[0].end(), point.x
                ) - coordinates[0].begin()
            ),
            static_cast<std::size_t>(
                std::lower_bound(
                    coordinates[1].begin(), coordinates[1].end(), point.y
                ) - coordinates[1].begin()
            ),
            static_cast<std::size_t>(
                std::lower_bound(
                    coordinates[2].begin(), coordinates[2].end(), point.z
                ) - coordinates[2].begin()
            ),
        });
    }

    deduplicate_points_3d_detail::DynamicOrthogonalRangeEmptiness3D index(
        ranked, coordinates[0].size()
    );
    std::vector<Point3> result;
    result.reserve(points.size());
    for(std::size_t point_index = 0;
        point_index < points.size(); ++point_index){
        std::array<std::vector<deduplicate_points_3d_detail::RankInterval>, 3>
            intervals;
        for(std::size_t axis = 0; axis < 3; ++axis){
            intervals[axis] =
                deduplicate_points_3d_detail::coordinate_intervals(
                    coordinates[axis],
                    deduplicate_points_3d_detail::coordinate(
                        points[point_index], axis
                    ),
                    relative_tolerance, absolute_tolerance
                );
        }
        bool duplicate = false;
        for(const auto& x: intervals[0]){
            for(const auto& y: intervals[1]){
                for(const auto& z: intervals[2]){
                    if(index.any(x, y, z)){
                        duplicate = true;
                        break;
                    }
                }
                if(duplicate) break;
            }
            if(duplicate) break;
        }
        if(!duplicate){
            result.push_back(points[point_index]);
            index.add(ranked[point_index]);
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_DEDUPLICATE_POINTS_3D_HPP
