#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MULTIPLICATIVE_DIRICHLET_CONVOLUTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MULTIPLICATIVE_DIRICHLET_CONVOLUTION_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace math {

template<class Value>
std::vector<Value> dirichlet_convolution(
    const std::vector<Value>& first,
    const std::vector<Value>& second
) {
    if(first.size() != second.size() || first.empty()){
        throw std::invalid_argument(
            "Dirichlet convolution tables must have equal positive size"
        );
    }
    const std::size_t maximum = first.size() - 1;
    std::vector<Value> result(maximum + 1);
    for(std::size_t divisor = 1; divisor <= maximum; ++divisor){
        for(std::size_t quotient = 1;
            quotient <= maximum / divisor;
            ++quotient){
            result[divisor * quotient] +=
                first[divisor] * second[quotient];
        }
    }
    return result;
}

template<class Value>
std::vector<Value> dirichlet_inverse(
    const std::vector<Value>& values
) {
    if(values.size() < 2){
        throw std::invalid_argument(
            "Dirichlet inverse needs values through index 1"
        );
    }
    const std::size_t maximum = values.size() - 1;
    std::vector<Value> result(maximum + 1);
    result[1] = Value{1} / values[1];
    for(std::size_t value = 2; value <= maximum; ++value){
        Value sum{};
        for(std::size_t divisor = 2;
            divisor <= value / divisor;
            ++divisor){
            if(value % divisor != 0) continue;
            const std::size_t quotient = value / divisor;
            sum += values[divisor] * result[quotient];
            if(divisor != quotient){
                sum += values[quotient] * result[divisor];
            }
        }
        sum += values[value] * result[1];
        result[value] = -sum / values[1];
    }
    return result;
}

}  // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MULTIPLICATIVE_DIRICHLET_CONVOLUTION_HPP_INCLUDED
