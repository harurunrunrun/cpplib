#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SORT_POINTS_BY_ARGUMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SORT_POINTS_BY_ARGUMENT_HPP_INCLUDED

#include <algorithm>
#include <vector>

#include "types.hpp"

namespace sort_points_by_argument_detail{

inline int sector(const Point& point){
    if(point.y < 0) return -1;
    if(point.y == 0 && point.x >= 0) return 0;
    return 1;
}

struct Less{
    bool operator()(const Point& left, const Point& right) const{
        const int left_sector = sector(left);
        const int right_sector = sector(right);
        if(left_sector != right_sector) return left_sector < right_sector;
        const long double determinant =
            left.x * right.y - left.y * right.x;
        if(determinant != 0) return determinant > 0;
        if(left.x != right.x) return left.x < right.x;
        return left.y < right.y;
    }
};

}  // namespace sort_points_by_argument_detail

inline std::vector<Point> sort_points_by_argument(std::vector<Point> points){
    std::sort(points.begin(), points.end(), sort_points_by_argument_detail::Less{});
    return points;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SORT_POINTS_BY_ARGUMENT_HPP_INCLUDED
