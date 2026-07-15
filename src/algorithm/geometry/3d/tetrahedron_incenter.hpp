#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>

#include "base.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "is_finite.hpp"
#include "tetrahedron_orientation.hpp"

namespace geometry3d_tetrahedron_incenter_detail{

struct LocalData{
    Point3 offset{};
    long double scale = 1.0L;
    long double normalized_radius = 0.0L;
};

inline Point3 rescale_difference(
    const Geometry3DNormalizedDifference& difference,
    long double scale
){
    const long double ratio = difference.scale / scale;
    const bool nonzero = difference.value.x != 0.0L
        || difference.value.y != 0.0L
        || difference.value.z != 0.0L;
    if(nonzero && ratio == 0.0L)[[unlikely]]{
        throw std::overflow_error(
            "tetrahedron local coordinates are not representable"
        );
    }
    return difference.value * ratio;
}

inline long double face_area(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    const Point3 product = cross(second - first, third - first);
    return std::hypot(product.x, product.y, product.z) / 2.0L;
}

inline LocalData local_data(const Tetrahedron3& tetrahedron){
    if(!geometry3d_is_finite(tetrahedron))[[unlikely]]{
        throw std::invalid_argument(
            "tetrahedron incenter requires finite points"
        );
    }
    if(tetrahedron_orientation(tetrahedron) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron");
    }
    const std::array<Geometry3DNormalizedDifference, 3> differences{
        geometry3d_normalized_difference(
            tetrahedron.b, tetrahedron.a
        ),
        geometry3d_normalized_difference(
            tetrahedron.c, tetrahedron.a
        ),
        geometry3d_normalized_difference(
            tetrahedron.d, tetrahedron.a
        ),
    };
    const long double scale = std::max({
        differences[0].scale,
        differences[1].scale,
        differences[2].scale,
    });
    const std::array<Point3, 4> points{
        Point3{},
        rescale_difference(differences[0], scale),
        rescale_difference(differences[1], scale),
        rescale_difference(differences[2], scale),
    };
    const long double opposite_a =
        face_area(points[1], points[2], points[3]);
    const long double opposite_b =
        face_area(points[0], points[2], points[3]);
    const long double opposite_c =
        face_area(points[0], points[1], points[3]);
    const long double opposite_d =
        face_area(points[0], points[1], points[2]);
    const long double surface_area =
        opposite_a + opposite_b + opposite_c + opposite_d;
    const long double six_volume = std::abs(dot(
        points[1], cross(points[2], points[3])
    ));
    if(surface_area == 0.0L || six_volume == 0.0L)[[unlikely]]{
        throw std::overflow_error(
            "tetrahedron local metric is not representable"
        );
    }
    return {
        (
            points[1] * opposite_b
            + points[2] * opposite_c
            + points[3] * opposite_d
        ) / surface_area,
        scale,
        (six_volume / 2.0L) / surface_area,
    };
}

inline long double restore_length(
    long double normalized,
    long double scale,
    const char* message
){
    if(normalized > std::numeric_limits<long double>::max() / scale){
        throw std::overflow_error(message);
    }
    const long double result = normalized * scale;
    if(!std::isfinite(result) || result == 0.0L)[[unlikely]]{
        throw std::overflow_error(message);
    }
    return result;
}

}  // namespace geometry3d_tetrahedron_incenter_detail

inline Point3 tetrahedron_incenter(const Tetrahedron3& tetrahedron){
    const auto data =
        geometry3d_tetrahedron_incenter_detail::local_data(tetrahedron);
    const Point3 result{
        std::fma(data.offset.x, data.scale, tetrahedron.a.x),
        std::fma(data.offset.y, data.scale, tetrahedron.a.y),
        std::fma(data.offset.z, data.scale, tetrahedron.a.z),
    };
    if(!geometry3d_is_finite(result))[[unlikely]]{
        throw std::overflow_error(
            "tetrahedron incenter is not representable"
        );
    }
    return result;
}
