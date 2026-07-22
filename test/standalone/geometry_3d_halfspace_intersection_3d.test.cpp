// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
#include <string_view>
#include <variant>
#include <vector>

#include "../../src/algorithm/geometry/3d/predicate/convex_polyhedron_contains.hpp"
#include "../../src/algorithm/geometry/3d/scalar/convex_polyhedron_volume.hpp"
#include "../../src/algorithm/geometry/3d/shape/halfspace_intersection_3d.hpp"
#include "../../src/algorithm/geometry/3d/point/three_plane_intersection.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

std::vector<Plane3> box_halfspaces(
    long double minimum_x,
    long double maximum_x,
    long double minimum_y,
    long double maximum_y,
    long double minimum_z,
    long double maximum_z
){
    return {
        {{minimum_x, 0, 0}, {-1, 0, 0}},
        {{maximum_x, 0, 0}, {1, 0, 0}},
        {{0, minimum_y, 0}, {0, -1, 0}},
        {{0, maximum_y, 0}, {0, 1, 0}},
        {{0, 0, minimum_z}, {0, 0, -1}},
        {{0, 0, maximum_z}, {0, 0, 1}},
    };
}

bool independently_feasible(
    const std::vector<Plane3>& halfspaces,
    const Point3& point
){
    for(const Plane3& halfspace: halfspaces){
        const Point3 difference = point - halfspace.point;
        const long double value = dot(halfspace.normal, difference);
        const long double scale = std::max(
            1.0L,
            std::abs(halfspace.normal.x * difference.x)
                + std::abs(halfspace.normal.y * difference.y)
                + std::abs(halfspace.normal.z * difference.z)
        );
        if(value > 1e-9L * scale) return false;
    }
    return true;
}

std::vector<Point3> exhaustive_vertices(
    const std::vector<Plane3>& halfspaces
){
    std::vector<Point3> result;
    for(std::size_t first = 0; first < halfspaces.size(); ++first){
        for(std::size_t second = first + 1; second < halfspaces.size();
            ++second){
            for(std::size_t third = second + 1; third < halfspaces.size();
                ++third){
                const auto intersection = three_plane_intersection(
                    halfspaces[first], halfspaces[second], halfspaces[third]
                );
                const Point3* point = std::get_if<Point3>(&intersection);
                if(point && independently_feasible(halfspaces, *point)){
                    const bool duplicate = std::any_of(
                        result.begin(), result.end(), [&](const Point3& other){
                            return geometry3d_api_close(
                                *point, other, 1e-8L
                            );
                        }
                    );
                    if(!duplicate) result.push_back(*point);
                }
            }
        }
    }
    return result;
}

bool same_vertex_set(
    const std::vector<Point3>& first,
    const std::vector<Point3>& second
){
    if(first.size() != second.size()) return false;
    for(const Point3& point: first){
        if(std::none_of(second.begin(), second.end(), [&](const Point3& other){
            return geometry3d_api_close(point, other, 1e-7L);
        })) return false;
    }
    return true;
}

}  // namespace

int main(){
    return geometry3d_api_test_main([](auto& random, std::size_t rounds){
        const auto cube = halfspace_intersection_3d(box_halfspaces(
            -1, 1, -1, 1, -1, 1
        ));
        if(cube.affine_dimension != 3 || cube.vertices.size() != 8
            || !geometry3d_api_close(convex_polyhedron_volume(cube), 8.0L)
            || !convex_polyhedron_contains(cube, {0, 0, 0})
            || convex_polyhedron_contains(cube, {2, 0, 0})) return false;

        const auto fast_cube = halfspace_intersection_3d_with_interior_point(
            box_halfspaces(-1, 1, -1, 1, -1, 1), {0, 0, 0}
        );
        if(fast_cube.affine_dimension != 3
            || !same_vertex_set(fast_cube.vertices, cube.vertices)){
            return false;
        }
        bool boundary_interior_threw = false;
        try{
            static_cast<void>(
                halfspace_intersection_3d_with_interior_point(
                    box_halfspaces(-1, 1, -1, 1, -1, 1), {1, 0, 0}
                )
            );
        }catch(const std::invalid_argument&){
            boundary_interior_threw = true;
        }
        if(!boundary_interior_threw) return false;

        auto open_box = box_halfspaces(-1, 1, -1, 1, -1, 1);
        open_box.pop_back();
        bool unbounded_threw = false;
        try{
            static_cast<void>(halfspace_intersection_3d(open_box));
        }catch(const std::domain_error&){
            unbounded_threw = true;
        }
        if(!unbounded_threw) return false;
        bool fast_unbounded_threw = false;
        try{
            static_cast<void>(
                halfspace_intersection_3d_with_interior_point(
                    open_box, {0, 0, 0}
                )
            );
        }catch(const std::domain_error&){
            fast_unbounded_threw = true;
        }
        if(!fast_unbounded_threw) return false;
        bool unbounded_plane_threw = false;
        try{
            static_cast<void>(halfspace_intersection_3d({
                {{0, 0, 0}, {1, 1, 1}},
                {{0, 0, 0}, {-1, -1, -1}},
            }));
        }catch(const std::domain_error& error){
            unbounded_plane_threw = std::string_view(error.what()) ==
                "3D halfspace intersection is unbounded";
        }
        if(!unbounded_plane_threw) return false;
        const auto empty = halfspace_intersection_3d({
            {{0, 0, 0}, {1, 0, 0}}, {{1, 0, 0}, {-1, 0, 0}}
        });
        if(!empty.vertices.empty()) return false;
        const Point3 anisotropic_normal{1e3000L, 1e-3000L, 0};
        const auto anisotropic_empty = halfspace_intersection_3d({
            {{0, 0, 0}, anisotropic_normal},
            {{0, 1e3000L, 0}, -anisotropic_normal},
        });
        if(anisotropic_empty.affine_dimension != -1
            || !anisotropic_empty.vertices.empty()){
            return false;
        }
        const auto point = halfspace_intersection_3d({
            {{0, 0, 0}, {1, 0, 0}}, {{0, 0, 0}, {-1, 0, 0}},
            {{0, 0, 0}, {0, 1, 0}}, {{0, 0, 0}, {0, -1, 0}},
            {{0, 0, 0}, {0, 0, 1}}, {{0, 0, 0}, {0, 0, -1}},
        });
        if(point.affine_dimension != 0 || point.vertices.size() != 1
            || !geometry3d_api_close(point.vertices.front(), Point3{})){
            return false;
        }
        const auto square = halfspace_intersection_3d({
            {{-1, 0, 0}, {-1, 0, 0}}, {{1, 0, 0}, {1, 0, 0}},
            {{0, -1, 0}, {0, -1, 0}}, {{0, 1, 0}, {0, 1, 0}},
            {{0, 0, 0}, {0, 0, -1}}, {{0, 0, 0}, {0, 0, 1}},
        });
        if(square.affine_dimension != 2 || square.vertices.size() != 4){
            return false;
        }
        const auto segment = halfspace_intersection_3d({
            {{-1, 0, 0}, {-1, 0, 0}}, {{1, 0, 0}, {1, 0, 0}},
            {{0, 0, 0}, {0, -1, 0}}, {{0, 0, 0}, {0, 1, 0}},
            {{0, 0, 0}, {0, 0, -1}}, {{0, 0, 0}, {0, 0, 1}},
        });
        if(segment.affine_dimension != 1 || segment.vertices.size() != 2
            || std::none_of(
                segment.vertices.begin(), segment.vertices.end(),
                [](const Point3& vertex){
                    return geometry3d_api_close(vertex, {-1, 0, 0});
                }
            )){
            return false;
        }
        bool non_finite_threw = false;
        try{
            static_cast<void>(halfspace_intersection_3d_with_interior_point(
                box_halfspaces(-1, 1, -1, 1, -1, 1),
                {std::numeric_limits<long double>::infinity(), 0, 0}
            ));
        }catch(const std::invalid_argument&){
            non_finite_threw = true;
        }
        if(!non_finite_threw) return false;

        std::uniform_int_distribution<int> coordinate(-20, 20);
        const std::size_t iterations = std::min<std::size_t>(rounds, 40);
        for(std::size_t iteration = 0; iteration < iterations; ++iteration){
            const long double x0 = coordinate(random);
            const long double y0 = coordinate(random);
            const long double z0 = coordinate(random);
            const long double dx = 1 + random() % 10;
            const long double dy = 1 + random() % 10;
            const long double dz = 1 + random() % 10;
            auto halfspaces = box_halfspaces(x0, x0 + dx, y0, y0 + dy, z0, z0 + dz);
            halfspaces.push_back(halfspaces.front());
            std::shuffle(halfspaces.begin(), halfspaces.end(), random);
            const auto box = halfspace_intersection_3d(halfspaces);
            if(box.affine_dimension != 3 || box.vertices.size() != 8
                || !geometry3d_api_close(
                    convex_polyhedron_volume(box), dx * dy * dz, 1e-8L
                )) return false;
        }

        std::uniform_int_distribution<int> coefficient(-4, 4);
        const std::size_t oracle_iterations =
            std::min<std::size_t>(rounds, 24);
        for(std::size_t iteration = 0; iteration < oracle_iterations;
            ++iteration){
            std::vector<Plane3> halfspaces =
                box_halfspaces(-7, 7, -7, 7, -7, 7);
            const std::size_t added = 1 + random() % 5;
            for(std::size_t count = 0; count < added; ++count){
                Point3 normal;
                do{
                    normal = {
                        static_cast<long double>(coefficient(random)),
                        static_cast<long double>(coefficient(random)),
                        static_cast<long double>(coefficient(random)),
                    };
                }while(normal.x == 0 && normal.y == 0 && normal.z == 0);
                const long double offset = 1 + random() % 12;
                const long double normal_square = dot(normal, normal);
                halfspaces.push_back({
                    normal * (offset / normal_square), normal
                });
            }
            std::shuffle(halfspaces.begin(), halfspaces.end(), random);
            const std::vector<Point3> expected =
                exhaustive_vertices(halfspaces);
            const auto automatic = halfspace_intersection_3d(halfspaces);
            const auto supplied =
                halfspace_intersection_3d_with_interior_point(
                    halfspaces, {0, 0, 0}
                );
            if(automatic.affine_dimension != 3
                || supplied.affine_dimension != 3
                || !same_vertex_set(automatic.vertices, expected)
                || !same_vertex_set(supplied.vertices, expected)){
                return false;
            }
        }

        const std::size_t side_count = rounds >= 128 ? 2000 : 400;
        std::vector<Plane3> prism;
        prism.reserve(side_count + 2);
        const long double pi = std::acos(-1.0L);
        for(std::size_t index = 0; index < side_count; ++index){
            const long double angle =
                2.0L * pi * static_cast<long double>(index)
                / static_cast<long double>(side_count);
            const Point3 normal{std::cos(angle), std::sin(angle), 0};
            prism.push_back({normal, normal});
        }
        prism.push_back({{0, 0, 1}, {0, 0, 1}});
        prism.push_back({{0, 0, -1}, {0, 0, -1}});
        const auto large =
            halfspace_intersection_3d_with_interior_point(
                prism, {0, 0, 0}
            );
        if(large.affine_dimension != 3
            || large.vertices.size() != 2 * side_count
            || !geometry3d_api_close(
                convex_polyhedron_volume(large),
                2.0L * static_cast<long double>(side_count)
                    * std::tan(pi / static_cast<long double>(side_count)),
                1e-6L
            )){
            return false;
        }
        return true;
    });
}
