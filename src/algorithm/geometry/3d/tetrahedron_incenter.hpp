#pragma once

#include <stdexcept>

#include "area.hpp"
#include "base.hpp"
#include "tetrahedron_orientation.hpp"

inline Point3 tetrahedron_incenter(const Tetrahedron3& tetrahedron){
    if(tetrahedron_orientation(tetrahedron) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron");
    }
    const long double opposite_a = area({
        tetrahedron.b, tetrahedron.c, tetrahedron.d
    });
    const long double opposite_b = area({
        tetrahedron.a, tetrahedron.c, tetrahedron.d
    });
    const long double opposite_c = area({
        tetrahedron.a, tetrahedron.b, tetrahedron.d
    });
    const long double opposite_d = area({
        tetrahedron.a, tetrahedron.b, tetrahedron.c
    });
    const long double surface_area =
        opposite_a + opposite_b + opposite_c + opposite_d;
    return (
        tetrahedron.a * opposite_a +
        tetrahedron.b * opposite_b +
        tetrahedron.c * opposite_c +
        tetrahedron.d * opposite_d
    ) / surface_area;
}
