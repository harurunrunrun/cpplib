#ifndef CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_OPTIMIZATION_RESULT_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_OPTIMIZATION_RESULT_HPP_INCLUDED

#include <cstddef>
#include <vector>

namespace approximate::optimization {

template<class Real>
struct OptimizationResult{
    std::vector<Real> point;
    Real value{};
    std::size_t iterations = 0;
    std::size_t evaluations = 0;
    std::size_t gradient_evaluations = 0;
    bool converged = false;
};

}  // namespace approximate::optimization

#endif  // CPPLIB_SRC_APPROXIMATE_OPTIMIZATION_OPTIMIZATION_RESULT_HPP_INCLUDED
