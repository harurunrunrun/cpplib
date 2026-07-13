#include "../../src/algorithm/geometry/advanced_convex_geometry.hpp"

#include <cstddef>
#include <utility>
#include <vector>

std::size_t advanced_umbrella_normalized_size_b(std::vector<Point> polygon){
    return normalize_convex_polygon(std::move(polygon)).size();
}
