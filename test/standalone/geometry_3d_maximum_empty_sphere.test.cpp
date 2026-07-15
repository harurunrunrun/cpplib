// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
#include <vector>

#include "../../src/algorithm/geometry/3d/convex_hull_3d.hpp"
#include "../../src/algorithm/geometry/3d/convex_polyhedron_contains.hpp"
#include "../../src/algorithm/geometry/3d/maximum_empty_sphere.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

long double nearest_distance(
    const Point3& point,
    const std::vector<Point3>& sites
){
    long double result = std::numeric_limits<long double>::infinity();
    for(const Point3& site: sites){
        result = std::min(result, std::hypot(
            point.x - site.x, point.y - site.y, point.z - site.z
        ));
    }
    return result;
}

}  // namespace

int main(){
    return geometry3d_api_test_main([](auto& random, std::size_t rounds){
        const ConvexPolyhedron3 cube = convex_hull_3d({
            {-1, -1, -1}, {-1, -1, 1}, {-1, 1, -1}, {-1, 1, 1},
            {1, -1, -1}, {1, -1, 1}, {1, 1, -1}, {1, 1, 1},
        });
        const Sphere3 one = maximum_empty_sphere({{{0, 0, 0}}}, cube);
        if(!geometry3d_api_close(one.radius, std::sqrt(3.0L), 1e-8L)){
            return false;
        }
        const Sphere3 two = maximum_empty_sphere(
            {{-1, 0, 0}, {1, 0, 0}}, cube
        );
        if(!geometry3d_api_close(two.radius, std::sqrt(3.0L), 1e-8L)
            || !geometry3d_api_close(std::abs(two.center.x), 0.0L, 1e-8L)){
            return false;
        }
        const Sphere3 tetrahedron = maximum_empty_sphere({
            {1, 1, 1}, {1, -1, -1}, {-1, 1, -1}, {-1, -1, 1}
        }, cube);
        if(!geometry3d_api_close(tetrahedron.radius, 2.0L, 1e-8L)){
            return false;
        }

        const long double translation = 1e3000L;
        const long double ulp = std::nextafter(
            translation, std::numeric_limits<long double>::infinity()
        ) - translation;
        const long double upper = translation + 64 * ulp;
        std::vector<Point3> thin_box_vertices;
        for(int x = 0; x < 2; ++x){
            for(int y = 0; y < 2; ++y){
                for(int z = 0; z < 2; ++z){
                    thin_box_vertices.push_back({
                        static_cast<long double>(x),
                        static_cast<long double>(y),
                        z ? upper : translation,
                    });
                }
            }
        }
        const ConvexPolyhedron3 thin_box =
            convex_hull_3d(thin_box_vertices);
        const Sphere3 translated = maximum_empty_sphere({
            {0, 0, translation},
            {1, 0, translation},
            {0, 1, translation},
            {0, 0, upper},
        }, thin_box);
        if(translated.center.x != 0.5L
            || translated.center.y != 0.5L
            || translated.center.z != translation + 32 * ulp
            || translated.radius != 32 * ulp){
            return false;
        }

        std::uniform_real_distribution<long double> coordinate(-0.9L, 0.9L);
        const std::size_t iterations = std::min<std::size_t>(rounds, 20);
        for(std::size_t iteration = 0; iteration < iterations; ++iteration){
            std::vector<Point3> sites;
            const std::size_t size = 1 + random() % 6;
            for(std::size_t index = 0; index < size; ++index){
                sites.push_back({
                    coordinate(random), coordinate(random), coordinate(random)
                });
            }
            const Sphere3 actual = maximum_empty_sphere(sites, cube);
            if(!convex_polyhedron_contains(cube, actual.center)) return false;
            if(!geometry3d_api_close(
                actual.radius, nearest_distance(actual.center, sites), 2e-7L
            )) return false;
            long double grid_lower_bound = 0.0L;
            for(int x = 0; x <= 6; ++x){
                for(int y = 0; y <= 6; ++y){
                    for(int z = 0; z <= 6; ++z){
                        grid_lower_bound = std::max(grid_lower_bound, nearest_distance({
                            -1.0L + static_cast<long double>(x) / 3.0L,
                            -1.0L + static_cast<long double>(y) / 3.0L,
                            -1.0L + static_cast<long double>(z) / 3.0L,
                        }, sites));
                    }
                }
            }
            if(actual.radius + 2e-7L < grid_lower_bound) return false;
        }
        return true;
    });
}
