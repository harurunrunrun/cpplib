// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/query/convex_collision_3d.hpp"
#include "geometry_3d_collision_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const Point3 half{1, 1, 1};
        ConvexCollisionResult3 result = convex_collision_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_box({3, 0, 0}, half)
        );
        if(result.intersects || !result.converged
            || !geometry3d_api_close(result.distance, 1)) return false;
        result = convex_collision_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_box({1.5L, 0, 0}, half)
        );
        if(!result.intersects || !result.converged
            || !geometry3d_api_close(result.penetration_depth, 0.5L, 1e-7L)){
            return false;
        }
        for(const long double scale: {1.0e-2000L, 1.0e2000L}){
            const ConvexPolyhedron3 scaled_first =
                geometry3d_collision_axis_box(
                    {-scale, -scale, -scale}, {scale, scale, scale}
                );
            result = convex_collision_3d(
                scaled_first,
                geometry3d_collision_axis_box(
                    {2 * scale, -scale, -scale},
                    {4 * scale, scale, scale}
                )
            );
            if(result.intersects || !result.converged
                || !geometry3d_api_close(result.distance / scale, 1.0L)
                || !geometry3d_collision_is_unit_finite(result.normal)){
                return false;
            }
            result = convex_collision_3d(
                scaled_first,
                geometry3d_collision_axis_box(
                    {scale / 2, -scale, -scale},
                    {5 * scale / 2, scale, scale}
                )
            );
            if(!result.intersects || !result.converged
                || !geometry3d_api_close(
                    result.penetration_depth / scale, 0.5L, 1.0e-7L
                )
                || !geometry3d_collision_is_unit_finite(result.normal)){
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
        result = convex_collision_3d(
            translated_first,
            geometry3d_collision_axis_box(
                {translation + 12 * unit, translation, translation},
                {translation + 20 * unit, translation + 8 * unit,
                    translation + 8 * unit}
            )
        );
        if(result.intersects || !result.converged
            || !geometry3d_api_close(result.distance / unit, 4.0L)
            || !geometry3d_collision_is_unit_finite(result.normal)){
            return false;
        }
        result = convex_collision_3d(
            translated_first,
            geometry3d_collision_axis_box(
                {translation + 4 * unit, translation, translation},
                {translation + 12 * unit, translation + 8 * unit,
                    translation + 8 * unit}
            )
        );
        if(!result.intersects || !result.converged
            || !geometry3d_api_close(
                result.penetration_depth / unit, 4.0L, 1.0e-7L
            )
            || !geometry3d_collision_is_unit_finite(result.normal)){
            return false;
        }

        const ConvexCollisionResult3 limited = convex_collision_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_rotated_z_box(
                {2.4L, 0.7L, 0.3L}, half, 0.37L
            ),
            1.0e-10L, 1, 1
        );
        if(limited.gjk_iterations != 1 || limited.converged) return false;

        const long double near_max =
            std::numeric_limits<long double>::max() * 0.75L;
        const long double shallow_depth = near_max * 1.0e-6L;
        result = convex_collision_3d(
            geometry3d_collision_axis_box(
                {-near_max, -near_max, -near_max},
                {0, near_max, near_max}
            ),
            geometry3d_collision_axis_box(
                {-shallow_depth, -near_max, -near_max},
                {near_max, near_max, near_max}
            )
        );
        if(!result.intersects || !result.converged
            || !geometry3d_api_close(
                result.penetration_depth / shallow_depth, 1.0L, 1.0e-7L
            )
            || !geometry3d_collision_is_unit_finite(result.normal)
            || !geometry3d_is_finite(result.point_on_first)
            || !geometry3d_is_finite(result.point_on_second)){
            return false;
        }

        const ConvexPolyhedron3 point{0, {{0, 0, 0}}, {}};
        const ConvexPolyhedron3 other_point{0, {{2, 0, 0}}, {}};
        result = convex_collision_3d(point, point);
        if(!result.intersects || !result.converged
            || result.penetration_depth != 0.0L
            || result.normal != Point3{}
            || result.epa_iterations != 0
            || !geometry3d_is_finite(result.point_on_first)
            || !geometry3d_is_finite(result.point_on_second)){
            return false;
        }
        result = convex_collision_3d(point, other_point);
        if(result.intersects || !result.converged
            || !geometry3d_api_close(result.distance, 2.0L)
            || !geometry3d_collision_is_unit_finite(result.normal)){
            return false;
        }

        const ConvexPolyhedron3 first_segment{
            1, {{-1, 0, 0}, {1, 0, 0}}, {}
        };
        const ConvexPolyhedron3 second_segment{
            1, {{0, -1, 0}, {0, 1, 0}}, {}
        };
        result = convex_collision_3d(first_segment, second_segment);
        if(!result.intersects || !result.converged
            || result.penetration_depth != 0.0L
            || result.normal != Point3{}
            || result.epa_iterations != 0
            || !geometry3d_is_finite(result.point_on_first)
            || !geometry3d_is_finite(result.point_on_second)){
            return false;
        }

        const ConvexPolyhedron3 first_polygon{
            2,
            {{-1, -1, 0}, {1, -1, 0}, {1, 1, 0}, {-1, 1, 0}},
            {}
        };
        const ConvexPolyhedron3 second_polygon{
            2,
            {{0, -2, 0}, {2, -2, 0}, {2, 2, 0}, {0, 2, 0}},
            {}
        };
        result = convex_collision_3d(first_polygon, second_polygon);
        if(!result.intersects || !result.converged
            || result.penetration_depth != 0.0L
            || result.normal != Point3{}
            || result.epa_iterations != 0
            || !geometry3d_is_finite(result.point_on_first)
            || !geometry3d_is_finite(result.point_on_second)){
            return false;
        }

        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 first_center = geometry3d_random_point(random, -4, 4);
            const Point3 second_center = geometry3d_random_point(random, -4, 4);
            const Point3 first_half = geometry3d_random_point(random, 0.2L, 1.5L);
            const Point3 second_half = geometry3d_random_point(random, 0.2L, 1.5L);
            const long double expected = geometry3d_box_distance(
                first_center, first_half, second_center, second_half
            );
            result = convex_collision_3d(
                geometry3d_collision_box(first_center, first_half),
                geometry3d_collision_box(second_center, second_half)
            );
            if(result.intersects != (expected <= 1.0e-10L)) return false;
            if(expected > 0 && !geometry3d_api_close(
                result.distance, expected, 1.0e-8L
            )) return false;
        }
        return true;
    });
}
