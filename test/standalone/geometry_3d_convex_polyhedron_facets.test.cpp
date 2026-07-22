// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/3d/shape/convex_hull_3d.hpp"
#include "../../src/algorithm/geometry/3d/query/convex_polyhedron_facets.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

using CanonicalFacet = std::pair<
    std::vector<std::size_t>,
    std::vector<std::array<std::size_t, 3>>
>;

std::vector<CanonicalFacet> canonical_facets(
    const ConvexPolyhedron3& polyhedron,
    const std::vector<ConvexPolyhedronFacet3>& facets
){
    std::vector<CanonicalFacet> result;
    result.reserve(facets.size());
    for(const ConvexPolyhedronFacet3& facet: facets){
        std::vector<std::size_t> boundary = facet.boundary;
        std::sort(boundary.begin(), boundary.end());
        std::vector<std::array<std::size_t, 3>> triangles;
        triangles.reserve(facet.triangles.size());
        for(const std::size_t index: facet.triangles){
            if(index >= polyhedron.faces.size()) return {};
            std::array<std::size_t, 3> triangle = polyhedron.faces[index];
            std::sort(triangle.begin(), triangle.end());
            triangles.push_back(triangle);
        }
        std::sort(triangles.begin(), triangles.end());
        result.emplace_back(std::move(boundary), std::move(triangles));
    }
    std::sort(result.begin(), result.end());
    return result;
}

bool throws_domain_error(const ConvexPolyhedron3& polyhedron){
    try{
        static_cast<void>(convex_polyhedron_facets(polyhedron));
    }catch(const std::domain_error&){
        return true;
    }
    return false;
}

}  // namespace

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

        const ConvexPolyhedron3 square{
            2,
            {{0, 0, 0}, {2, 0, 0}, {2, 2, 0}, {0, 2, 0}},
            {{0, 1, 2}, {0, 2, 3}},
        };
        const auto square_facets = convex_polyhedron_facets(square);
        if(square_facets.size() != 1
            || square_facets.front().boundary.size() != 4
            || square_facets.front().triangles.size() != 2){
            return false;
        }

        ConvexPolyhedron3 open_cube = cube;
        open_cube.faces.pop_back();
        if(!throws_domain_error(open_cube)) return false;

        ConvexPolyhedron3 inconsistent_cube = cube;
        std::swap(
            inconsistent_cube.faces.front()[1],
            inconsistent_cube.faces.front()[2]
        );
        if(!throws_domain_error(inconsistent_cube)) return false;

        const ConvexPolyhedron3 non_manifold{
            2,
            {{0, 0, 0}, {1, 0, 0}, {0, 1, 0},
             {0, -1, 0}, {2, 1, 0}},
            {{0, 1, 2}, {1, 0, 3}, {0, 1, 4}},
        };
        if(!throws_domain_error(non_manifold)) return false;

        std::vector<Point3> fan_vertices{{0, 0, 0}};
        constexpr std::size_t fan_size = 2048;
        fan_vertices.reserve(fan_size + 1);
        for(std::size_t index = 0; index < fan_size; ++index){
            const long double angle = 2.0L * std::acos(-1.0L)
                * static_cast<long double>(index)
                / static_cast<long double>(fan_size);
            fan_vertices.push_back({
                std::cos(angle), std::sin(angle), 0
            });
        }
        std::vector<std::array<std::size_t, 3>> fan_triangles;
        fan_triangles.reserve(fan_size);
        for(std::size_t index = 0; index < fan_size; ++index){
            fan_triangles.push_back({
                0, index + 1, (index + 1) % fan_size + 1
            });
        }
        const auto fan_facets = convex_polyhedron_facets({
            2, std::move(fan_vertices), std::move(fan_triangles)
        });
        if(fan_facets.size() != 1
            || fan_facets.front().boundary.size() != fan_size
            || fan_facets.front().triangles.size() != fan_size){
            return false;
        }

        const auto expected_cube = canonical_facets(cube, cube_facets);
        for(std::size_t iteration = 0; iteration < 32; ++iteration){
            ConvexPolyhedron3 shuffled = cube;
            std::shuffle(shuffled.faces.begin(), shuffled.faces.end(), random);
            for(auto& face: shuffled.faces){
                const std::size_t rotation = random() % 3;
                std::rotate(
                    face.begin(),
                    face.begin() + static_cast<std::ptrdiff_t>(rotation),
                    face.end()
                );
            }
            if(canonical_facets(
                shuffled, convex_polyhedron_facets(shuffled)
            ) != expected_cube){
                return false;
            }
        }

        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<Point3> points;
            const std::size_t count = 8 + random() % 32;
            for(std::size_t index = 0; index < count; ++index){
                points.push_back({
                    static_cast<long double>(
                        static_cast<int>(random() % 41) - 20
                    ),
                    static_cast<long double>(
                        static_cast<int>(random() % 41) - 20
                    ),
                    static_cast<long double>(
                        static_cast<int>(random() % 41) - 20
                    ),
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
