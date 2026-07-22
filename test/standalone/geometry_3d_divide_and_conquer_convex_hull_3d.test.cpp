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

#include "../../src/algorithm/geometry/3d/predicate/convex_polyhedron_contains.hpp"
#include "../../src/algorithm/geometry/3d/query/convex_polyhedron_counts.hpp"
#include "../../src/algorithm/geometry/3d/shape/divide_and_conquer_convex_hull_3d.hpp"
#include "../../src/algorithm/geometry/3d/shape/quickhull_3d.hpp"
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

bool same_hull_region(
    const ConvexPolyhedron3& left,
    const ConvexPolyhedron3& right
){
    if(left.affine_dimension != right.affine_dimension) return false;
    if(left.affine_dimension < 3){
        if(left.vertices.size() != right.vertices.size()) return false;
        std::vector<Point3> left_vertices = left.vertices;
        std::vector<Point3> right_vertices = right.vertices;
        std::sort(left_vertices.begin(), left_vertices.end());
        std::sort(right_vertices.begin(), right_vertices.end());
        for(std::size_t index = 0; index < left_vertices.size(); ++index){
            if(!exactly_equal(left_vertices[index], right_vertices[index])){
                return false;
            }
        }
        return true;
    }
    for(const Point3& point: left.vertices){
        if(!convex_polyhedron_contains(right, point)) return false;
    }
    for(const Point3& point: right.vertices){
        if(!convex_polyhedron_contains(left, point)) return false;
    }
    return true;
}

bool exactly_same_hull(
    const ConvexPolyhedron3& left,
    const ConvexPolyhedron3& right
){
    if(left.affine_dimension != right.affine_dimension
        || left.faces != right.faces
        || left.vertices.size() != right.vertices.size()){
        return false;
    }
    for(std::size_t index = 0; index < left.vertices.size(); ++index){
        if(!exactly_equal(left.vertices[index], right.vertices[index])){
            return false;
        }
    }
    return true;
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


        std::vector<Point3> dense_box;
        for(int x = -2; x <= 2; ++x){
            for(int y = -2; y <= 2; ++y){
                for(int z = -2; z <= 2; ++z){
                    if(std::abs(x) == 2 || std::abs(y) == 2
                        || std::abs(z) == 2){
                        dense_box.push_back({
                            static_cast<long double>(x),
                            static_cast<long double>(y),
                            static_cast<long double>(z),
                        });
                    }
                }
            }
        }
        dense_box.insert(
            dense_box.end(), dense_box.begin(), dense_box.begin() + 17
        );
        const auto dense_box_hull =
            divide_and_conquer_convex_hull_3d(dense_box);
        const auto dense_box_counts =
            convex_polyhedron_counts(dense_box_hull);
        if(dense_box_counts.vertices != 8 || dense_box_counts.edges != 12
            || dense_box_counts.facet_count != 6
            || !valid_full_dimensional_hull(dense_box_hull, dense_box)){
            return false;
        }

        std::vector<Point3> dense_pyramid;
        for(int x = -8; x <= 8; ++x){
            for(int y = -8; y <= 8; ++y){
                dense_pyramid.push_back({
                    static_cast<long double>(x),
                    static_cast<long double>(y), 0.0L,
                });
            }
        }
        dense_pyramid.push_back({0, 0, 9});
        const auto pyramid_hull =
            divide_and_conquer_convex_hull_3d(dense_pyramid);
        if(pyramid_hull.vertices.size() != 5
            || !valid_full_dimensional_hull(
                pyramid_hull, dense_pyramid
            )){
            return false;
        }

        std::vector<Point3> same_x_layers;
        for(int x: {-5, 0, 5}){
            for(int y = -4; y <= 4; ++y){
                for(int z = -4; z <= 4; ++z){
                    same_x_layers.push_back({
                        static_cast<long double>(x),
                        static_cast<long double>(y),
                        static_cast<long double>(z),
                    });
                }
            }
        }
        const auto same_x_hull =
            divide_and_conquer_convex_hull_3d(same_x_layers);
        if(same_x_hull.vertices.size() != 8
            || !valid_full_dimensional_hull(
                same_x_hull, same_x_layers
            )){
            return false;
        }

        std::vector<Point3> moment_curve;
        for(int parameter = -20; parameter <= 20; ++parameter){
            const long double value = parameter;
            moment_curve.push_back({
                value, value * value, value * value * value,
            });
        }
        const auto moment_hull =
            divide_and_conquer_convex_hull_3d(moment_curve);
        if(moment_hull.vertices.size() != moment_curve.size()
            || !valid_full_dimensional_hull(moment_hull, moment_curve)){
            return false;
        }

        const long double tiny =
            std::nextafter(0.0L, 1.0L);
        std::vector<Point3> almost_planar{
            {-1, -1, 0}, {1, -1, 0}, {1, 1, 0}, {-1, 1, 0},
            {0, 0, tiny}, {0, 0, -tiny},
        };
        const auto almost_planar_hull =
            divide_and_conquer_convex_hull_3d(almost_planar);
        if(almost_planar_hull.vertices.size() != almost_planar.size()
            || !valid_full_dimensional_hull(
                almost_planar_hull, almost_planar
            )){
            return false;
        }

        std::vector<Point3> permuted_cube = cube;
        std::shuffle(permuted_cube.begin(), permuted_cube.end(), random);
        if(!exactly_same_hull(
            cube_hull,
            divide_and_conquer_convex_hull_3d(permuted_cube)
        )){
            return false;
        }

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

        const long double large = 1e2000L;
        const long double small = 1e-2000L;
        std::vector<Point3> mixed_scale{
            {large, 0, 0}, {-large, 0, 0}, {0, large, 0},
            {0, 0, small}, {0, 0, small / 2},
        };
        const auto mixed_scale_hull =
            divide_and_conquer_convex_hull_3d(mixed_scale);
        if(mixed_scale_hull.vertices.size() != 4
            || !valid_full_dimensional_hull(
                mixed_scale_hull, mixed_scale
            )){
            return false;
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
            if(!same_hull_region(actual, reference)) return false;
            if(actual.affine_dimension == 3
                && !valid_full_dimensional_hull(actual, points)) return false;
            std::shuffle(points.begin(), points.end(), random);
            if(!exactly_same_hull(
                actual, divide_and_conquer_convex_hull_3d(points)
            )){
                return false;
            }
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
