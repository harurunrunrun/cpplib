#pragma once

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

inline bool phone_list_is_consistent(const std::vector<std::string>& phone_numbers){
    std::vector<std::string_view> ordered;
    ordered.reserve(phone_numbers.size());
    for(const auto& phone_number: phone_numbers) ordered.emplace_back(phone_number);
    std::sort(ordered.begin(), ordered.end());

    for(std::size_t index = 1; index < ordered.size(); ++index){
        if(ordered[index].starts_with(ordered[index - 1])) return false;
    }
    return true;
}
