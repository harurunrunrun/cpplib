#ifndef CPPLIB_SRC_ALGORITHM_OTHER_UTILITY_ORDERED_BOUND_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_UTILITY_ORDERED_BOUND_HPP_INCLUDED

#include <algorithm>
#include <iterator>
#include <utility>

template<class Range, class Value>
constexpr auto ordered_lower_bound(Range& range, const Value& value)
    -> decltype(std::begin(range)){
    if constexpr(requires{ range.lower_bound(value); }){
        return range.lower_bound(value);
    }else{
        return std::lower_bound(std::begin(range), std::end(range), value);
    }
}

template<class Range, class Value>
constexpr auto ordered_upper_bound(Range& range, const Value& value)
    -> decltype(std::begin(range)){
    if constexpr(requires{ range.upper_bound(value); }){
        return range.upper_bound(value);
    }else{
        return std::upper_bound(std::begin(range), std::end(range), value);
    }
}

template<class Range, class Value>
constexpr auto ordered_last_less_equal(Range& range, const Value& value)
    -> decltype(std::begin(range)){
    auto iterator = ordered_upper_bound(range, value);
    if(iterator == std::begin(range)) return std::end(range);
    return std::prev(iterator);
}

template<class Range, class Value>
constexpr auto ordered_last_less(Range& range, const Value& value)
    -> decltype(std::begin(range)){
    auto iterator = ordered_lower_bound(range, value);
    if(iterator == std::begin(range)) return std::end(range);
    return std::prev(iterator);
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_UTILITY_ORDERED_BOUND_HPP_INCLUDED
