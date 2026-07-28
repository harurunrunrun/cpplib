#include <vector>

#include "../../../src/algorithm/geometry/2d/integer_set/point_set_point_set/enumerate_points_on_polygon_boundary.hpp"
#include "../../../src/algorithm/geometry/2d/aggregate/all/polygon_boundary_points.hpp"

std::vector<int> polygon_boundary_points_odr_part_a(
    const std::vector<Point>& polygon,
    const std::vector<Point>& points,
    bool unique
){
    return enumerate_points_on_polygon_boundary(polygon, points, unique);
}
