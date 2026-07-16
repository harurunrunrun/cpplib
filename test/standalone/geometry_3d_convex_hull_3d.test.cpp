// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <set>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/3d/convex_hull_3d.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

bool validates_hull(
    const ConvexPolyhedron3& hull,
    const std::vector<Point3>& points
){
    if(hull.affine_dimension != 3) return false;
    std::set<std::pair<std::size_t, std::size_t>> edges;
    for(const auto& face: hull.faces){
        if(face[0] >= hull.vertices.size() || face[1] >= hull.vertices.size()
            || face[2] >= hull.vertices.size()) return false;
        for(const Point3& point: points){
            if(adaptive_orient3d(
                hull.vertices[face[0]], hull.vertices[face[1]],
                hull.vertices[face[2]], point
            ) > 0) return false;
        }
        for(std::size_t index = 0; index < 3; ++index){
            edges.insert(std::minmax(face[index], face[(index + 1) % 3]));
        }
    }
    return hull.vertices.size() + hull.faces.size() == edges.size() + 2;
}

}  // namespace

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        if(convex_hull_3d({}).affine_dimension != -1
            || convex_hull_3d({{1, 2, 3}, {1, 2, 3}}).affine_dimension != 0){
            return false;
        }
        const auto line = convex_hull_3d({
            {-2, -2, -2}, {0, 0, 0}, {3, 3, 3}, {1, 1, 1}
        });
        if(line.affine_dimension != 1 || line.vertices.size() != 2) return false;
        const auto square = convex_hull_3d({
            {0, 0, 4}, {2, 0, 4}, {2, 2, 4}, {0, 2, 4}, {1, 1, 4}
        });
        if(square.affine_dimension != 2 || square.vertices.size() != 4
            || square.faces.size() != 2) return false;

        if(rounds == 32){
            constexpr std::size_t size = 20000;
            std::vector<Point3> coplanar;
            coplanar.reserve(size);
            for(std::size_t index = 0; index < size; ++index){
                const long double x =
                    static_cast<long double>(index)
                    - static_cast<long double>(size / 2);
                coplanar.push_back({x, x * x, 7.0L});
            }
            const auto large_coplanar_hull = convex_hull_3d(coplanar);
            if(large_coplanar_hull.affine_dimension != 2
                || large_coplanar_hull.vertices.size() != size
                || large_coplanar_hull.faces.size() != size - 2){
                return false;
            }
            for(const Point3& vertex: large_coplanar_hull.vertices){
                if(vertex.z != 7.0L || vertex.y != vertex.x * vertex.x){
                    return false;
                }
            }

            constexpr std::size_t spatial_size = 1000;
            std::vector<Point3> moment_curve;
            moment_curve.reserve(spatial_size);
            for(std::size_t index = 0; index < spatial_size; ++index){
                const long double value = static_cast<long double>(index);
                moment_curve.push_back({
                    value, value * value, value * value * value,
                });
            }
            const auto spatial_hull = convex_hull_3d(moment_curve);
            if(spatial_hull.affine_dimension != 3
                || spatial_hull.vertices.size() != spatial_size
                || spatial_hull.faces.size() != 2 * spatial_size - 4){
                return false;
            }
        }

        const long double maximum = std::numeric_limits<long double>::max();
        const auto extreme_line = convex_hull_3d({
            {-maximum, 0, 0}, {0, 0, 0}, {maximum, 0, 0}
        });
        if(extreme_line.affine_dimension != 1) return false;
        if(extreme_line.vertices.size() != 2) return false;
        const long double translation = 1e3000L;
        const long double local = 1e-3000L;
        const auto tiny_translated_triangle = convex_hull_3d({
            {0, 0, translation},
            {local, 0, translation},
            {0, local, translation},
        });
        if(tiny_translated_triangle.affine_dimension != 2
            || tiny_translated_triangle.vertices.size() != 3
            || tiny_translated_triangle.faces.size() != 1) return false;

        const long double translated_ulp = std::nextafter(
            translation, std::numeric_limits<long double>::infinity()
        ) - translation;
        const std::vector<Point3> thin_tetrahedron{
            {0, 0, translation},
            {local, 0, translation},
            {0, local, translation},
            {0, 0, translation + translated_ulp},
        };
        const auto thin_hull = convex_hull_3d(thin_tetrahedron);
        if(thin_hull.affine_dimension != 3
            || thin_hull.vertices.size() != 4
            || thin_hull.faces.size() != 4
            || !validates_hull(thin_hull, thin_tetrahedron)) return false;


        const std::vector<Point3> cube{
            {-1, -1, -1}, {-1, -1, 1}, {-1, 1, -1}, {-1, 1, 1},
            {1, -1, -1}, {1, -1, 1}, {1, 1, -1}, {1, 1, 1},
            {0, 0, 0}, {1, 1, 1},
        };
        const auto cube_hull = convex_hull_3d(cube);
        if(cube_hull.vertices.size() != 8 || cube_hull.faces.size() != 12
            || !validates_hull(cube_hull, cube)) return false;

        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<Point3> points;
            const std::size_t size = 8 + random() % 20;
            points.reserve(size);
            for(std::size_t index = 0; index < size; ++index){
                points.push_back({
                    static_cast<long double>(static_cast<int>(random() % 41) - 20),
                    static_cast<long double>(static_cast<int>(random() % 41) - 20),
                    static_cast<long double>(static_cast<int>(random() % 41) - 20),
                });
            }
            const auto hull = convex_hull_3d(points);
            if(hull.affine_dimension == 3 && !validates_hull(hull, points)){
                return false;
            }
        }
        return true;
    });
}
