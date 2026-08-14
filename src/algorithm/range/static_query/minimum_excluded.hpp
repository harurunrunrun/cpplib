#ifndef CPPLIB_SRC_ALGORITHM_RANGE_STATIC_QUERY_MINIMUM_EXCLUDED_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_STATIC_QUERY_MINIMUM_EXCLUDED_HPP_INCLUDED

#include <concepts>
#include <cstddef>
#include <type_traits>
#include <vector>

template<std::integral Integer>
std::size_t minimum_excluded(const std::vector<Integer>& values){
    std::vector<bool> present(values.size() + 1, false);
    for(const Integer value: values){
        if constexpr(std::signed_integral<Integer>){
            if(value < 0) continue;
        }
        using Unsigned = std::make_unsigned_t<Integer>;
        const auto converted = static_cast<Unsigned>(value);
        if(converted <= values.size()){
            present[static_cast<std::size_t>(converted)] = true;
        }
    }
    for(std::size_t value = 0; value < present.size(); value++){
        if(!present[value]) return value;
    }
    return present.size();
}

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_STATIC_QUERY_MINIMUM_EXCLUDED_HPP_INCLUDED
