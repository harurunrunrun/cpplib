#ifndef CPPLIB_SRC_APPROXIMATE_SCHEDULING_SCHEDULING_COMMON_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SCHEDULING_SCHEDULING_COMMON_HPP_INCLUDED

#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace approximate::scheduling::scheduling_internal {

template<class Value>
[[nodiscard]] bool finite_value(const Value& value){
    if constexpr(std::is_floating_point_v<Value>){
        return std::isfinite(value);
    }
    return true;
}

template<class Value>
[[nodiscard]] bool nonnegative_finite(const Value& value){
    return !(value < Value{}) && finite_value(value);
}

template<class Value>
[[nodiscard]] bool positive_finite(const Value& value){
    return Value{} < value && finite_value(value);
}

template<class Value>
[[nodiscard]] long double as_long_double(const Value& value){
    const long double converted = static_cast<long double>(value);
    if(!std::isfinite(converted))[[unlikely]]{
        throw std::overflow_error("scheduling value does not fit long double");
    }
    return converted;
}

template<class Time>
[[nodiscard]] Time checked_time_add(const Time& first, const Time& second){
    if constexpr(std::is_integral_v<Time>){
        if(second > std::numeric_limits<Time>::max() - first)[[unlikely]]{
            throw std::overflow_error("scheduling time overflow");
        }
        return static_cast<Time>(first + second);
    }else if constexpr(std::is_floating_point_v<Time>){
        const Time sum = first + second;
        if(!std::isfinite(sum))[[unlikely]]{
            throw std::overflow_error("scheduling time overflow");
        }
        return sum;
    }else{
        return first + second;
    }
}

template<class Time>
void validate_nonnegative_time(const Time& value, const char* message){
    if(!nonnegative_finite(value))[[unlikely]]{
        throw std::invalid_argument(message);
    }
}

}  // namespace approximate::scheduling::scheduling_internal

#endif  // CPPLIB_SRC_APPROXIMATE_SCHEDULING_SCHEDULING_COMMON_HPP_INCLUDED
