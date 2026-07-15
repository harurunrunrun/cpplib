#pragma once

#include <array>
#include <stdexcept>

#include "base.hpp"
#include "circumcircle.hpp"
#include "is_finite.hpp"

inline Sphere3 circumsphere(const Tetrahedron3& tetrahedron){
    using namespace geometry3d_adaptive_detail;
    using namespace geometry3d_circumcircle_detail;
    if(!geometry3d_is_finite(tetrahedron))[[unlikely]]{
        throw std::invalid_argument("circumsphere requires finite points");
    }
    const std::array<ExactVector, 3> directions{
        geometry3d_plane_numeric_detail::exact_difference(
            tetrahedron.b, tetrahedron.a
        ),
        geometry3d_plane_numeric_detail::exact_difference(
            tetrahedron.c, tetrahedron.a
        ),
        geometry3d_plane_numeric_detail::exact_difference(
            tetrahedron.d, tetrahedron.a
        ),
    };
    const std::array<ExactVector, 3> cofactors{
        exact_cross(directions[1], directions[2]),
        exact_cross(directions[2], directions[0]),
        exact_cross(directions[0], directions[1]),
    };
    const ExactDyadic denominator = multiply(
        exact_dyadic(2.0L), exact_dot(directions[0], cofactors[0])
    );
    if(sign(denominator) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron");
    }
    const std::array<ExactDyadic, 3> squared_lengths{
        exact_dot(directions[0], directions[0]),
        exact_dot(directions[1], directions[1]),
        exact_dot(directions[2], directions[2]),
    };
    ExactVector numerator{};
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        for(std::size_t direction = 0; direction < 3; ++direction){
            numerator[coordinate] = add(
                numerator[coordinate],
                multiply(
                    squared_lengths[direction],
                    cofactors[direction][coordinate]
                )
            );
        }
    }
    const Point3 center = exact_affine_point(
        tetrahedron.a, numerator, denominator,
        "3D circumsphere center is not representable"
    );
    return {
        center,
        checked_distance(
            center, tetrahedron.a,
            "3D circumsphere radius is not representable"
        ),
    };
}
