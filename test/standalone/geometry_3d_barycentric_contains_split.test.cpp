// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <iostream>

#include "../../src/algorithm/geometry/3d/detail/point_point_point/barycentric_coordinates_detail.hpp"
#include "../../src/algorithm/geometry/3d/type/definition/circle3.hpp"
#include "../../src/algorithm/geometry/3d/constant/definition/constants.hpp"
#include "../../src/algorithm/geometry/3d/type/definition/line3.hpp"
#include "../../src/algorithm/geometry/3d/type/definition/plane3.hpp"
#include "../../src/algorithm/geometry/3d/type/definition/point3.hpp"
#include "../../src/algorithm/geometry/3d/type/definition/ray3.hpp"
#include "../../src/algorithm/geometry/3d/type/definition/segment3.hpp"
#include "../../src/algorithm/geometry/3d/integer/sphere_point/sphere_contains_point.hpp"
#include "../../src/algorithm/geometry/3d/type/definition/sphere3.hpp"
#include "../../src/algorithm/geometry/3d/predicate/tetrahedron_point/tetrahedron_contains_point.hpp"
#include "../../src/algorithm/geometry/3d/scalar/tetrahedron_point/tetrahedron_point_barycentric_coordinates.hpp"
#include "../../src/algorithm/geometry/3d/type/definition/tetrahedron3.hpp"
#include "../../src/algorithm/geometry/3d/predicate/triangle_point/triangle_contains_point.hpp"
#include "../../src/algorithm/geometry/3d/scalar/triangle_point/triangle_point_barycentric_coordinates.hpp"
#include "../../src/algorithm/geometry/3d/type/definition/triangle3.hpp"

namespace{

bool close(long double left, long double right){
    return std::abs(left - right) <= 1e-12L;
}

}  // namespace

int main(){
    const Triangle3 triangle{
        {0, 0, 0},
        {2, 0, 0},
        {0, 2, 0},
    };
    const auto triangle_weight = barycentric_coordinates(
        triangle,
        Point3{0.5L, 0.5L, 0}
    );
    assert(close(triangle_weight[0], 0.5L));
    assert(close(triangle_weight[1], 0.25L));
    assert(close(triangle_weight[2], 0.25L));
    assert(contains(triangle, Point3{0.5L, 0.5L, 0}));
    assert(contains(triangle, Point3{1, 1, 0}));
    assert(!contains(triangle, Point3{1.1L, 1.1L, 0}));
    assert(!contains(triangle, Point3{0.5L, 0.5L, 1}));

    const Tetrahedron3 tetrahedron{
        {0, 0, 0},
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
    };
    const auto tetrahedron_weight = barycentric_coordinates(
        tetrahedron,
        Point3{0.1L, 0.2L, 0.3L}
    );
    assert(close(tetrahedron_weight[0], 0.4L));
    assert(close(tetrahedron_weight[1], 0.1L));
    assert(close(tetrahedron_weight[2], 0.2L));
    assert(close(tetrahedron_weight[3], 0.3L));
    assert(contains(tetrahedron, Point3{0.1L, 0.2L, 0.3L}));
    assert(contains(tetrahedron, Point3{1, 0, 0}));
    assert(!contains(tetrahedron, Point3{-0.1L, 0.2L, 0.3L}));

    const Sphere3 sphere{{0, 0, 0}, 2};
    assert(contains(sphere, Point3{0, 0, 0}) == 1);
    assert(contains(sphere, Point3{2, 0, 0}) == 0);
    assert(contains(sphere, Point3{3, 0, 0}) == -1);

    std::cout << "OK\n";
}
