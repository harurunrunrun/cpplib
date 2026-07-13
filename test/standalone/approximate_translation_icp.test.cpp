// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/geometry/iterative_closest_point.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t source_size;
    std::size_t target_size;
    std::size_t max_iterations;
    long double tolerance;
    std::array<long double, 3> expected{};
    if(!(std::cin >> source_size >> target_size >> max_iterations >> tolerance
         >> expected[0] >> expected[1] >> expected[2])) return 0;
    std::vector<std::array<long double, 3>> source(source_size);
    std::vector<std::array<long double, 3>> target(target_size);
    for(auto& point: source) std::cin >> point[0] >> point[1] >> point[2];
    for(auto& point: target) std::cin >> point[0] >> point[1] >> point[2];
    const auto result = approximate::geometry::brute_force_translation_icp<3>(
        source, target, max_iterations, tolerance
    );
    assert(result.converged);
    assert(result.transformed_points.size() == source.size());
    assert(result.correspondences.size() == source.size());
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        assert(std::fabs(result.translation[coordinate] - expected[coordinate]) < 1.0e-15L);
    }
    assert(result.mean_squared_error < 1.0e-24L);
    const auto crossed_boundary = approximate::geometry::brute_force_translation_icp<1>(
        std::vector<std::array<long double, 1>>{{4.9L}, {-100.0L}, {-100.0L}},
        std::vector<std::array<long double, 1>>{{0.0L}, {10.0L}}, 1, 0.0L
    );
    assert(crossed_boundary.correspondences[0] == 1);
    for(std::size_t index = 0; index < crossed_boundary.transformed_points.size(); ++index){
        const long double value = crossed_boundary.transformed_points[index][0];
        const long double left = std::fabs(value);
        const long double right = std::fabs(value - 10.0L);
        assert(crossed_boundary.correspondences[index] == (right < left ? 1U : 0U));
    }
    bool invalid = false;
    try{
        static_cast<void>(approximate::geometry::brute_force_translation_icp<3>(
            std::vector<std::array<double, 3>>{{0.0, 0.0, 0.0}},
            std::vector<std::array<double, 3>>{}, 10
        ));
    }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{
        static_cast<void>(approximate::geometry::brute_force_translation_icp<1>(
            std::vector<std::array<long double, 1>>{{std::numeric_limits<long double>::max()}},
            std::vector<std::array<long double, 1>>{{0.0L}}, 2, 0.0L,
            std::array<long double, 1>{std::numeric_limits<long double>::max()}
        ));
    }catch(const std::overflow_error&){ invalid = true; }
    assert(invalid);
    std::cout << std::llround(result.translation[0]) << ' '
              << std::llround(result.translation[1]) << ' '
              << std::llround(result.translation[2]) << " 0\n";
}
