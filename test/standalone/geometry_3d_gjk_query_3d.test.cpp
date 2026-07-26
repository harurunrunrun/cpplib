// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/gjk_query_3d.hpp"
#include "geometry_3d_collision_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const Point3 half{1, 1, 1};
        const ConvexPolyhedron3 first = geometry3d_collision_box({}, half);
        GJKResult3 result = gjk_query_3d(
            first, geometry3d_collision_box({3, 0, 0}, half)
        );
        if(result.intersects || !result.converged
            || !geometry3d_api_close(result.distance, 1.0L)
            || !geometry3d_api_close(result.normal, {1, 0, 0})) return false;
        result = gjk_query_3d(
            first, geometry3d_collision_box({1.5L, 0, 0}, half)
        );
        if(!result.intersects || result.distance != 0.0L) return false;

        for(const long double scale: {1.0e-2000L, 1.0e2000L}){
            const ConvexPolyhedron3 scaled_first =
                geometry3d_collision_axis_box(
                    {-scale, -scale, -scale}, {scale, scale, scale}
                );
            const ConvexPolyhedron3 scaled_second =
                geometry3d_collision_axis_box(
                    {2 * scale, -scale, -scale},
                    {4 * scale, scale, scale}
                );
            result = gjk_query_3d(scaled_first, scaled_second);
            if(result.intersects || !result.converged
                || !geometry3d_api_close(result.distance / scale, 1.0L)
                || !geometry3d_collision_is_unit_finite(result.normal)){
                return false;
            }
            if(!gjk_query_3d(
                scaled_first,
                geometry3d_collision_axis_box(
                    {scale, -scale, -scale},
                    {3 * scale, scale, scale}
                )
            ).intersects) return false;
            if(!gjk_query_3d(
                scaled_first,
                geometry3d_collision_axis_box(
                    {scale / 2, -scale, -scale},
                    {5 * scale / 2, scale, scale}
                )
            ).intersects) return false;
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
                {translation + 12 * unit, translation, translation},
                {translation + 20 * unit, translation + 8 * unit,
                    translation + 8 * unit}
            );
        result = gjk_query_3d(translated_first, translated_second);
        if(result.intersects || !result.converged
            || !geometry3d_api_close(result.distance / unit, 4.0L)
            || !geometry3d_collision_is_unit_finite(result.normal)){
            return false;
        }
        const long double contact_gap =
            result.point_on_second.x - result.point_on_first.x;
        if(!geometry3d_api_close(contact_gap / unit, 4.0L)) return false;

        const GJKResult3 limited = gjk_query_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_rotated_z_box(
                {2.4L, 0.7L, 0.3L}, half, 0.37L
            ),
            1.0e-12L, 1
        );
        if(limited.iterations != 1 || limited.converged) return false;

        ConvexPolyhedron3 invalid = first;
        invalid.vertices[0].x =
            std::numeric_limits<long double>::quiet_NaN();
        try{
            static_cast<void>(gjk_query_3d(invalid, first));
            return false;
        }catch(const std::invalid_argument&){}
        invalid.vertices[0].x =
            std::numeric_limits<long double>::infinity();
        try{
            static_cast<void>(gjk_query_3d(first, invalid));
            return false;
        }catch(const std::invalid_argument&){}

        const long double near_max =
            std::numeric_limits<long double>::max() * 0.75L;
        const long double max_unit = std::nextafter(
            near_max, std::numeric_limits<long double>::infinity()
        ) - near_max;
        try{
            static_cast<void>(gjk_query_3d(
                geometry3d_collision_axis_box(
                    {-near_max - 8 * max_unit, -max_unit, -max_unit},
                    {-near_max, max_unit, max_unit}
                ),
                geometry3d_collision_axis_box(
                    {near_max, -max_unit, -max_unit},
                    {near_max + 8 * max_unit, max_unit, max_unit}
                )
            ));
            return false;
        }catch(const std::overflow_error&){}

        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 first_center = geometry3d_random_point(random, -4, 4);
            const Point3 second_center = geometry3d_random_point(random, -4, 4);
            const Point3 first_half = geometry3d_random_point(random, 0.2L, 1.5L);
            const Point3 second_half = geometry3d_random_point(random, 0.2L, 1.5L);
            const long double expected = geometry3d_box_distance(
                first_center, first_half, second_center, second_half
            );
            result = gjk_query_3d(
                geometry3d_collision_box(first_center, first_half),
                geometry3d_collision_box(second_center, second_half)
            );
            if(result.intersects != (expected <= 1.0e-10L)
                || !geometry3d_api_close(result.distance, expected, 1.0e-8L)){
                return false;
            }
        }
        return true;
    });
}
