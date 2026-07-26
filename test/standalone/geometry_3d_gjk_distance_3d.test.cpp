// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/gjk_distance_3d.hpp"
#include "geometry_3d_collision_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        for(const long double scale: {1.0e-2000L, 1.0e2000L}){
            const long double actual = gjk_distance_3d(
                geometry3d_collision_axis_box(
                    {-scale, -scale, -scale}, {scale, scale, scale}
                ),
                geometry3d_collision_axis_box(
                    {2 * scale, -scale, -scale},
                    {4 * scale, scale, scale}
                )
            );
            if(!geometry3d_api_close(actual / scale, 1.0L)) return false;
        }
        const long double translation = 1.0e2000L;
        const long double unit = std::nextafter(
            translation, std::numeric_limits<long double>::infinity()
        ) - translation;
        const long double translated = gjk_distance_3d(
            geometry3d_collision_axis_box(
                {translation, translation, translation},
                {translation + 8 * unit, translation + 8 * unit,
                    translation + 8 * unit}
            ),
            geometry3d_collision_axis_box(
                {translation + 12 * unit, translation, translation},
                {translation + 20 * unit, translation + 8 * unit,
                    translation + 8 * unit}
            )
        );
        if(!geometry3d_api_close(translated / unit, 4.0L)) return false;

        const long double near_max =
            std::numeric_limits<long double>::max() * 0.75L;
        const long double max_unit = std::nextafter(
            near_max, std::numeric_limits<long double>::infinity()
        ) - near_max;
        try{
            static_cast<void>(gjk_distance_3d(
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
            const Point3 first_center = geometry3d_random_point(random, -5, 5);
            const Point3 second_center = geometry3d_random_point(random, -5, 5);
            const Point3 first_half = geometry3d_random_point(random, 0.1L, 2);
            const Point3 second_half = geometry3d_random_point(random, 0.1L, 2);
            const long double expected = geometry3d_box_distance(
                first_center, first_half, second_center, second_half
            );
            const long double actual = gjk_distance_3d(
                geometry3d_collision_box(first_center, first_half),
                geometry3d_collision_box(second_center, second_half)
            );
            if(!geometry3d_api_close(actual, expected, 1.0e-8L)) return false;
        }
        return true;
    });
}
