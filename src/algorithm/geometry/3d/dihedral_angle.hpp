#pragma once

#include "angle.hpp"
#include "base.hpp"
#include "plane3_unit_normal.hpp"

inline long double dihedral_angle(
    const Plane3& first,
    const Plane3& second
){
    return angle(
        plane3_unit_normal(first),
        plane3_unit_normal(second)
    );
}
