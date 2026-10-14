#ifndef CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_CYCLIC_CORRELATION_64_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_CYCLIC_CORRELATION_64_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "convolution_mod_2_64.hpp"
#include "convolution_gf2_64.hpp"

inline std::vector<std::uint64_t> cyclic_correlation_mod_2_64(
    const std::vector<std::uint64_t>& first,const std::vector<std::uint64_t>& second
){
    if(first.size()!=second.size()) throw std::invalid_argument("cyclic correlation size mismatch");
    if(first.empty())return {};
    auto reversed=first;
    std::reverse(reversed.begin()+1,reversed.end());
    const auto product=convolution_mod_2_64(reversed,second);
    std::vector<std::uint64_t> result(first.size());
    for(std::size_t i=0;i<product.size();++i)result[i%result.size()]+=product[i];
    return result;
}

inline std::vector<std::uint64_t> cyclic_correlation_gf2_64(
    const std::vector<std::uint64_t>& first,const std::vector<std::uint64_t>& second
){
    if(first.size()!=second.size()) throw std::invalid_argument("cyclic correlation size mismatch");
    if(first.empty())return {};
    auto reversed=first;
    std::reverse(reversed.begin()+1,reversed.end());
    const auto product=gf2_64_convolution(reversed,second);
    std::vector<std::uint64_t> result(first.size());
    for(std::size_t i=0;i<product.size();++i)result[i%result.size()]^=product[i];
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_TRANSFORM_CYCLIC_CORRELATION_64_HPP_INCLUDED
