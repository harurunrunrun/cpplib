#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_SYMMETRIC_EIGEN3_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_SYMMETRIC_EIGEN3_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <stdexcept>

#include "matrix3.hpp"

struct SymmetricEigen3{
    std::array<long double, 3> eigenvalues{};
    Matrix3 eigenvectors = Matrix3::identity();

    Point3 eigenvector(std::size_t index) const{
        if(index >= 3){
            throw std::out_of_range("eigenvector index must be below three");
        }
        return {
            eigenvectors[0][index],
            eigenvectors[1][index],
            eigenvectors[2][index],
        };
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_TYPE_DEFINITION_SYMMETRIC_EIGEN3_HPP_INCLUDED
