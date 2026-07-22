// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/predicate/gjk_intersects_3d.hpp"
#include "geometry_3d_collision_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        if(!gjk_intersects_3d(
            geometry3d_collision_tetra(), geometry3d_collision_tetra({0.1L, 0, 0})
        )) return false;
        if(gjk_intersects_3d(
            geometry3d_collision_tetra(), geometry3d_collision_tetra({3, 3, 3})
        )) return false;
        for(const long double scale: {1.0e-2000L, 1.0e2000L}){
            const ConvexPolyhedron3 scaled_first =
                geometry3d_collision_axis_box(
                    {-scale, -scale, -scale}, {scale, scale, scale}
                );
            if(gjk_intersects_3d(
                scaled_first,
                geometry3d_collision_axis_box(
                    {2 * scale, -scale, -scale},
                    {4 * scale, scale, scale}
                )
            )) return false;
            if(!gjk_intersects_3d(
                scaled_first,
                geometry3d_collision_axis_box(
                    {scale, -scale, -scale},
                    {3 * scale, scale, scale}
                )
            )) return false;
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
        if(gjk_intersects_3d(
            translated_first,
            geometry3d_collision_axis_box(
                {translation + 12 * unit, translation, translation},
                {translation + 20 * unit, translation + 8 * unit,
                    translation + 8 * unit}
            )
        )) return false;
        if(!gjk_intersects_3d(
            translated_first,
            geometry3d_collision_axis_box(
                {translation + 4 * unit, translation, translation},
                {translation + 12 * unit, translation + 8 * unit,
                    translation + 8 * unit}
            )
        )) return false;
        ConvexPolyhedron3 invalid = translated_first;
        invalid.vertices[0].x =
            std::numeric_limits<long double>::infinity();
        try{
            static_cast<void>(gjk_intersects_3d(invalid, translated_first));
            return false;
        }catch(const std::invalid_argument&){}

        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 center = geometry3d_random_point(random, -3, 3);
            const Point3 half = geometry3d_random_point(random, 0.2L, 1.5L);
            const Point3 other_center = geometry3d_random_point(random, -3, 3);
            const Point3 other_half = geometry3d_random_point(random, 0.2L, 1.5L);
            const bool expected = geometry3d_box_distance(
                center, half, other_center, other_half
            ) <= 1.0e-10L;
            if(gjk_intersects_3d(
                geometry3d_collision_box(center, half),
                geometry3d_collision_box(other_center, other_half)
            ) != expected) return false;
        }
        return true;
    });
}
