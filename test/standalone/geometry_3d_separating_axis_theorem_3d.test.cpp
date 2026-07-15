// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/separating_axis_theorem_3d.hpp"
#include "geometry_3d_collision_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const Point3 half{1, 1, 1};
        SATResult3 result = separating_axis_theorem_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_box({3, 0, 0}, half)
        );
        if(result.intersects || !geometry3d_api_close(result.separation, 1)
            || result.tested_axes < 3) return false;
        result = separating_axis_theorem_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_box({1.5L, 0, 0}, half)
        );
        if(!result.intersects
            || !geometry3d_api_close(result.penetration_depth, 0.5L)){
            return false;
        }
        if(!separating_axis_theorem_3d(
            geometry3d_collision_tetra(),
            geometry3d_collision_tetra({0.1L, 0.1L, 0.1L})
        ).intersects) return false;
        if(separating_axis_theorem_3d(
            geometry3d_collision_tetra(),
            geometry3d_collision_tetra({3, 3, 3})
        ).intersects) return false;
        const long double quarter_turn = std::acos(-1.0L) / 4.0L;
        if(separating_axis_theorem_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_rotated_z_box(
                {3, 0, 0}, half, quarter_turn
            )
        ).intersects) return false;
        if(!separating_axis_theorem_3d(
            geometry3d_collision_box({}, half),
            geometry3d_collision_rotated_z_box(
                {2, 0, 0}, half, quarter_turn
            )
        ).intersects) return false;

        for(const long double scale: {1.0e-2000L, 1.0e2000L}){
            const ConvexPolyhedron3 scaled_first =
                geometry3d_collision_axis_box(
                    {-scale, -scale, -scale}, {scale, scale, scale}
                );
            result = separating_axis_theorem_3d(
                scaled_first,
                geometry3d_collision_axis_box(
                    {2 * scale, -scale, -scale},
                    {4 * scale, scale, scale}
                )
            );
            if(result.intersects
                || !geometry3d_api_close(result.separation / scale, 1.0L)
                || !geometry3d_collision_is_unit_finite(result.axis)){
                return false;
            }
            result = separating_axis_theorem_3d(
                scaled_first,
                geometry3d_collision_axis_box(
                    {scale / 2, -scale, -scale},
                    {5 * scale / 2, scale, scale}
                )
            );
            if(!result.intersects
                || !geometry3d_api_close(
                    result.penetration_depth / scale, 0.5L
                )
                || !geometry3d_collision_is_unit_finite(result.axis)){
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
        result = separating_axis_theorem_3d(
            translated_first,
            geometry3d_collision_axis_box(
                {translation + 12 * unit, translation, translation},
                {translation + 20 * unit, translation + 8 * unit,
                    translation + 8 * unit}
            )
        );
        if(result.intersects
            || !geometry3d_api_close(result.separation / unit, 4.0L)
            || !geometry3d_collision_is_unit_finite(result.axis)){
            return false;
        }

        ConvexPolyhedron3 invalid = translated_first;
        invalid.vertices[0].z =
            std::numeric_limits<long double>::quiet_NaN();
        try{
            static_cast<void>(separating_axis_theorem_3d(
                invalid, translated_first
            ));
            return false;
        }catch(const std::invalid_argument&){}
        invalid.vertices[0].z =
            std::numeric_limits<long double>::infinity();
        try{
            static_cast<void>(separating_axis_theorem_3d(
                translated_first, invalid
            ));
            return false;
        }catch(const std::invalid_argument&){}

        const long double near_max =
            std::numeric_limits<long double>::max() * 0.75L;
        const long double max_unit = std::nextafter(
            near_max, std::numeric_limits<long double>::infinity()
        ) - near_max;
        try{
            static_cast<void>(separating_axis_theorem_3d(
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
            result = separating_axis_theorem_3d(
                geometry3d_collision_box(first_center, first_half),
                geometry3d_collision_box(second_center, second_half)
            );
            const bool expected = geometry3d_box_distance(
                first_center, first_half, second_center, second_half
            ) <= 1.0e-10L;
            if(result.intersects != expected) return false;
        }
        return true;
    });
}
