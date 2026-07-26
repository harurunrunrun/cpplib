// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/3d/delaunay_tetrahedralization_3d.hpp"
#include "../../src/algorithm/geometry/3d/adaptive_insphere.hpp"
#include "../../src/algorithm/geometry/3d/adaptive_orient3d.hpp"
#include "../../src/algorithm/geometry/3d/convex_hull_3d.hpp"
#include "../../src/algorithm/geometry/3d/convex_polyhedron_volume.hpp"
#include "../../src/algorithm/geometry/3d/signed_volume.hpp"
#include "geometry_3d_api_test_common.hpp"

namespace{

bool valid_delaunay(const DelaunayTetrahedralization3& result){
    for(const auto& tetrahedron: result.tetrahedra){
        for(std::size_t vertex: tetrahedron){
            if(vertex >= result.vertices.size()){
                std::cerr << "invalid vertex\n";
                return false;
            }
        }
        if(adaptive_orient3d(
            result.vertices[tetrahedron[0]], result.vertices[tetrahedron[1]],
            result.vertices[tetrahedron[2]], result.vertices[tetrahedron[3]]
        ) <= 0){
            std::cerr << "invalid orientation\n";
            return false;
        }
        for(std::size_t point = 0; point < result.vertices.size(); ++point){
            if(std::find(
                tetrahedron.begin(), tetrahedron.end(), point
            ) != tetrahedron.end()) continue;
            if(adaptive_insphere(
                result.vertices[tetrahedron[0]], result.vertices[tetrahedron[1]],
                result.vertices[tetrahedron[2]], result.vertices[tetrahedron[3]],
                result.vertices[point]
            ) > 0){
                std::cerr << "nonempty circumsphere\n";
                return false;
            }
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
        )){
            std::cerr << "volume mismatch\n";
            return false;
        }
    }
    return true;
}

bool valid_large_result(const DelaunayTetrahedralization3& result){
    if(result.affine_dimension != 3 || result.tetrahedra.empty()) return false;
    std::vector<unsigned char> used(result.vertices.size(), false);
    for(const auto& tetrahedron: result.tetrahedra){
        for(std::size_t vertex: tetrahedron){
            if(vertex >= result.vertices.size()) return false;
            used[vertex] = true;
        }
        if(adaptive_orient3d(
            result.vertices[tetrahedron[0]], result.vertices[tetrahedron[1]],
            result.vertices[tetrahedron[2]], result.vertices[tetrahedron[3]]
        ) <= 0) return false;
    }
    return std::find(used.begin(), used.end(), false) == used.end();
}

std::vector<Point3> performance_points(std::size_t size){
    std::mt19937_64 random(0xa4093822299f31d0ULL);
    std::uniform_real_distribution<long double> coordinate(-1.0L, 1.0L);
    std::vector<Point3> points;
    points.reserve(size);
    for(std::size_t index = 0; index < size; ++index){
        points.push_back({
            coordinate(random), coordinate(random),
            coordinate(random) + static_cast<long double>(index) * 1e-12L
        });
    }
    return points;
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
        const auto repeated = delaunay_tetrahedralization_3d({
            {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, 0, 0}
        });
        if(repeated.tetrahedra != tetrahedron.tetrahedra) return false;
        const auto seeded = delaunay_tetrahedralization_3d_randomized({
            {0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0.2L, 0.2L, 0.2L}
        }, random());
        if(!valid_delaunay(seeded)) return false;

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
        for(std::uint64_t seed = 0; seed < 8; ++seed){
            const auto randomized_cube =
                delaunay_tetrahedralization_3d_randomized({
                    {-1, -1, -1}, {-1, -1, 1},
                    {-1, 1, -1}, {-1, 1, 1},
                    {1, -1, -1}, {1, -1, 1},
                    {1, 1, -1}, {1, 1, 1},
                }, seed);
            if(!valid_delaunay(randomized_cube)) return false;
        }

        bool rejected_non_finite = false;
        try{
            static_cast<void>(delaunay_tetrahedralization_3d({
                {0, 0, 0},
                {1, 0, 0},
                {0, 1, 0},
                {0, 0, std::numeric_limits<long double>::infinity()},
            }));
        }catch(const std::invalid_argument&){
            rejected_non_finite = true;
        }
        if(!rejected_non_finite) return false;

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
        if(rounds >= 64){
            std::vector<Point3> oracle_points;
            oracle_points.reserve(64);
            for(std::size_t index = 0; index < 64; ++index){
                oracle_points.push_back({
                    coordinate(random), coordinate(random),
                    coordinate(random) + static_cast<long double>(index) * 1e-9L
                });
            }
            const auto oracle_result =
                delaunay_tetrahedralization_3d_randomized(
                    oracle_points, 0x082efa98ec4e6c89ULL
                );
            if(!valid_delaunay(oracle_result)) return false;
        }
        if(rounds >= 128){
            const auto large = delaunay_tetrahedralization_3d_randomized(
                performance_points(1600), 0x13198a2e03707344ULL
            );
            if(large.vertices.size() != 1600 || !valid_large_result(large)){
                return false;
            }
        }

        return true;
    });
}
