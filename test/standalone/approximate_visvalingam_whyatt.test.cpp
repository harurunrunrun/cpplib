// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/geometry/visvalingam_whyatt.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t n;
    std::size_t target;
    if(!(std::cin >> n >> target)) return 0;
    std::vector<std::array<long double, 2>> points(n);
    for(auto& point: points) std::cin >> point[0] >> point[1];
    const auto indices = approximate::geometry::visvalingam_whyatt_indices(points, target);
    const auto simplified = approximate::geometry::visvalingam_whyatt(points, target);
    assert(indices.size() == target);
    assert(indices.size() == simplified.size());
    assert(indices.front() == 0);
    assert(indices.back() + 1 == n);
    for(std::size_t index = 0; index < indices.size(); ++index){
        assert(simplified[index] == points[indices[index]]);
    }
    bool invalid = false;
    try{
        static_cast<void>(approximate::geometry::visvalingam_whyatt_indices(points, 1));
    }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{
        static_cast<void>(approximate::geometry::visvalingam_whyatt_indices(
            std::vector<std::array<double, 2>>{{0.0, 0.0}, {1.0, 1.0},
                {2.0, std::numeric_limits<double>::quiet_NaN()}}, 2
        ));
    }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    std::cout << indices.size();
    for(std::size_t index: indices) std::cout << ' ' << index;
    std::cout << '\n';
}
