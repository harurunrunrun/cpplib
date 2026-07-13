#include "../../src/algorithm/geometry/advanced_convex_geometry.hpp"

bool advanced_umbrella_contains_a(const Line& half_plane, const Point& point){
    return half_plane_contains(half_plane, point);
}
