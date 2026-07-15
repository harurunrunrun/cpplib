// competitive-verifier: STANDALONE

#include <algorithm>
#include <vector>

#include "../../src/algorithm/geometry/3d/divide_and_conquer_convex_hull_3d.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<Point3> points;
            const std::size_t count = 40 + random() % 30;
            for(std::size_t index = 0; index < count; ++index){
                points.push_back({
                    static_cast<long double>(static_cast<int>(random() % 41) - 20),
                    static_cast<long double>(static_cast<int>(random() % 41) - 20),
                    static_cast<long double>(static_cast<int>(random() % 41) - 20),
                });
            }
            auto actual = divide_and_conquer_convex_hull_3d(points).vertices;
            auto expected = convex_hull_3d(points).vertices;
            std::sort(actual.begin(), actual.end());
            std::sort(expected.begin(), expected.end());
            if(actual != expected) return false;
        }
        return true;
    });
}
