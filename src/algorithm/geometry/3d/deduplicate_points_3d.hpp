#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include <boost/multiprecision/cpp_int.hpp>

#include "base.hpp"
#include "is_finite.hpp"
#include "relative_equal.hpp"

namespace deduplicate_points_3d_detail{

using CellCoordinate = boost::multiprecision::cpp_int;
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
            ++significand;
            fraction -= 1.0L;
        }
    }
    if(negative) significand = -significand;
    return {std::move(significand), exponent - digits};
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
    const int exponent_difference =
        value.exponent - positive_denominator.exponent;
    if(exponent_difference >= 0){
        numerator <<= static_cast<unsigned>(exponent_difference);
    }else{
        denominator <<= static_cast<unsigned>(-exponent_difference);
    }
    CellCoordinate quotient = numerator / denominator;
    if(negative && numerator % denominator != 0) ++quotient;
    return negative ? -quotient : quotient;
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
