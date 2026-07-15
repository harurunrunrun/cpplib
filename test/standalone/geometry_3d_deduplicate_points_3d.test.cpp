// competitive-verifier: STANDALONE

#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/3d/deduplicate_points_3d.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const std::vector<Point3> result = deduplicate_points_3d({
            {0, 0, 0}, {5.0e-11L, 0, 0}, {2.0e-10L, 0, 0},
            {1, 2, 3}, {1, 2, 3}
        });
        if(result.size() != 3) return false;
        if(!(result[0] == Point3{0, 0, 0})
            || !(result[1] == Point3{2.0e-10L, 0, 0})
            || !(result[2] == Point3{1, 2, 3})) return false;
        const auto relative = deduplicate_points_3d({
            {1.0e12L, 0, 0}, {1.0e12L + 50.0L, 0, 0}
        }, 1.0e-10L, 0.0L);
        if(relative.size() != 1) return false;
        try{
            static_cast<void>(deduplicate_points_3d({{
                0, std::numeric_limits<long double>::quiet_NaN(), 0
            }}));
            return false;
        }catch(const std::invalid_argument&){}
        return true;
    });
}
