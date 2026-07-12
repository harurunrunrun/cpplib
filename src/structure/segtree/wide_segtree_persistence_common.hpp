#pragma once

#include "../types/monoid.hpp"

namespace wide_segtree_persistence_internal{

constexpr long long sum_op(long long left, long long right){ return left + right; }
constexpr long long sum_identity(){ return 0; }
inline constexpr Monoid<sum_op, sum_identity> sum_monoid;

} // namespace wide_segtree_persistence_internal
