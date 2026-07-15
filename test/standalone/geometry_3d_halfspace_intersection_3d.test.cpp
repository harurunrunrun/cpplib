// competitive-verifier: STANDALONE

#include <algorithm>
#include <cstddef>
#include <random>
#include <string_view>
#include <vector>

#include "../../src/algorithm/geometry/3d/convex_polyhedron_contains.hpp"
#include "../../src/algorithm/geometry/3d/convex_polyhedron_volume.hpp"
#include "../../src/algorithm/geometry/3d/halfspace_intersection_3d.hpp"
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

        auto open_box = box_halfspaces(-1, 1, -1, 1, -1, 1);
        open_box.pop_back();
        bool unbounded_threw = false;
        try{
            static_cast<void>(halfspace_intersection_3d(open_box));
        }catch(const std::domain_error&){
            unbounded_threw = true;
        }
        if(!unbounded_threw) return false;
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
        return true;
    });
}
