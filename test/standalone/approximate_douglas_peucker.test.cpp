// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/geometry/douglas_peucker.hpp"
#include "../../src/approximate/geometry/detail/polyline_common.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    const std::array<long double, 2> probe{1.0L, 1.0L};
    const std::array<long double, 2> first{0.0L, 0.0L};
    const std::array<long double, 2> second{2.0L, 0.0L};
    assert(approximate::geometry::detail::point_segment_squared_distance(
        probe, first, second
    ) == 1.0L);
    std::size_t n;
    long double tolerance;
    if(!(std::cin >> n >> tolerance)) return 0;
    std::vector<std::array<long double, 2>> points(n);
    for(auto& point: points) std::cin >> point[0] >> point[1];
    const auto indices = approximate::geometry::douglas_peucker_indices(points, tolerance);
    const auto simplified = approximate::geometry::douglas_peucker(points, tolerance);
    assert(indices.size() == simplified.size());
    for(std::size_t index = 0; index < indices.size(); ++index){
        assert(simplified[index] == points[indices[index]]);
        if(index != 0) assert(indices[index - 1] < indices[index]);
    }
    bool invalid = false;
    try{
        static_cast<void>(approximate::geometry::douglas_peucker_indices(
            points, std::numeric_limits<long double>::quiet_NaN()
        ));
    }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{
        static_cast<void>(approximate::geometry::douglas_peucker_indices(
            std::vector<std::array<double, 2>>{{0.0, std::numeric_limits<double>::infinity()}},
            1.0L
        ));
    }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    std::cout << indices.size();
    for(std::size_t index: indices) std::cout << ' ' << index;
    std::cout << '\n';
}
