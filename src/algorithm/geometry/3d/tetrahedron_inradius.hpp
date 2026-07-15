#pragma once

#include <stdexcept>

#include "area.hpp"
#include "base.hpp"
#include "tetrahedron_orientation.hpp"
#include "volume.hpp"

inline long double tetrahedron_inradius(const Tetrahedron3& tetrahedron){
    if(tetrahedron_orientation(tetrahedron) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron");
    }
    const long double surface_area =
        area({tetrahedron.b, tetrahedron.c, tetrahedron.d}) +
        area({tetrahedron.a, tetrahedron.c, tetrahedron.d}) +
        area({tetrahedron.a, tetrahedron.b, tetrahedron.d}) +
        area({tetrahedron.a, tetrahedron.b, tetrahedron.c});
    return 3 * volume(tetrahedron) / surface_area;
}
