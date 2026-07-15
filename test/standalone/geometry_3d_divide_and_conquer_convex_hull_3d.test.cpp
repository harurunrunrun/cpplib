// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/3d/convex_polyhedron_contains.hpp"
#include "../../src/algorithm/geometry/3d/convex_polyhedron_counts.hpp"
#include "../../src/algorithm/geometry/3d/convex_polyhedron_volume.hpp"
#include "../../src/algorithm/geometry/3d/divide_and_conquer_convex_hull_3d.hpp"
#include "../../src/algorithm/geometry/3d/quickhull_3d.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

bool same_body(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    if(first.affine_dimension != second.affine_dimension) return false;
    for(const Point3& point: first.vertices){
        if(!convex_polyhedron_contains(second, point)) return false;
    }
    for(const Point3& point: second.vertices){
        if(!convex_polyhedron_contains(first, point)) return false;
    }
    if(first.affine_dimension == 3 && !geometry3d_api_close(
        convex_polyhedron_volume(first), convex_polyhedron_volume(second), 1e-8L
    )) return false;
    return true;
}

bool valid_full_dimensional_hull(
    const ConvexPolyhedron3& hull,
    const std::vector<Point3>& input
){
    if(hull.affine_dimension != 3) return false;
    for(const Point3& point: input){
        if(!convex_polyhedron_contains(hull, point)) return false;
    }
    const auto counts = convex_polyhedron_counts(hull);
    return counts.vertices + counts.facet_count == counts.edges + 2;
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
            const auto reference = quickhull_3d(points);
            if(!same_body(actual, reference)) return false;
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
        return same_body(many_hull, quickhull_3d(many_points))
            && valid_full_dimensional_hull(many_hull, many_points);
    });
}
