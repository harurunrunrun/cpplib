#include <vector>

#include "../../../src/algorithm/geometry/2d/query/polygon_boundary_point_query.hpp"
#include "../../../src/algorithm/geometry/2d/core/polygon_boundary_points.hpp"

std::vector<int> polygon_boundary_points_odr_part_b(
    const std::vector<Point>& polygon,
    const std::vector<Point>& points,
    bool unique
){
    return PolygonBoundaryPointQuery(polygon).enumerate(points, unique);
}
