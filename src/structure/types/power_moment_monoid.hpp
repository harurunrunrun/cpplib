#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "monoid.hpp"

template<class T, std::size_t MAX_POWER>
struct PowerMomentAggregate{
    std::size_t length = 0;
    std::array<T, MAX_POWER + 1> moment{};

    friend constexpr bool operator==(
        const PowerMomentAggregate&,
        const PowerMomentAggregate&
    ) = default;
};

template<class T, std::size_t MAX_POWER>
constexpr PowerMomentAggregate<T, MAX_POWER>
power_moment_singleton(const T& value){
    PowerMomentAggregate<T, MAX_POWER> result;
    result.length = 1;
    result.moment.fill(value);
    return result;
}

namespace power_moment_detail{

template<std::size_t MAX_POWER>
inline constexpr auto binomial = []{
    static_assert(
        MAX_POWER <= 67,
        "PowerMomentMonoid supports powers up to 67."
    );
    std::array<std::array<std::uint64_t, MAX_POWER + 1>, MAX_POWER + 1>
        result{};
    result[0][0] = 1;
    for(std::size_t n = 1; n <= MAX_POWER; ++n){
        result[n][0] = 1;
        result[n][n] = 1;
        for(std::size_t k = 1; k < n; ++k){
            result[n][k] = result[n - 1][k - 1]
                + result[n - 1][k];
        }
    }
    return result;
}();

template<class T, std::size_t MAX_POWER>
constexpr PowerMomentAggregate<T, MAX_POWER> power_moment_op(
    PowerMomentAggregate<T, MAX_POWER> left,
    PowerMomentAggregate<T, MAX_POWER> right
){
    PowerMomentAggregate<T, MAX_POWER> result;
    result.length = left.length + right.length;

    std::array<T, MAX_POWER + 1> shift_power{};
    shift_power[0] = T(1);
    const T shift = static_cast<T>(left.length);
    for(std::size_t power = 1; power <= MAX_POWER; ++power){
        shift_power[power] = shift_power[power - 1] * shift;
    }

    for(std::size_t power = 0; power <= MAX_POWER; ++power){
        result.moment[power] = left.moment[power];
        for(std::size_t lower_power = 0;
            lower_power <= power;
            ++lower_power){
            result.moment[power] = result.moment[power]
                + static_cast<T>(binomial<MAX_POWER>[power][lower_power])
                    * shift_power[power - lower_power]
                    * right.moment[lower_power];
        }
    }
    return result;
}

template<class T, std::size_t MAX_POWER>
constexpr PowerMomentAggregate<T, MAX_POWER> power_moment_identity(){
    return {};
}

} // namespace power_moment_detail

template<class T, std::size_t MAX_POWER>
using PowerMomentMonoid = Monoid<
    power_moment_detail::power_moment_op<T, MAX_POWER>,
    power_moment_detail::power_moment_identity<T, MAX_POWER>
>;
