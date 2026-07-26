// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/epa_3d.hpp"
#include "geometry_3d_collision_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const Point3 half{1, 1, 1};
        EPAResult3 result = epa_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_box({1.5L, 0, 0}, half)
        );
        if(!result.intersects || !result.converged
            || !geometry3d_api_close(result.penetration_depth, 0.5L, 1e-7L)
            || !geometry3d_api_close(result.normal, {1, 0, 0}, 1e-7L)){
            return false;
        }
        if(epa_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_box({4, 0, 0}, half)
        ).intersects) return false;
        const EPAResult3 tetra_result = epa_3d(
            geometry3d_collision_tetra(),
            geometry3d_collision_tetra({0.1L, 0.1L, 0.1L})
        );
        if(!tetra_result.intersects || !tetra_result.converged
            || !(tetra_result.penetration_depth > 0.0L)) return false;

        for(const long double scale: {1.0e-2000L, 1.0e2000L}){
            const ConvexPolyhedron3 scaled_first =
                geometry3d_collision_axis_box(
                    {-scale, -scale, -scale}, {scale, scale, scale}
                );
            const ConvexPolyhedron3 scaled_second =
                geometry3d_collision_axis_box(
                    {scale / 2, -scale, -scale},
                    {5 * scale / 2, scale, scale}
                );
            result = epa_3d(scaled_first, scaled_second);
            if(!result.intersects || !result.converged
                || !geometry3d_api_close(
                    result.penetration_depth / scale, 0.5L, 1.0e-7L
                )
                || !geometry3d_collision_is_unit_finite(result.normal)){
                return false;
            }
            const GJKResult3 gjk = gjk_query_3d(
                scaled_first, scaled_second
            );
            const EPAResult3 with_simplex = epa_3d(
                scaled_first, scaled_second, gjk
            );
            if(!with_simplex.intersects || !with_simplex.converged
                || !geometry3d_api_close(
                    with_simplex.penetration_depth
                        / result.penetration_depth,
                    1.0L, 1.0e-7L
                )) return false;
            const EPAResult3 touching = epa_3d(
                scaled_first,
                geometry3d_collision_axis_box(
                    {scale, -scale, -scale},
                    {3 * scale, scale, scale}
                )
            );
            if(!touching.intersects || touching.penetration_depth != 0.0L){
                return false;
            }
        }

        const long double translation = 1.0e2000L;
        const long double unit = std::nextafter(
            translation, std::numeric_limits<long double>::infinity()
        ) - translation;
        const ConvexPolyhedron3 translated_first =
            geometry3d_collision_axis_box(
                {translation, translation, translation},
                {translation + 8 * unit, translation + 8 * unit,
                    translation + 8 * unit}
            );
        const ConvexPolyhedron3 translated_second =
            geometry3d_collision_axis_box(
                {translation + 4 * unit, translation, translation},
                {translation + 12 * unit, translation + 8 * unit,
                    translation + 8 * unit}
            );
        const GJKResult3 translated_gjk = gjk_query_3d(
            translated_first, translated_second
        );
        result = epa_3d(
            translated_first, translated_second, translated_gjk
        );
        if(!result.intersects || !result.converged
            || !geometry3d_api_close(
                result.penetration_depth / unit, 4.0L, 1.0e-7L
            )
            || !geometry3d_collision_is_unit_finite(result.normal)
            || !geometry3d_is_finite(result.point_on_first)
            || !geometry3d_is_finite(result.point_on_second)){
            return false;
        }

        const EPAResult3 limited = epa_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_rotated_z_box(
                {0.7L, 0.37L, 0.21L}, {1, 0.7L, 1.2L}, 0.37L
            ),
            1.0e-10L, 1
        );
        if(!limited.intersects || limited.converged
            || limited.iterations != 1) return false;

        GJKResult3 invalid_simplex = translated_gjk;
        invalid_simplex.simplex[0].point_on_first.x =
            std::numeric_limits<long double>::quiet_NaN();
        try{
            static_cast<void>(epa_3d(
                translated_first, translated_second, invalid_simplex
            ));
            return false;
        }catch(const std::invalid_argument&){}

        ConvexPolyhedron3 invalid_polyhedron = translated_first;
        invalid_polyhedron.vertices[0].y =
            std::numeric_limits<long double>::infinity();
        try{
            static_cast<void>(epa_3d(
                invalid_polyhedron, translated_second
            ));
            return false;
        }catch(const std::invalid_argument&){}

        const long double near_max =
            std::numeric_limits<long double>::max() * 0.75L;
        try{
            static_cast<void>(epa_3d(
                geometry3d_collision_axis_box(
                    {-near_max, -near_max, -near_max},
                    {near_max, near_max, near_max}
                ),
                geometry3d_collision_axis_box(
                    {-near_max, -near_max, -near_max},
                    {near_max, near_max, near_max}
                )
            ));
            return false;
        }catch(const std::overflow_error&){}

        const ConvexPolyhedron3 point{0, {{0, 0, 0}}, {}};
        try{
            static_cast<void>(epa_3d(
                point, geometry3d_collision_box({}, half)
            ));
            return false;
        }catch(const std::invalid_argument&){}
        const GJKResult3 point_gjk = gjk_query_3d(
            point, geometry3d_collision_box({}, half)
        );
        if(!point_gjk.intersects) return false;
        try{
            static_cast<void>(epa_3d(
                point, geometry3d_collision_box({}, half), point_gjk
            ));
            return false;
        }catch(const std::invalid_argument&){}
        try{
            static_cast<void>(epa_3d(
                geometry3d_collision_box({}, half), point
            ));
            return false;
        }catch(const std::invalid_argument&){}

        std::uniform_real_distribution<long double> offset(-1.8L, 1.8L);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 center{offset(random), offset(random), offset(random)};
            const long double expected = std::min({
                2.0L - std::abs(center.x), 2.0L - std::abs(center.y),
                2.0L - std::abs(center.z)
            });
            result = epa_3d(
                geometry3d_collision_box({}, half),
                geometry3d_collision_box(center, half)
            );
            if(!result.intersects
                || !geometry3d_api_close(
                    result.penetration_depth, expected, 1.0e-7L
                )) return false;
        }
        return true;
    });
}
