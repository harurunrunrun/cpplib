#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUATERNION3_TO_MATRIX3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUATERNION3_TO_MATRIX3_HPP_INCLUDED

#include "matrix3.hpp"
#include "quaternion3.hpp"

inline Matrix3 quaternion3_to_matrix3(const Quaternion3& quaternion){
    const Quaternion3 value = quaternion.normalized();
    const long double xx = value.x * value.x;
    const long double yy = value.y * value.y;
    const long double zz = value.z * value.z;
    const long double xy = value.x * value.y;
    const long double xz = value.x * value.z;
    const long double yz = value.y * value.z;
    const long double wx = value.w * value.x;
    const long double wy = value.w * value.y;
    const long double wz = value.w * value.z;
    return Matrix3(Matrix3::Storage{{
        Matrix3::Row{{1 - 2 * (yy + zz), 2 * (xy - wz), 2 * (xz + wy)}},
        Matrix3::Row{{2 * (xy + wz), 1 - 2 * (xx + zz), 2 * (yz - wx)}},
        Matrix3::Row{{2 * (xz - wy), 2 * (yz + wx), 1 - 2 * (xx + yy)}},
    }});
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUATERNION3_TO_MATRIX3_HPP_INCLUDED
