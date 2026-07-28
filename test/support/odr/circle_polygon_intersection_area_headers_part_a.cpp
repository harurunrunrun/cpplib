#include <vector>

#include "../../../src/algorithm/geometry/2d/scalar/circle_point_set/signed_circle_polygon_intersection_area.hpp"
#include "../../../src/algorithm/geometry/2d/aggregate/all/circle_polygon_intersection_area.hpp"

long double circle_polygon_intersection_area_odr_signed(
    const Circle& circle,
    const std::vector<Point>& polygon
){
    return signed_circle_polygon_intersection_area(circle, polygon);
}
