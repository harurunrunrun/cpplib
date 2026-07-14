#pragma once

#include <array>
#include <stdexcept>
#include <string>
#include <vector>

inline bool words_can_form_chain(const std::vector<std::string>& words){
    if(words.empty()) return false;
    std::array<int, 26> in_degree{};
    std::array<int, 26> out_degree{};
    std::array<std::array<unsigned char, 26>, 26> adjacent{};
    std::array<unsigned char, 26> used{};
    for(const auto& word: words){
        if(word.empty())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty word (words_can_form_chain)."
            );
        }
        for(char character: word){
            if(character < 'a' || 'z' < character)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: invalid character "
                    "(words_can_form_chain)."
                );
            }
        }
        const int first = word.front() - 'a';
        const int last = word.back() - 'a';
        out_degree[static_cast<std::size_t>(first)]++;
        in_degree[static_cast<std::size_t>(last)]++;
        used[static_cast<std::size_t>(first)] = 1;
        used[static_cast<std::size_t>(last)] = 1;
        adjacent[static_cast<std::size_t>(first)][static_cast<std::size_t>(last)] = 1;
        adjacent[static_cast<std::size_t>(last)][static_cast<std::size_t>(first)] = 1;
    }

    int start_count = 0;
    int end_count = 0;
    for(int letter = 0; letter < 26; letter++){
        const int difference =
            out_degree[static_cast<std::size_t>(letter)] -
            in_degree[static_cast<std::size_t>(letter)];
        if(difference == 1){
            start_count++;
        }else if(difference == -1){
            end_count++;
        }else if(difference != 0){
            return false;
        }
    }
    if(!((start_count == 0 && end_count == 0) ||
         (start_count == 1 && end_count == 1))) return false;

    int start = 0;
    while(start < 26 && !used[static_cast<std::size_t>(start)]) start++;
    std::array<unsigned char, 26> seen{};
    std::vector<int> stack = {start};
    seen[static_cast<std::size_t>(start)] = 1;
    while(!stack.empty()){
        const int letter = stack.back();
        stack.pop_back();
        for(int next = 0; next < 26; next++){
            if(!adjacent[static_cast<std::size_t>(letter)][static_cast<std::size_t>(next)] ||
               seen[static_cast<std::size_t>(next)]) continue;
            seen[static_cast<std::size_t>(next)] = 1;
            stack.push_back(next);
        }
    }
    for(int letter = 0; letter < 26; letter++){
        if(used[static_cast<std::size_t>(letter)] &&
           !seen[static_cast<std::size_t>(letter)]) return false;
    }
    return true;
}
