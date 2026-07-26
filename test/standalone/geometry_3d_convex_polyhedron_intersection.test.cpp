// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>
#include "../../src/algorithm/geometry/3d/convex_polyhedron_point_distance.hpp"

#include "../../src/algorithm/geometry/3d/convex_polyhedron_intersection.hpp"
#include "../../src/algorithm/geometry/3d/convex_polyhedron_volume.hpp"
#include "geometry_3d_api_test_common.hpp"
#include "geometry_3d_convex_polyhedron_test_common.hpp"

namespace{

ConvexPolyhedron3 centered_parallelotope(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    std::vector<Point3> points;
    points.reserve(8);
    for(const long double first_sign: {-1.0L, 1.0L}){
        for(const long double second_sign: {-1.0L, 1.0L}){
            for(const long double third_sign: {-1.0L, 1.0L}){
                points.push_back(
                    first * first_sign
                    + second * second_sign
                    + third * third_sign
                );
            }
        }
    }
    return convex_hull_3d(std::move(points));
}

bool same_convex_set(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    if(first.affine_dimension != second.affine_dimension) return false;
    if(first.vertices.empty() || second.vertices.empty()){
        return first.vertices.empty() && second.vertices.empty();
    }
    for(const Point3& point: first.vertices){
        if(!geometry3d_convex_close(
            convex_polyhedron_point_distance(second, point), 0.0L
        )) return false;
    }
    for(const Point3& point: second.vertices){
        if(!geometry3d_convex_close(
            convex_polyhedron_point_distance(first, point), 0.0L
        )) return false;
    }
    if(first.affine_dimension == 3){
        return geometry3d_convex_close(
            convex_polyhedron_volume(first),
            convex_polyhedron_volume(second),
            2.0e-8L
        );
    }
    return true;
}

long double random_small(std::mt19937_64& random){
    const long long numerator =
        static_cast<long long>(random() % 41) - 20;
    return static_cast<long double>(numerator) / 100.0L;
}

}  // namespace

int main(){
    return geometry3d_api_test_main([](
        std::mt19937_64& random,
        std::size_t rounds
    ){
        const ConvexPolyhedron3 first_cube = geometry3d_test_cube();
        const ConvexPolyhedron3 second_cube =
            geometry3d_test_cube({1, 0, 0});
        const ConvexPolyhedron3 overlap = convex_polyhedron_intersection(
            first_cube, second_cube
        );
        if(!geometry3d_convex_close(
            convex_polyhedron_volume(overlap), 4.0L
        )) return false;

        const auto fast_with_interior =
            convex_polyhedron_intersection_via_halfspaces_with_interior_point(
                first_cube, second_cube, {0.5L, 0.0L, 0.0L}
            );
        const auto fast =
            convex_polyhedron_intersection_via_halfspaces(
                first_cube, second_cube
            );
        if(!same_convex_set(overlap, fast_with_interior)
            || !same_convex_set(overlap, fast)){
            return false;
        }

        const auto touching = convex_polyhedron_intersection_via_halfspaces(
            first_cube, geometry3d_test_cube({2, 0, 0})
        );
        const auto touching_oracle = convex_polyhedron_intersection(
            first_cube, geometry3d_test_cube({2, 0, 0})
        );
        if(!same_convex_set(touching, touching_oracle)
            || touching.affine_dimension != 2){
            return false;
        }

        const auto disjoint = convex_polyhedron_intersection_via_halfspaces(
            first_cube, geometry3d_test_cube({3, 0, 0})
        );
        if(!disjoint.vertices.empty() || disjoint.affine_dimension != -1){
            return false;
        }

        const std::size_t random_rounds = std::min(
            rounds, static_cast<std::size_t>(12)
        );
        for(std::size_t iteration = 0;
            iteration < random_rounds;
            ++iteration){
            const ConvexPolyhedron3 first = centered_parallelotope(
                {1.4L + random_small(random), random_small(random), random_small(random)},
                {random_small(random), 1.3L + random_small(random), random_small(random)},
                {random_small(random), random_small(random), 1.2L + random_small(random)}
            );
            const ConvexPolyhedron3 second = centered_parallelotope(
                {1.1L + random_small(random), 0.3L + random_small(random), random_small(random)},
                {-0.2L + random_small(random), 1.4L + random_small(random), random_small(random)},
                {random_small(random), 0.2L + random_small(random), 1.3L + random_small(random)}
            );
            const ConvexPolyhedron3 oracle =
                convex_polyhedron_intersection(first, second);
            const ConvexPolyhedron3 generic =
                convex_polyhedron_intersection_via_halfspaces(first, second);
            const ConvexPolyhedron3 explicit_interior =
                convex_polyhedron_intersection_via_halfspaces_with_interior_point(
                    first, second, {0.0L, 0.0L, 0.0L}
                );
            if(!same_convex_set(oracle, generic)
                || !same_convex_set(oracle, explicit_interior)){
                return false;
            }
        }

        bool threw = false;
        try{
            static_cast<void>(
                convex_polyhedron_intersection_via_halfspaces_with_interior_point(
                    first_cube, second_cube, {0.0L, 0.0L, 0.0L}
                )
            );
        }catch(const std::invalid_argument&){
            threw = true;
        }
        if(!threw) return false;

        ConvexPolyhedron3 invalid = first_cube;
        invalid.faces.front()[0] = invalid.vertices.size();
        threw = false;
        try{
            static_cast<void>(
                convex_polyhedron_intersection_via_halfspaces(
                    invalid, second_cube
                )
            );
        }catch(const std::invalid_argument&){
            threw = true;
        }
        if(!threw) return false;

        if(rounds == 32){
            constexpr std::size_t side_count = 48;
            const long double pi = std::acos(-1.0L);
            std::vector<Point3> prism_points;
            prism_points.reserve(2 * side_count);
            for(std::size_t index = 0; index < side_count; ++index){
                const long double angle =
                    2.0L * pi * static_cast<long double>(index)
                    / static_cast<long double>(side_count);
                for(const long double z: {-1.0L, 1.0L}){
                    prism_points.push_back({
                        std::cos(angle), std::sin(angle), z
                    });
                }
            }
            const ConvexPolyhedron3 prism =
                convex_hull_3d(std::move(prism_points));
            const ConvexPolyhedron3 prism_intersection =
                convex_polyhedron_intersection_via_halfspaces_with_interior_point(
                    prism, prism, {0.0L, 0.0L, 0.0L}
                );
            if(!same_convex_set(prism, prism_intersection)){
                return false;
            }
        }
        return true;
    });
}
