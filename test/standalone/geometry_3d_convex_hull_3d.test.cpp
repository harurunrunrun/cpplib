// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <set>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/3d/shape/convex_hull_3d.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

using IntegerPoint3 = std::array<long long, 3>;

int integer_orient3d(
    const IntegerPoint3& first,
    const IntegerPoint3& second,
    const IntegerPoint3& third,
    const IntegerPoint3& fourth
){
    const __int128 ax = second[0] - first[0];
    const __int128 ay = second[1] - first[1];
    const __int128 az = second[2] - first[2];
    const __int128 bx = third[0] - first[0];
    const __int128 by = third[1] - first[1];
    const __int128 bz = third[2] - first[2];
    const __int128 cx = fourth[0] - first[0];
    const __int128 cy = fourth[1] - first[1];
    const __int128 cz = fourth[2] - first[2];
    const __int128 determinant =
        ax * (by * cz - bz * cy)
        - ay * (bx * cz - bz * cx)
        + az * (bx * cy - by * cx);
    return (determinant > 0) - (determinant < 0);
}

bool integer_non_collinear(
    const IntegerPoint3& first,
    const IntegerPoint3& second,
    const IntegerPoint3& third
){
    const __int128 ax = second[0] - first[0];
    const __int128 ay = second[1] - first[1];
    const __int128 az = second[2] - first[2];
    const __int128 bx = third[0] - first[0];
    const __int128 by = third[1] - first[1];
    const __int128 bz = third[2] - first[2];
    return ay * bz != az * by || az * bx != ax * bz
        || ax * by != ay * bx;
}

bool can_append_general_position(
    const std::vector<IntegerPoint3>& points,
    const IntegerPoint3& candidate
){
    if(std::find(points.begin(), points.end(), candidate) != points.end()){
        return false;
    }
    for(std::size_t first = 0; first < points.size(); ++first){
        for(std::size_t second = first + 1; second < points.size(); ++second){
            if(!integer_non_collinear(
                points[first], points[second], candidate
            )) return false;
            for(std::size_t third = second + 1; third < points.size(); ++third){
                if(integer_orient3d(
                    points[first], points[second], points[third], candidate
                ) == 0) return false;
            }
        }
    }
    return true;
}

bool agrees_with_combination_oracle(
    const std::vector<IntegerPoint3>& integer_points,
    std::uint64_t seed
){
    std::set<std::array<std::size_t, 3>> expected;
    for(std::size_t first = 0; first < integer_points.size(); ++first){
        for(std::size_t second = first + 1;
            second < integer_points.size(); ++second){
            for(std::size_t third = second + 1;
                third < integer_points.size(); ++third){
                bool positive = false;
                bool negative = false;
                for(std::size_t point = 0; point < integer_points.size(); ++point){
                    if(point == first || point == second || point == third){
                        continue;
                    }
                    const int side = integer_orient3d(
                        integer_points[first], integer_points[second],
                        integer_points[third], integer_points[point]
                    );
                    positive = positive || side > 0;
                    negative = negative || side < 0;
                }
                if(!(positive && negative)){
                    expected.insert({first, second, third});
                }
            }
        }
    }

    std::vector<Point3> points;
    points.reserve(integer_points.size());
    for(const IntegerPoint3& point: integer_points){
        points.push_back({
            static_cast<long double>(point[0]),
            static_cast<long double>(point[1]),
            static_cast<long double>(point[2]),
        });
    }
    const ConvexPolyhedron3 hull = convex_hull_3d_with_seed(points, seed);
    if(hull.affine_dimension != 3) return false;
    std::set<std::array<std::size_t, 3>> actual;
    for(const auto& face: hull.faces){
        std::array<std::size_t, 3> source{};
        for(std::size_t corner = 0; corner < 3; ++corner){
            source[corner] = integer_points.size();
            const Point3& vertex = hull.vertices[face[corner]];
            for(std::size_t point = 0; point < integer_points.size(); ++point){
                if(vertex.x == integer_points[point][0]
                    && vertex.y == integer_points[point][1]
                    && vertex.z == integer_points[point][2]){
                    source[corner] = point;
                    break;
                }
            }
            if(source[corner] == integer_points.size()) return false;
        }
        std::sort(source.begin(), source.end());
        if(!actual.insert(source).second) return false;
    }
    return actual == expected;
}

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

            constexpr std::size_t spatial_size = 8000;
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
            // Facet-interior and edge-interior points must not survive as
            // vertices even if they belonged to an intermediate hull.
            {-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0},
            {0, 0, -1}, {0, 0, 1},
            {-1, -1, 0}, {-1, 1, 0}, {1, -1, 0}, {1, 1, 0},
            {-1, 0, -1}, {1, 0, 1}, {0, 0, 0}, {1, 1, 1},
        };
        const auto cube_hull = convex_hull_3d(cube);
        if(cube_hull.vertices.size() != 8 || cube_hull.faces.size() != 12
            || !validates_hull(cube_hull, cube)) return false;
        const auto seeded_cube = convex_hull_3d_with_seed(cube, 123456789);
        const auto repeated_cube = convex_hull_3d_with_seed(cube, 123456789);
        if(seeded_cube.vertices.size() != 8
            || seeded_cube.faces.size() != 12
            || seeded_cube.affine_dimension != repeated_cube.affine_dimension
            || seeded_cube.vertices != repeated_cube.vertices
            || seeded_cube.faces != repeated_cube.faces){
            return false;
        }
        std::vector<Point3> reversed_cube = cube;
        std::reverse(reversed_cube.begin(), reversed_cube.end());
        const auto reordered_cube = convex_hull_3d(reversed_cube);
        if(cube_hull.affine_dimension != reordered_cube.affine_dimension
            || cube_hull.vertices != reordered_cube.vertices
            || cube_hull.faces != reordered_cube.faces){
            return false;
        }

        for(std::size_t iteration = 0;
            iteration < std::min<std::size_t>(rounds, 96); ++iteration){
            std::vector<IntegerPoint3> integer_points;
            const std::size_t size = 7 + random() % 5;
            for(std::size_t attempt = 0;
                integer_points.size() < size && attempt < 100000; ++attempt){
                const IntegerPoint3 candidate{
                    static_cast<long long>(random() % 61) - 30,
                    static_cast<long long>(random() % 61) - 30,
                    static_cast<long long>(random() % 61) - 30,
                };
                if(can_append_general_position(integer_points, candidate)){
                    integer_points.push_back(candidate);
                }
            }
            if(integer_points.size() != size
                || !agrees_with_combination_oracle(integer_points, random())){
                return false;
            }
        }

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
