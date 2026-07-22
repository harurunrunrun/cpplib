// competitive-verifier: STANDALONE

#include <algorithm>
#include <map>
#include <utility>
#include <cstdint>

#include "../../src/algorithm/geometry/3d/minkowski_sum_3d.hpp"
#include "../../src/algorithm/geometry/3d/convex_polyhedron_point_distance.hpp"
#include "../../src/algorithm/geometry/3d/convex_polyhedron_volume.hpp"
#include "geometry_3d_collision_test_common.hpp"

namespace{

ConvexPolyhedron3 random_strict_polyhedron(
    std::mt19937_64& random,
    std::size_t count,
    const Point3& center
){
    std::vector<Point3> points;
    points.reserve(count);
    while(points.size() < count){
        Point3 direction = geometry3d_random_point(random, -1.0L, 1.0L);
        const long double length = std::hypot(
            direction.x, direction.y, direction.z
        );
        if(length <= 0.1L) continue;
        points.push_back(center + direction / length);
    }
    return convex_hull_3d(std::move(points));
}

bool is_closed_oriented_manifold(const ConvexPolyhedron3& polyhedron){
    std::map<std::pair<std::size_t, std::size_t>, std::pair<int, int>> edges;
    for(const auto& face: polyhedron.faces){
        for(std::size_t index = 0; index < 3; ++index){
            std::size_t from = face[index];
            std::size_t to = face[(index + 1) % 3];
            const int orientation = from < to ? 1 : -1;
            if(to < from) std::swap(from, to);
            auto& use = edges[{from, to}];
            ++use.first;
            use.second += orientation;
        }
    }
    for(const auto& [edge, use]: edges){
        static_cast<void>(edge);
        if(use.first != 2 || use.second != 0) return false;
    }
    return convex_polyhedron_signed_volume(polyhedron) > 0.0L
        && polyhedron.vertices.size() + polyhedron.faces.size()
            == edges.size() + 2;
}

bool same_convex_body(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    if(!geometry3d_api_close(
        convex_polyhedron_volume(first),
        convex_polyhedron_volume(second),
        1.0e-7L
    )) return false;
    for(const Point3& point: first.vertices){
        if(!geometry3d_api_close(
            convex_polyhedron_point_distance(second, point),
            0.0L,
            1.0e-7L
        )) return false;
    }
    for(const Point3& point: second.vertices){
        if(!geometry3d_api_close(
            convex_polyhedron_point_distance(first, point),
            0.0L,
            1.0e-7L
        )) return false;
    }
    return true;
}
ConvexPolyhedron3 parallelotope(
    const Point3& center,
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    std::vector<Point3> points;
    for(int x: {-1, 1}) for(int y: {-1, 1}) for(int z: {-1, 1}){
        points.push_back(
            center + static_cast<long double>(x) * first
            + static_cast<long double>(y) * second
            + static_cast<long double>(z) * third
        );
    }
    return convex_hull_3d(std::move(points));
}

}  // namespace

int main(){
    return geometry3d_api_test_main([](
        std::mt19937_64& random,
        std::size_t rounds
    ){
        const ConvexPolyhedron3 tetra = geometry3d_collision_tetra();
        const ConvexPolyhedron3 doubled = minkowski_sum_3d(tetra, tetra);
        const ConvexPolyhedron3 seeded_doubled =
            minkowski_sum_3d_with_seed(tetra, tetra, 0x123456789abcdef0ULL);
        if(doubled.affine_dimension != 3
            || seeded_doubled.affine_dimension != 3
            || doubled.vertices.size() != seeded_doubled.vertices.size()){
            return false;
        }

        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 first_center =
                geometry3d_random_point(random, -5, 5);
            const Point3 second_center =
                geometry3d_random_point(random, -5, 5);
            const Point3 first_half =
                geometry3d_random_point(random, 0.1L, 2);
            const Point3 second_half =
                geometry3d_random_point(random, 0.1L, 2);
            const ConvexPolyhedron3 first =
                geometry3d_collision_box(first_center, first_half);
            const ConvexPolyhedron3 second =
                geometry3d_collision_box(second_center, second_half);
            const ConvexPolyhedron3 sum = iteration % 2 == 0
                ? minkowski_sum_3d(first, second)
                : minkowski_sum_3d_with_seed(
                    first, second, static_cast<std::uint64_t>(random())
                );

            Point3 minimum = sum.vertices.front();
            Point3 maximum = sum.vertices.front();
            for(const Point3& point: sum.vertices){
                minimum.x = std::min(minimum.x, point.x);
                minimum.y = std::min(minimum.y, point.y);
                minimum.z = std::min(minimum.z, point.z);
                maximum.x = std::max(maximum.x, point.x);
                maximum.y = std::max(maximum.y, point.y);
                maximum.z = std::max(maximum.z, point.z);
            }
            const Point3 expected_center = first_center + second_center;
            const Point3 expected_half = first_half + second_half;
            if(!geometry3d_api_close(
                    minimum, expected_center - expected_half
                )
                || !geometry3d_api_close(
                    maximum, expected_center + expected_half
                )){
                return false;
            }
        }
        const std::size_t direct_rounds = std::min<std::size_t>(rounds, 24);
        for(std::size_t iteration = 0; iteration < direct_rounds; ++iteration){
            const ConvexPolyhedron3 first = random_strict_polyhedron(
                random, 6 + iteration % 4,
                geometry3d_random_point(random, -2.0L, 2.0L)
            );
            const ConvexPolyhedron3 second = random_strict_polyhedron(
                random, 7 + iteration % 5,
                geometry3d_random_point(random, -2.0L, 2.0L)
            );
            const ConvexPolyhedron3 expected =
                minkowski_sum_3d_with_seed(first, second, random());
            const ConvexPolyhedron3 direct =
                minkowski_sum_3d_direct(first, second);
            if(!is_closed_oriented_manifold(direct)
                || !same_convex_body(expected, direct)){
                return false;
            }
        }
        const ConvexPolyhedron3 triangulated_facets_first = parallelotope(
            {}, {2, 0, 0}, {0, 3, 0}, {0, 0, 5}
        );
        const ConvexPolyhedron3 triangulated_facets_second = parallelotope(
            {1, -2, 3}, {1, 2, 3}, {-2, 1, 4},
            {3, -4, 2}
        );
        const ConvexPolyhedron3 triangulated_expected = minkowski_sum_3d(
            triangulated_facets_first, triangulated_facets_second
        );
        const ConvexPolyhedron3 triangulated_direct = minkowski_sum_3d_direct(
            triangulated_facets_first, triangulated_facets_second
        );
        if(!same_convex_body(triangulated_expected, triangulated_direct)){
            return false;
        }

        const ConvexPolyhedron3 performance_first =
            random_strict_polyhedron(random, 40, {});
        const ConvexPolyhedron3 performance_second =
            random_strict_polyhedron(random, 43, {3, -2, 1});
        const ConvexPolyhedron3 performance = minkowski_sum_3d_direct(
            performance_first, performance_second
        );
        if(!is_closed_oriented_manifold(performance)) return false;

        bool direct_rejected_degeneracy = false;
        try{
            static_cast<void>(minkowski_sum_3d_direct(tetra, tetra));
        }catch(const std::domain_error&){
            direct_rejected_degeneracy = true;
        }
        if(!direct_rejected_degeneracy) return false;


        bool threw = false;
        try{
            static_cast<void>(
                minkowski_sum_3d_with_seed({}, tetra, 0)
            );
        }catch(const std::invalid_argument&){
            threw = true;
        }
        return threw;
    });
}
