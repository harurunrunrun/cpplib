// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/point_collection/great_circle_intersections.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        const auto intersections = great_circle_intersections({0, 0, 1}, {1, 0, 0});
        if(!intersections || !geometry3d_api_close(intersections->first, {0, 1, 0})
            || !geometry3d_api_close(intersections->second, {0, -1, 0})
            || great_circle_intersections({0, 0, 1}, {0, 0, -3})) return false;
        bool zero_rejected = false;
        try{ (void)great_circle_intersections({}, {1, 0, 0}); }
        catch(const std::invalid_argument&){ zero_rejected = true; }
        if(!zero_rejected) return false;
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 first{1, static_cast<long double>(random() % 100 + 1), 0};
            const Point3 second{0, 1, static_cast<long double>(random() % 100 + 1)};
            const auto result = great_circle_intersections(first, second);
            if(!result) return false;
            const auto dot_local = [](const Point3& a, const Point3& b){
                return a.x * b.x + a.y * b.y + a.z * b.z;
            };
            if(!geometry3d_api_close(dot_local(result->first, first), 0.0L)
                || !geometry3d_api_close(dot_local(result->first, second), 0.0L)
                || !geometry3d_api_close(result->second, -result->first)) return false;
        }
        return true;
    });
}
