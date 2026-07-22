#ifndef CPPLIB_SRC_ALGORITHM_STRING_IS_PREFIX_FREE_STRING_SET_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_IS_PREFIX_FREE_STRING_SET_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

inline bool is_prefix_free_string_set(const std::vector<std::string>& strings){
    std::vector<std::string_view> ordered;
    ordered.reserve(strings.size());
    for(const auto& string: strings) ordered.emplace_back(string);
    std::sort(ordered.begin(), ordered.end());

    for(std::size_t index = 1; index < ordered.size(); ++index){
        if(ordered[index].starts_with(ordered[index - 1])) return false;
    }
    return true;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_IS_PREFIX_FREE_STRING_SET_HPP_INCLUDED
