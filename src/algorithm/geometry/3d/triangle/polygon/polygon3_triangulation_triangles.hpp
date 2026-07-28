#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_POLYGON_POLYGON3_TRIANGULATION_TRIANGLES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_POLYGON_POLYGON3_TRIANGULATION_TRIANGLES_HPP_INCLUDED

#include <vector>

#include "../../index_set/polygon/polygon3_triangulation_indices.hpp"
#include "../../type/definition/triangle3.hpp"

inline std::vector<Triangle3> polygon3_triangulation(const Polygon3& polygon){
    const auto indices = polygon3_triangulation_indices(polygon);
    std::vector<Triangle3> result;
    result.reserve(indices.size());
    for(const auto& triangle: indices){
        result.push_back({
            polygon[triangle[0]], polygon[triangle[1]], polygon[triangle[2]]
        });
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TRIANGLE_POLYGON_POLYGON3_TRIANGULATION_TRIANGLES_HPP_INCLUDED
