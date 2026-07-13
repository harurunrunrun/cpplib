#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>

namespace approximate::evolutionary::detail {

inline std::size_t checked_add(
    std::size_t lhs,
    std::size_t rhs,
    const char* message
){
    if(rhs > std::numeric_limits<std::size_t>::max() - lhs)[[unlikely]]{
        throw std::length_error(message);
    }
    return lhs + rhs;
}

inline std::size_t checked_multiply(
    std::size_t lhs,
    std::size_t rhs,
    const char* message
){
    if(lhs != 0 && rhs > std::numeric_limits<std::size_t>::max() / lhs)[[unlikely]]{
        throw std::length_error(message);
    }
    return lhs * rhs;
}

inline std::size_t checked_multiply_add(
    std::size_t lhs,
    std::size_t rhs,
    std::size_t addend,
    const char* message
){
    return checked_add(checked_multiply(lhs, rhs, message), addend, message);
}

}  // namespace approximate::evolutionary::detail
