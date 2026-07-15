#pragma once

#include "base.hpp"
#include "triangle_incenter.hpp"

inline long double triangle_inradius(const Triangle3& triangle){
    const auto data =
        geometry3d_triangle_incenter_detail::local_data(triangle);
    return geometry3d_triangle_incenter_detail::restore_length(
        data.normalized_radius, data.scale,
        "3D triangle inradius is not representable"
    );
}
