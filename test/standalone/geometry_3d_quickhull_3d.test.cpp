// competitive-verifier: STANDALONE

#include <algorithm>
#include <vector>

#include "../../src/algorithm/geometry/3d/quickhull_3d.hpp"
#include "../../src/algorithm/geometry/3d/convex_polyhedron_volume.hpp"
#include "geometry_3d_api_test_common.hpp"


int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        if(quickhull_3d({{0, 0, 0}, {1, 1, 1}}).affine_dimension != 1) return false;
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
