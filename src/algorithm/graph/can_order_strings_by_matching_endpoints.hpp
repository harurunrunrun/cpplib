#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

inline bool can_order_strings_by_matching_endpoints(
    const std::vector<std::string>& strings
){
    if(strings.empty()) return false;
    constexpr int alphabet_size = 256;
    std::array<int, alphabet_size> in_degree{};
    std::array<int, alphabet_size> out_degree{};
    std::array<std::vector<unsigned char>, alphabet_size> adjacent;
    std::array<unsigned char, alphabet_size> used{};
    for(const auto& string: strings){
        if(string.empty())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty string "
                "(can_order_strings_by_matching_endpoints)."
            );
        }
        const auto first =
            static_cast<unsigned char>(string.front());
        const auto last =
            static_cast<unsigned char>(string.back());
        ++out_degree[static_cast<std::size_t>(first)];
        ++in_degree[static_cast<std::size_t>(last)];
        used[static_cast<std::size_t>(first)] = 1;
        used[static_cast<std::size_t>(last)] = 1;
        adjacent[static_cast<std::size_t>(first)].push_back(last);
        adjacent[static_cast<std::size_t>(last)].push_back(first);
    }

    int start_count = 0;
    int end_count = 0;
    for(int symbol = 0; symbol < alphabet_size; ++symbol){
        const int difference =
            out_degree[static_cast<std::size_t>(symbol)] -
            in_degree[static_cast<std::size_t>(symbol)];
        if(difference == 1){
            ++start_count;
        }else if(difference == -1){
            ++end_count;
        }else if(difference != 0){
            return false;
        }
    }
    if(!((start_count == 0 && end_count == 0) ||
         (start_count == 1 && end_count == 1))){
        return false;
    }

    int start = 0;
    while(start < alphabet_size &&
          used[static_cast<std::size_t>(start)] == 0){
        ++start;
    }
    std::array<unsigned char, alphabet_size> seen{};
    std::vector<int> stack = {start};
    seen[static_cast<std::size_t>(start)] = 1;
    while(!stack.empty()){
        const int symbol = stack.back();
        stack.pop_back();
        for(const unsigned char next:
            adjacent[static_cast<std::size_t>(symbol)]){
            if(seen[static_cast<std::size_t>(next)] != 0) continue;
            seen[static_cast<std::size_t>(next)] = 1;
            stack.push_back(next);
        }
    }
    for(int symbol = 0; symbol < alphabet_size; ++symbol){
        if(used[static_cast<std::size_t>(symbol)] != 0 &&
           seen[static_cast<std::size_t>(symbol)] == 0){
            return false;
        }
    }
    return true;
}
