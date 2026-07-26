#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_REAL_GAUSSIAN_ELIMINATION_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_REAL_GAUSSIAN_ELIMINATION_RESULT_HPP_INCLUDED

#include <cstddef>
#include <vector>

struct RealGaussianEliminationResult{
    std::vector<std::vector<long double>> reduced_row_echelon_form;
    std::vector<std::size_t> pivot_columns;

    std::size_t rank() const noexcept{ return pivot_columns.size(); }
};

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_REAL_GAUSSIAN_ELIMINATION_RESULT_HPP_INCLUDED
