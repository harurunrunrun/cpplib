// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <limits>
#include <set>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/3d/shape/quickhull_3d.hpp"
#include "../../src/algorithm/geometry/3d/scalar/convex_polyhedron_volume.hpp"
#include "geometry_3d_api_test_common.hpp"


bool moment_curve_stress_test(){
    constexpr std::size_t count = 1200;
    std::vector<Point3> points;
    points.reserve(count);
    for(std::size_t index = 0; index < count; ++index){
        const long double parameter =
            static_cast<long double>(index) - count / 2;
        points.push_back({
            parameter,
            parameter * parameter,
            parameter * parameter * parameter,
        });
    }
    const ConvexPolyhedron3 hull = quickhull_3d(points);
    if(hull.affine_dimension != 3 || hull.vertices != points) return false;
    if(hull.faces.size() != 2 * count - 4) return false;

    std::set<std::array<std::size_t, 3>> expected;
    for(std::size_t index = 1; index + 1 < count; ++index){
        expected.insert({0, index, index + 1});
    }
    for(std::size_t index = 0; index + 2 < count; ++index){
        expected.insert({index, index + 1, count - 1});
    }
    std::set<std::array<std::size_t, 3>> actual;
    for(auto face: hull.faces){
        std::sort(face.begin(), face.end());
        actual.insert(face);
    }
    return actual == expected;
}

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        if(rounds == 9999) return moment_curve_stress_test();
        if(quickhull_3d({}).affine_dimension != -1) return false;
        if(quickhull_3d({{0, 0, 0}, {1, 1, 1}}).affine_dimension != 1) return false;
        const ConvexPolyhedron3 coplanar = quickhull_3d({
            {0, 0, 0}, {2, 0, 0}, {2, 2, 0}, {0, 2, 0},
            {1, 1, 0}, {0, 0, 0},
        });
        if(coplanar.affine_dimension != 2 || coplanar.vertices.size() != 4){
            return false;
        }
        const ConvexPolyhedron3 cube = quickhull_3d({
            {-1, -1, -1}, {-1, -1, 1}, {-1, 1, -1}, {-1, 1, 1},
            {1, -1, -1}, {1, -1, 1}, {1, 1, -1}, {1, 1, 1},
            {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0},
            {0, 0, -1}, {0, 0, 1},
            {-1, -1, 0}, {-1, 1, 0}, {1, -1, 0}, {1, 1, 0},
            {-1, 0, -1}, {1, 0, 1}, {0, 0, 0},
        });
        if(cube.affine_dimension != 3 || cube.vertices.size() != 8
            || cube.faces.size() != 12){
            return false;
        }
        bool rejected = false;
        try{
            static_cast<void>(quickhull_3d({{
                std::numeric_limits<long double>::infinity(), 0, 0,
            }}));
        }catch(const std::invalid_argument&){
            rejected = true;
        }
        if(!rejected) return false;
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<Point3> points;
            const std::size_t count = 8 + random() % 20;
            for(std::size_t index = 0; index < count; ++index){
                points.push_back({
                    static_cast<long double>(static_cast<int>(random() % 31) - 15),
                    static_cast<long double>(static_cast<int>(random() % 31) - 15),
                    static_cast<long double>(static_cast<int>(random() % 31) - 15),
                });
            }
            const ConvexPolyhedron3 actual = quickhull_3d(points);
            const ConvexPolyhedron3 expected = convex_hull_3d(points);
            std::vector<Point3> permuted = points;
            std::shuffle(permuted.begin(), permuted.end(), random);
            const ConvexPolyhedron3 permuted_hull = quickhull_3d(permuted);
            if(actual.affine_dimension != permuted_hull.affine_dimension
                || actual.vertices != permuted_hull.vertices
                || actual.faces != permuted_hull.faces){
                return false;
            }
            if(actual.affine_dimension != expected.affine_dimension) return false;
            if(actual.affine_dimension != 3){
                auto actual_vertices = actual.vertices;
                auto expected_vertices = expected.vertices;
                std::sort(actual_vertices.begin(), actual_vertices.end());
                std::sort(expected_vertices.begin(), expected_vertices.end());
                if(actual_vertices != expected_vertices) return false;
                continue;
            }
            for(const auto& face: actual.faces){
                for(const Point3& point: points){
                    if(adaptive_orient3d(
                        actual.vertices[face[0]], actual.vertices[face[1]],
                        actual.vertices[face[2]], point
                    ) > 0) return false;
                }
            }
            if(!geometry3d_api_close(
                convex_polyhedron_volume(actual),
                convex_polyhedron_volume(expected), 1.0e-8L
            )) return false;
        }
        return true;
    });
}
