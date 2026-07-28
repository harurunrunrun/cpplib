#ifndef CPPLIB_SRC_ALGORITHM_STRING_COMPRESSION_LZ78_FACTORIZATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_COMPRESSION_LZ78_FACTORIZATION_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <string>
#include <vector>

struct LZ78Factor{
    int prefix = 0;
    unsigned char next_symbol = 0;
    bool has_next_symbol = false;
};

inline std::vector<LZ78Factor> lz78_factorization(const std::string& text){
    std::vector<std::array<int, 256>> transition(1);
    transition.front().fill(-1);
    std::vector<LZ78Factor> result;
    std::size_t position = 0;
    while(position < text.size()){
        int node = 0;
        while(position < text.size()){
            const unsigned char symbol =
                static_cast<unsigned char>(text[position]);
            const int next =
                transition[static_cast<std::size_t>(node)]
                    [static_cast<std::size_t>(symbol)];
            if(next == -1){
                result.push_back({node, symbol, true});
                transition[static_cast<std::size_t>(node)]
                    [static_cast<std::size_t>(symbol)] =
                    static_cast<int>(transition.size());
                transition.emplace_back();
                transition.back().fill(-1);
                ++position;
                node = -1;
                break;
            }
            node = next;
            ++position;
        }
        if(node != -1) result.push_back({node, 0, false});
    }
    return result;
}

inline std::string decode_lz78(const std::vector<LZ78Factor>& factors){
    std::vector<std::string> dictionary(1);
    std::string result;
    for(const LZ78Factor& factor: factors){
        std::string phrase =
            dictionary[static_cast<std::size_t>(factor.prefix)];
        if(factor.has_next_symbol){
            phrase.push_back(static_cast<char>(factor.next_symbol));
            dictionary.push_back(phrase);
        }
        result += phrase;
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_COMPRESSION_LZ78_FACTORIZATION_HPP_INCLUDED
