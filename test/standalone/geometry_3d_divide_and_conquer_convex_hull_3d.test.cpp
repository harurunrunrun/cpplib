// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/3d/convex_polyhedron_contains.hpp"
#include "../../src/algorithm/geometry/3d/convex_polyhedron_counts.hpp"
#include "../../src/algorithm/geometry/3d/divide_and_conquer_convex_hull_3d.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

bool exactly_equal(const Point3& left, const Point3& right){
    return left.x == right.x && left.y == right.y && left.z == right.z;
}

bool valid_full_dimensional_hull(
    const ConvexPolyhedron3& hull,
    const std::vector<Point3>& input
){
    if(hull.affine_dimension != 3) return false;
    for(const Point3& vertex: hull.vertices){
        if(std::none_of(input.begin(), input.end(), [&](const Point3& point){
            return exactly_equal(vertex, point);
        })) return false;
    }
    std::map<std::array<std::size_t, 2>, std::size_t> edge_uses;
    for(const auto& face: hull.faces){
        for(std::size_t vertex: face){
            if(vertex >= hull.vertices.size()) return false;
        }
        if(face[0] == face[1] || face[1] == face[2] || face[2] == face[0]){
            return false;
        }
        for(const Point3& point: input){
            if(adaptive_orient3d(
                hull.vertices[face[0]], hull.vertices[face[1]],
                hull.vertices[face[2]], point
            ) > 0) return false;
        }
        for(std::size_t edge = 0; edge < 3; ++edge){
            std::array<std::size_t, 2> key{
                face[edge], face[(edge + 1) % 3]
            };
            if(key[1] < key[0]) std::swap(key[0], key[1]);
            ++edge_uses[key];
        }
    }
    for(const auto& [edge, uses]: edge_uses){
        static_cast<void>(edge);
        if(uses != 2) return false;
    }
    for(const Point3& point: input){
        if(!convex_polyhedron_contains(hull, point)) return false;
    }
    return hull.vertices.size() + hull.faces.size()
        == edge_uses.size() + 2;
}

}  // namespace

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random,
                                       std::size_t rounds){
        if(divide_and_conquer_convex_hull_3d({}).affine_dimension != -1){
            return false;
        }
        if(divide_and_conquer_convex_hull_3d({{2, 3, 5}, {2, 3, 5}})
            .affine_dimension != 0) return false;

        const auto line = divide_and_conquer_convex_hull_3d({
            {3, 6, 9}, {-2, -4, -6}, {0, 0, 0}, {1, 2, 3}, {1, 2, 3},
        });
        if(line.affine_dimension != 1 || line.vertices.size() != 2) return false;

        const auto plane = divide_and_conquer_convex_hull_3d({
            {-2, -2, 7}, {-1, -1, 7}, {0, -2, 7}, {2, -2, 7},
            {2, 2, 7}, {0, 0, 7}, {-2, 2, 7}, {-2, 0, 7},
        });
        if(plane.affine_dimension != 2 || plane.vertices.size() != 4
            || plane.faces.size() != 2) return false;

        std::vector<Point3> cube;
        for(int x: {-1, 1}) for(int y: {-1, 1}) for(int z: {-1, 1}){
            cube.push_back({
                static_cast<long double>(x), static_cast<long double>(y),
                static_cast<long double>(z)
            });
        }
        cube.insert(cube.end(), {
            {0, 0, -1}, {0, 0, 1}, {0, -1, 0}, {0, 1, 0},
            {-1, 0, 0}, {1, 0, 0}, {0, 0, 0},
        });
        const auto cube_hull = divide_and_conquer_convex_hull_3d(cube);
        const auto cube_counts = convex_polyhedron_counts(cube_hull);
        if(cube_counts.vertices != 8 || cube_counts.edges != 12
            || cube_counts.facet_count != 6
            || cube_counts.triangulated_face_count != 12) return false;

        for(long double scale: {1e-2000L, 1e2000L}){
            std::vector<Point3> tetrahedron{
                {scale, scale, scale}, {-scale, -scale, scale},
                {-scale, scale, -scale}, {scale, -scale, -scale},
                {0, 0, 0},
            };
            const auto hull = divide_and_conquer_convex_hull_3d(tetrahedron);
            if(!valid_full_dimensional_hull(hull, tetrahedron)
                || hull.vertices.size() != 4) return false;
        }

        bool threw = false;
        try{
            static_cast<void>(divide_and_conquer_convex_hull_3d({
                {0, 0, std::numeric_limits<long double>::infinity()}
            }));
        }catch(const std::invalid_argument&){
            threw = true;
        }
        if(!threw) return false;

        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<Point3> points;
            const std::size_t count = 6 + random() % 30;
            for(std::size_t index = 0; index < count; ++index){
                points.push_back({
                    static_cast<long double>(static_cast<int>(random() % 61) - 30),
                    static_cast<long double>(static_cast<int>(random() % 61) - 30),
                    static_cast<long double>(static_cast<int>(random() % 61) - 30),
                });
                if(index % 11 == 0) points.push_back(points.back());
            }
            const auto actual = divide_and_conquer_convex_hull_3d(points);
            if(actual.affine_dimension == 3
                && !valid_full_dimensional_hull(actual, points)) return false;
        }

        std::vector<Point3> many_points;
        for(std::size_t index = 0; index < 128; ++index){
            many_points.push_back({
                static_cast<long double>(static_cast<int>(random() % 101) - 50),
                static_cast<long double>(static_cast<int>(random() % 101) - 50),
                static_cast<long double>(static_cast<int>(random() % 101) - 50),
            });
        }
        const auto many_hull = divide_and_conquer_convex_hull_3d(many_points);
        if(!valid_full_dimensional_hull(many_hull, many_points)) return false;

        // The generator reserves rounds == 1 for a large input.  All generated
        // points are strictly inside the cube, so the eight corners form an
        // independent expected hull while the former cubic merge is infeasible.
        if(rounds == 1){
            constexpr long double radius = 1000000.0L;
            std::vector<Point3> performance;
            for(int x: {-1, 1}) for(int y: {-1, 1}) for(int z: {-1, 1}){
                performance.push_back({x * radius, y * radius, z * radius});
            }
            for(std::size_t index = 0; index < 50000; ++index){
                const auto coordinate = [&]{
                    return static_cast<long double>(
                        static_cast<long long>(random() % 1999999) - 999999
                    );
                };
                performance.push_back({coordinate(), coordinate(), coordinate()});
            }
            const auto large_hull =
                divide_and_conquer_convex_hull_3d(performance);
            const auto counts = convex_polyhedron_counts(large_hull);
            if(counts.vertices != 8 || counts.edges != 12
                || counts.facet_count != 6
                || !valid_full_dimensional_hull(large_hull, performance)){
                return false;
            }
        }
        return true;
    });
}
