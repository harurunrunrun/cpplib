// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/convex_polyhedron_intersects.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](
        std::mt19937_64& random,
        std::size_t rounds
    ){
        if(!convex_polyhedron_intersects(
            geometry3d_test_cube(), geometry3d_test_cube({1, 0, 0})
        )) return false;
        if(!convex_polyhedron_intersects(
            geometry3d_test_cube(), geometry3d_test_cube({2, 0, 0})
        )) return false;
        if(convex_polyhedron_intersects(
            geometry3d_test_cube(), geometry3d_test_cube({4, 0, 0})
        )) return false;

        bool threw = false;
        try{
            (void)convex_polyhedron_intersects(
                ConvexPolyhedron3{}, geometry3d_test_cube()
            );
        }catch(const std::invalid_argument&){
            threw = true;
        }
        if(!threw) return false;

        std::uniform_real_distribution<long double> coordinate(-5, 5);
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 first{
                coordinate(random), coordinate(random), coordinate(random)
            };
            const Point3 second{
                coordinate(random), coordinate(random), coordinate(random)
            };
            const bool expected =
                std::abs(first.x - second.x) <= 2.0L
                && std::abs(first.y - second.y) <= 2.0L
                && std::abs(first.z - second.z) <= 2.0L;
            if(convex_polyhedron_intersects(
                geometry3d_test_cube(first),
                geometry3d_test_cube(second)
            ) != expected) return false;
        }

        if(rounds == 128){
            ConvexPolyhedron3 first;
            ConvexPolyhedron3 second;
            first.affine_dimension = second.affine_dimension = 3;
            constexpr std::size_t point_count = 200000;
            first.vertices.reserve(point_count);
            second.vertices.reserve(point_count);
            for(std::size_t index = 0; index < point_count; ++index){
                const long double y = (index & 2U) == 0 ? -1.0L : 1.0L;
                const long double z = (index & 4U) == 0 ? -1.0L : 1.0L;
                first.vertices.push_back({
                    (index & 1U) == 0 ? -1.0L : 1.0L, y, z
                });
                second.vertices.push_back({
                    (index & 1U) == 0 ? 3.0L : 5.0L, y, z
                });
            }
            if(convex_polyhedron_intersects(first, second)) return false;
        }
        return true;
    });
}
