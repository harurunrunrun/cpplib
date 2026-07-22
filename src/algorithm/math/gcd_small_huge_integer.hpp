#ifndef CPPLIB_SRC_ALGORITHM_MATH_GCD_SMALL_HUGE_INTEGER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_GCD_SMALL_HUGE_INTEGER_HPP_INCLUDED

#include <cctype>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <string>

namespace math{

inline std::string gcd_small_huge_integer(
    std::uint32_t small,
    const std::string& huge
){
    if(huge.empty())[[unlikely]]{
        throw std::invalid_argument("gcd_small_huge_integer requires a decimal integer");
    }
    std::size_t first_nonzero = huge.size();
    std::uint32_t remainder = 0;
    for(std::size_t i = 0; i < huge.size(); ++i){
        const unsigned char ch = static_cast<unsigned char>(huge[i]);
        if(!std::isdigit(ch))[[unlikely]]{
            throw std::invalid_argument("invalid decimal integer");
        }
        if(ch != '0' && first_nonzero == huge.size()) first_nonzero = i;
        if(small != 0){
            remainder = static_cast<std::uint32_t>(
                (static_cast<std::uint64_t>(remainder) * 10 + (ch - '0')) % small
            );
        }
    }
    if(small == 0){
        return first_nonzero == huge.size() ? "0" : huge.substr(first_nonzero);
    }
    return std::to_string(std::gcd(small, remainder));
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_GCD_SMALL_HUGE_INTEGER_HPP_INCLUDED
