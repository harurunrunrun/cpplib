#pragma once

#include <cstdint>

#include "prime_factorization.hpp"

namespace math{

inline std::uint64_t euler_totient_value(std::uint64_t n){
    return euler_phi(n);
}

} // namespace math
