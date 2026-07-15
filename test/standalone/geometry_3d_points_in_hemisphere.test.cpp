// competitive-verifier: STANDALONE

#include <vector>

#include "../../src/algorithm/geometry/3d/points_in_hemisphere.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        if(!points_in_hemisphere({})
            || !points_in_hemisphere({{1, 0, 0}, {-1, 0, 0}})
            || !points_in_hemisphere({{1, 0, 0}, {0, 1, 0}, {-1, 0, 0}, {0, -1, 0}})
            || points_in_hemisphere({
                {1, 1, 1}, {1, -1, -1}, {-1, 1, -1}, {-1, -1, 1}
            }) || points_in_hemisphere({
                {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}
            })) return false;
        bool zero_rejected = false;
        try{ (void)points_in_hemisphere({{0, 0, 0}}); }
        catch(const std::invalid_argument&){ zero_rejected = true; }
        if(!zero_rejected) return false;
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            std::vector<Point3> points;
            const std::size_t count = 3 + random() % 20;
            for(std::size_t index = 0; index < count; ++index){
                points.push_back({
                    static_cast<long double>(static_cast<int>(random() % 201) - 100),
                    static_cast<long double>(static_cast<int>(random() % 201) - 100),
                    static_cast<long double>(random() % 100 + 1),
                });
            }
            if(!points_in_hemisphere(points)) return false;
        }
        return true;
    });
}
