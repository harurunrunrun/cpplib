// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
#include <vector>

#include "../../src/algorithm/geometry/3d/adaptive_insphere.hpp"
#include "../../src/algorithm/geometry/3d/adaptive_orient3d.hpp"
#include "../../src/algorithm/geometry/3d/convex_hull_3d.hpp"
#include "../../src/algorithm/geometry/3d/convex_polyhedron_volume.hpp"
#include "../../src/algorithm/geometry/3d/signed_volume.hpp"
#include "../../src/algorithm/geometry/3d/delaunay_tetrahedralization_3d.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

bool valid_delaunay(const DelaunayTetrahedralization3& result){
    for(const auto& tetrahedron: result.tetrahedra){
        for(std::size_t vertex: tetrahedron){
            if(vertex >= result.vertices.size()) return false;
        }
        if(adaptive_orient3d(
            result.vertices[tetrahedron[0]], result.vertices[tetrahedron[1]],
            result.vertices[tetrahedron[2]], result.vertices[tetrahedron[3]]
        ) <= 0) return false;
        for(std::size_t point = 0; point < result.vertices.size(); ++point){
            if(std::find(
                tetrahedron.begin(), tetrahedron.end(), point
            ) != tetrahedron.end()) continue;
            if(adaptive_insphere(
                result.vertices[tetrahedron[0]], result.vertices[tetrahedron[1]],
                result.vertices[tetrahedron[2]], result.vertices[tetrahedron[3]],
                result.vertices[point]
            ) > 0) return false;
        }
    }
    if(result.affine_dimension == 3){
        long double tetrahedra_volume = 0.0L;
        for(const auto& tetrahedron: result.tetrahedra){
            tetrahedra_volume += std::abs(signed_volume({
                result.vertices[tetrahedron[0]], result.vertices[tetrahedron[1]],
                result.vertices[tetrahedron[2]], result.vertices[tetrahedron[3]]
            }));
        }
        const ConvexPolyhedron3 hull = convex_hull_3d(result.vertices);
        if(!geometry3d_api_close(
            tetrahedra_volume, convex_polyhedron_volume(hull), 2e-7L
        )) return false;
    }
    return true;
}

}  // namespace

int main(){
    return geometry3d_api_test_main([](auto& random, std::size_t rounds){
        const auto tetrahedron = delaunay_tetrahedralization_3d({
            {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, 0, 0}
        });
        if(tetrahedron.affine_dimension != 3 || tetrahedron.vertices.size() != 4
            || tetrahedron.tetrahedra.size() != 1 || !valid_delaunay(tetrahedron)){
            return false;
        }
        const auto coplanar = delaunay_tetrahedralization_3d({
            {0, 0, 2}, {1, 0, 2}, {0, 1, 2}, {1, 1, 2}
        });
        if(coplanar.affine_dimension != 2 || !coplanar.tetrahedra.empty()){
            return false;
        }
        const auto collinear = delaunay_tetrahedralization_3d({
            {0, 0, 0}, {1, 2, 3}, {2, 4, 6}
        });
        if(collinear.affine_dimension != 1) return false;
        const auto thin_tetrahedron = delaunay_tetrahedralization_3d({
            {0, 0, 0}, {1, 0, 0}, {0.5L, 1e-20L, 0},
            {0.5L, 0, 1e-20L},
        });
        if(thin_tetrahedron.affine_dimension != 3
            || thin_tetrahedron.tetrahedra.size() != 1
            || !valid_delaunay(thin_tetrahedron)) return false;
        const long double translation = 1e3000L;
        const long double upper = std::nextafter(
            translation, std::numeric_limits<long double>::infinity()
        );
        const auto translated_thin_tetrahedron =
            delaunay_tetrahedralization_3d({
                {0, 0, translation},
                {1, 0, translation},
                {0, 1, translation},
                {0, 0, upper},
            });
        if(translated_thin_tetrahedron.affine_dimension != 3
            || translated_thin_tetrahedron.tetrahedra.size() != 1){
            return false;
        }
        const auto& thin_indices =
            translated_thin_tetrahedron.tetrahedra.front();
        if(adaptive_orient3d(
            translated_thin_tetrahedron.vertices[thin_indices[0]],
            translated_thin_tetrahedron.vertices[thin_indices[1]],
            translated_thin_tetrahedron.vertices[thin_indices[2]],
            translated_thin_tetrahedron.vertices[thin_indices[3]]
        ) <= 0) return false;
        const auto cospherical_cube = delaunay_tetrahedralization_3d({
            {-1, -1, -1}, {-1, -1, 1}, {-1, 1, -1}, {-1, 1, 1},
            {1, -1, -1}, {1, -1, 1}, {1, 1, -1}, {1, 1, 1},
        });
        if(cospherical_cube.tetrahedra.empty()) return false;
        if(!valid_delaunay(cospherical_cube)) return false;

        std::uniform_real_distribution<long double> coordinate(-10.0L, 10.0L);
        const std::size_t iterations = std::min<std::size_t>(rounds, 30);
        for(std::size_t iteration = 0; iteration < iterations; ++iteration){
            std::vector<Point3> points;
            const std::size_t size = 5 + random() % 10;
            for(std::size_t index = 0; index < size; ++index){
                Point3 point{
                    coordinate(random), coordinate(random), coordinate(random)
                };
                point.z += static_cast<long double>(index) * 1e-7L;
                points.push_back(point);
            }
            const auto result = delaunay_tetrahedralization_3d(points);
            if(result.affine_dimension != 3 || result.tetrahedra.empty()
                || !valid_delaunay(result)) return false;
        }
        return true;
    });
}
