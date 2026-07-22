// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point_collection/gjk_closest_points_3d.hpp"
#include "geometry_3d_collision_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        for(const long double scale: {1.0e-2000L, 1.0e2000L}){
            const auto [first_point, second_point] =
                gjk_closest_points_3d(
                    geometry3d_collision_axis_box(
                        {-scale, -scale, -scale}, {scale, scale, scale}
                    ),
                    geometry3d_collision_axis_box(
                        {2 * scale, -scale, -scale},
                        {4 * scale, scale, scale}
                    )
                );
            const long double gap = second_point.x - first_point.x;
            if(!geometry3d_api_close(gap / scale, 1.0L)
                || !geometry3d_api_close(first_point.y, second_point.y)
                || !geometry3d_api_close(first_point.z, second_point.z)){
                return false;
            }
        }
        const long double translation = 1.0e2000L;
        const long double unit = std::nextafter(
            translation, std::numeric_limits<long double>::infinity()
        ) - translation;
        const Point3 first_minimum{translation, translation, translation};
        const Point3 first_maximum{
            translation + 8 * unit, translation + 8 * unit,
            translation + 8 * unit
        };
        const Point3 second_minimum{
            translation + 12 * unit, translation, translation
        };
        const Point3 second_maximum{
            translation + 20 * unit, translation + 8 * unit,
            translation + 8 * unit
        };
        const auto [first_point, second_point] = gjk_closest_points_3d(
            geometry3d_collision_axis_box(first_minimum, first_maximum),
            geometry3d_collision_axis_box(second_minimum, second_maximum)
        );
        if(!geometry3d_api_close(
            (second_point.x - first_point.x) / unit, 4.0L
        ) || first_point.x < first_minimum.x
            || first_point.x > first_maximum.x
            || second_point.x < second_minimum.x
            || second_point.x > second_maximum.x){
            return false;
        }

        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 first_center = geometry3d_random_point(random, -5, 5);
            const Point3 second_center = geometry3d_random_point(random, -5, 5);
            const Point3 first_half = geometry3d_random_point(random, 0.1L, 2);
            const Point3 second_half = geometry3d_random_point(random, 0.1L, 2);
            const auto [first_point, second_point] = gjk_closest_points_3d(
                geometry3d_collision_box(first_center, first_half),
                geometry3d_collision_box(second_center, second_half)
            );
            if(!geometry3d_inside_box(first_point, first_center, first_half)
                || !geometry3d_inside_box(
                    second_point, second_center, second_half
                )) return false;
            const Point3 difference = first_point - second_point;
            const long double actual = std::sqrt(dot(difference, difference));
            const long double expected = geometry3d_box_distance(
                first_center, first_half, second_center, second_half
            );
            if(!geometry3d_api_close(actual, expected, 1.0e-8L)) return false;
        }
        return true;
    });
}
