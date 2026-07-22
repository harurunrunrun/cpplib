// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <vector>

#include "../../src/algorithm/geometry/3d/shape/convex_hull_3d.hpp"
#include "../../src/algorithm/geometry/3d/query/convex_polyhedron_facets.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random,
                                       std::size_t rounds){
        const std::vector<Point3> cube_points{
            {-1, -1, -1}, {-1, -1, 1}, {-1, 1, -1}, {-1, 1, 1},
            {1, -1, -1}, {1, -1, 1}, {1, 1, -1}, {1, 1, 1},
        };
        const auto cube = convex_hull_3d(cube_points);
        const auto cube_facets = convex_polyhedron_facets(cube);
        if(cube_facets.size() != 6) return false;
        for(const auto& facet: cube_facets){
            if(facet.boundary.size() != 4 || facet.triangles.size() != 2){
                return false;
            }
        }

        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<Point3> points;
            const std::size_t count = 8 + random() % 32;
            for(std::size_t index = 0; index < count; ++index){
                points.push_back({
                    static_cast<long double>(static_cast<int>(random() % 41) - 20),
                    static_cast<long double>(static_cast<int>(random() % 41) - 20),
                    static_cast<long double>(static_cast<int>(random() % 41) - 20),
                });
            }
            const auto hull = convex_hull_3d(points);
            const auto facets = convex_polyhedron_facets(hull);
            std::vector<std::size_t> seen(hull.faces.size());
            for(const auto& facet: facets){
                if(facet.boundary.size() < 3) return false;
                for(std::size_t triangle: facet.triangles){
                    if(triangle >= seen.size() || seen[triangle]++) return false;
                }
            }
            if(!std::all_of(seen.begin(), seen.end(), [](std::size_t value){
                return value == 1;
            })) return false;
        }
        return true;
    });
}
