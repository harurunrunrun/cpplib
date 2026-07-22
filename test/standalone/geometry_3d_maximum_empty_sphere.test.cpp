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
Sphere3 exhaustive_candidate_sphere(
    const std::vector<Point3>& input_sites,
    const ConvexPolyhedron3& bounds
){
    const VoronoiDiagram3 voronoi = voronoi_diagram_3d(input_sites);
    const std::vector<Point3>& sites = voronoi.sites;
    Sphere3 best{{}, -1.0L};
    const auto consider = [&](const Point3& point){
        if(!convex_polyhedron_contains(bounds, point)) return;
        const long double radius = nearest_distance(point, sites);
        if(radius > best.radius) best = {point, radius};
    };
    for(const Point3& point: bounds.vertices) consider(point);
    for(const Point3& point: voronoi.finite_vertices) consider(point);

    for(const auto& edge: convex_polyhedron_edges(bounds)){
        const Segment3 segment{
            bounds.vertices[edge[0]], bounds.vertices[edge[1]]
        };
        for(std::size_t first = 0; first < sites.size(); ++first){
            for(std::size_t second = first + 1;
                second < sites.size(); ++second){
                const auto point = segment_plane_intersection(
                    segment,
                    maximum_empty_sphere_detail::bisector_plane(
                        sites[first], sites[second]
                    )
                );
                if(point) consider(*point);
            }
        }
    }

    for(const auto& face: bounds.faces){
        const Point3& origin = bounds.vertices[face[0]];
        const Plane3 plane{
            origin,
            cross(
                geometry3d_normalized_difference(
                    bounds.vertices[face[1]], origin
                ).value,
                geometry3d_normalized_difference(
                    bounds.vertices[face[2]], origin
                ).value
            ),
        };
        for(std::size_t first = 0; first < sites.size(); ++first){
            for(std::size_t second = first + 1;
                second < sites.size(); ++second){
                for(std::size_t third = second + 1;
                    third < sites.size(); ++third){
                    const ThreePlaneIntersection3 intersection =
                        three_plane_intersection(
                            plane,
                            maximum_empty_sphere_detail::bisector_plane(
                                sites[first], sites[second]
                            ),
                            maximum_empty_sphere_detail::bisector_plane(
                                sites[first], sites[third]
                            )
                        );
                    if(const Point3* point =
                        std::get_if<Point3>(&intersection)){
                        consider(*point);
                    }
                }
            }
        }
    }
    return best;
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
        const std::vector<Point3> coplanar_sites{
            {-0.6L, -0.6L, 0}, {-0.6L, 0.6L, 0},
            {0.6L, -0.6L, 0}, {0.6L, 0.6L, 0},
        };
        const Sphere3 coplanar =
            maximum_empty_sphere(coplanar_sites, cube);
        const Sphere3 coplanar_oracle =
            exhaustive_candidate_sphere(coplanar_sites, cube);
        if(!geometry3d_api_close(
            coplanar.radius, coplanar_oracle.radius, 2e-7L
        )) return false;
        const Sphere3 duplicates = maximum_empty_sphere({
            {-0.5L, 0, 0}, {-0.5L, 0, 0},
            {0.5L, 0, 0}, {0.5L, 0, 0},
        }, cube);
        if(!geometry3d_api_close(
            duplicates.radius, 1.5L, 2e-7L
        )) return false;

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
            const Sphere3 expected =
                exhaustive_candidate_sphere(sites, cube);
            if(!geometry3d_api_close(
                actual.radius, expected.radius, 2e-7L)) return false;
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
        if(rounds == 32){
            constexpr std::size_t large_size = 320;
            std::vector<Point3> large_sites;
            large_sites.reserve(large_size);
            for(std::size_t index = 0; index < large_size; ++index){
                const long double value =
                    static_cast<long double>(index + 1);
                large_sites.push_back({
                    std::sin(value * 1.731L) * 0.95L,
                    std::sin(value * value * 0.417L) * 0.95L,
                    std::cos(value * 2.319L) * 0.95L,
                });
            }
            const Sphere3 large =
                maximum_empty_sphere(large_sites, cube);
            if(!convex_polyhedron_contains(cube, large.center)
                || !geometry3d_api_close(
                    large.radius,
                    nearest_distance(large.center, large_sites),
                    2e-7L
                )){
                return false;
            }

            std::vector<Point3> sites;
            sites.reserve(32);
            for(std::size_t index = 0; index < 32; ++index){
                const long double value =
                    static_cast<long double>(index + 1);
                sites.push_back({
                    std::sin(value * 1.731L) * 0.9L,
                    std::sin(value * value * 0.417L) * 0.9L,
                    std::cos(value * 2.319L) * 0.9L,
                });
            }
            const Sphere3 stress = maximum_empty_sphere(sites, cube);
            if(!convex_polyhedron_contains(cube, stress.center)
                || !geometry3d_api_close(
                    stress.radius,
                    nearest_distance(stress.center, sites),
                    2e-7L
                )){
                return false;
            }
        }
        return true;
    });
}
