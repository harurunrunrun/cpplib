// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/shape/quaternion3_from_axis_angle.hpp"
#include "../../src/algorithm/geometry/3d/point/quaternion3_rotate.hpp"
#include "../../src/algorithm/geometry/3d/shape/quaternion3_to_matrix3.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64& random, std::size_t rounds){
        for(std::size_t iteration = 0; iteration < rounds; ++iteration){
            Point3 axis{
                static_cast<long double>(random() % 100 + 1),
                static_cast<long double>(random() % 101),
                static_cast<long double>(random() % 101),
            };
            const long double angle = static_cast<long double>(random() % 2001) / 1000.0L
                * GEOMETRY3D_PI - GEOMETRY3D_PI;
            const Quaternion3 quaternion = quaternion3_from_axis_angle(axis, angle);
            const Matrix3 matrix = quaternion3_to_matrix3(quaternion);
            const Point3 point{
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
                static_cast<long double>(static_cast<int>(random() % 201) - 100),
            };
            const Matrix3 product = matrix * matrix.transposed();
            if(!geometry3d_api_close(matrix * point, quaternion3_rotate(quaternion, point))
                || !geometry3d_api_close(matrix.determinant(), 1.0L)) return false;
            for(std::size_t row = 0; row < 3; ++row){
                for(std::size_t column = 0; column < 3; ++column){
                    if(!geometry3d_api_close(
                        product[row][column], row == column ? 1.0L : 0.0L
                    )) return false;
                }
            }
        }
        return true;
    });
}
