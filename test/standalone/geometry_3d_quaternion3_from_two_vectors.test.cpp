// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/shape/quaternion3_from_two_vectors.hpp"
#include "../../src/algorithm/geometry/3d/point/quaternion3_rotate.hpp"
#include "../../src/algorithm/geometry/3d/scalar/spherical_central_angle.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        if(!geometry3d_api_close(
            quaternion3_rotate(quaternion3_from_two_vectors({1, 0, 0}, {-1, 0, 0}), {1, 0, 0}),
            {-1, 0, 0}
        )) return false;
        bool zero_rejected = false;
        try{ (void)quaternion3_from_two_vectors({}, {1, 0, 0}); }
        catch(const std::invalid_argument&){ zero_rejected = true; }
        if(!zero_rejected) return false;
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            Point3 from{
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
            };
            Point3 to{
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
            };
            if(from.x == 0 && from.y == 0 && from.z == 0) from.x = 1;
            if(to.x == 0 && to.y == 0 && to.z == 0) to.y = 1;
            const Point3 rotated = quaternion3_rotate(
                quaternion3_from_two_vectors(from, to), from
            );
            if(!geometry3d_api_close(spherical_central_angle(rotated, to), 0.0L)){
                return false;
            }
        }
        return true;
    });
}
