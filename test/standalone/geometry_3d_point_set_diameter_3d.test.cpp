// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <optional>
#include <random>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/3d/point_set_diameter_3d.hpp"

namespace{

PointSetDiameter3DResult brute(const std::vector<Point3>& points){
    PointSetDiameter3DResult result{0, 1, 0};
    for(std::size_t first = 0; first < points.size(); ++first){
        for(std::size_t second = first + 1; second < points.size(); ++second){
            const long double distance = std::hypot(
                points[first].x - points[second].x,
                points[first].y - points[second].y,
                points[first].z - points[second].z
            );
            if(distance > result.distance ||
               (distance == result.distance &&
                std::pair{first, second} < std::pair{result.first, result.second})){
                result = {first, second, distance};
            }
        }
    }
    return result;
}

} // namespace

int main(){
    std::uint64_t seed;
    std::size_t rounds;
    if(!(std::cin >> seed >> rounds) || rounds == 0 || rounds > 10000) return 2;
    if(point_set_diameter_3d({}) || point_set_diameter_3d({{1, 2, 3}})) return 1;

    const std::vector<Point3> points{{
        {0, 0, 0}, {2, 0, 0}, {-2, 0, 0}, {0, 2, 0}, {0, 0, 2},
    }};
    const auto result = point_set_diameter_3d(points);
    if(!result || result->first != 1 || result->second != 2 ||
       std::abs(result->distance - 4) > 1e-12L) return 1;

    std::mt19937_64 random(seed);
    std::uniform_int_distribution<int> coordinate(-100, 100);
    for(std::size_t iteration = 0; iteration < rounds; ++iteration){
        const std::size_t size = 2 + static_cast<std::size_t>(random() % 60);
        std::vector<Point3> current;
        current.reserve(size);
        const long double shift = iteration % 2 == 0 ? 1e14L : 0;
        for(std::size_t index = 0; index < size; ++index){
            current.push_back({
                shift + coordinate(random),
                -shift + coordinate(random),
                shift + coordinate(random),
            });
        }
        const std::optional<PointSetDiameter3DResult> actual =
            point_set_diameter_3d(current);
        const PointSetDiameter3DResult expected = brute(current);
        if(!actual || actual->first != expected.first ||
           actual->second != expected.second ||
           std::abs(actual->distance - expected.distance) > 1e-12L *
               std::max(1.0L, expected.distance)) return 1;
    }
    std::cout << "OK\n";
    return 0;
}
