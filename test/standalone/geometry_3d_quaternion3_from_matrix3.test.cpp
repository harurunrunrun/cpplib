// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/quaternion3_from_axis_angle.hpp"
#include "../../src/algorithm/geometry/3d/quaternion3_from_matrix3.hpp"
#include "../../src/algorithm/geometry/3d/quaternion3_to_matrix3.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            const Point3 axis{
                static_cast<long double>(random() % 100 + 1),
                static_cast<long double>(random() % 101),
                static_cast<long double>(random() % 101),
            };
            const long double angle = static_cast<long double>(random() % 2001) / 1000.0L
                * GEOMETRY3D_PI - GEOMETRY3D_PI;
            const Matrix3 source = quaternion3_to_matrix3(
                quaternion3_from_axis_angle(axis, angle)
            );
            const Matrix3 restored = quaternion3_to_matrix3(
                quaternion3_from_matrix3(source)
            );
            for(std::size_t row = 0; row < 3; ++row){
                for(std::size_t column = 0; column < 3; ++column){
                    if(!geometry3d_api_close(source[row][column], restored[row][column])){
                        return false;
                    }
                }
            }
        }
        bool reflection_rejected = false;
        bool singular_rejected = false;
        bool tolerance_rejected = false;
        Matrix3 reflection = Matrix3::identity();
        reflection[0][0] = -1;
        try{ (void)quaternion3_from_matrix3(reflection); }
        catch(const std::invalid_argument&){ reflection_rejected = true; }
        try{ (void)quaternion3_from_matrix3(Matrix3{}); }
        catch(const std::invalid_argument&){ singular_rejected = true; }
        try{ (void)quaternion3_from_matrix3(Matrix3::identity(), 1.0L); }
        catch(const std::invalid_argument&){ tolerance_rejected = true; }
        return reflection_rejected && singular_rejected && tolerance_rejected;
    });
}
