#ifndef CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_MAIN_LORENTZ_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_MAIN_LORENTZ_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct TandemRepeat{
    int begin;
    int half_length;

    friend bool operator==(const TandemRepeat&, const TandemRepeat&) = default;
};

namespace main_lorentz_internal {

inline std::vector<int> z_function(const std::vector<int>& sequence){
    const int size = static_cast<int>(sequence.size());
    std::vector<int> z(static_cast<std::size_t>(size));
    for(int index = 1, left = 0, right = 0; index < size; ++index){
        if(index <= right){
            z[static_cast<std::size_t>(index)] = std::min(
                right - index + 1,
                z[static_cast<std::size_t>(index - left)]
            );
        }
        while(index + z[static_cast<std::size_t>(index)] < size
              && sequence[static_cast<std::size_t>(
                     z[static_cast<std::size_t>(index)]
                 )]
                     == sequence[static_cast<std::size_t>(
                         index + z[static_cast<std::size_t>(index)]
                     )]){
            ++z[static_cast<std::size_t>(index)];
        }
        if(right < index + z[static_cast<std::size_t>(index)] - 1){
            left = index;
            right = index + z[static_cast<std::size_t>(index)] - 1;
        }
    }
    if(size != 0) z[0] = size;
    return z;
}

inline std::vector<int> encode(
    const std::string& text,
    int left,
    int right,
    bool reverse
){
    std::vector<int> result;
    result.reserve(static_cast<std::size_t>(right - left));
    if(reverse){
        for(int index = right; index-- > left;){
            result.push_back(
                static_cast<int>(static_cast<unsigned char>(
                    text[static_cast<std::size_t>(index)]
                )) + 1
            );
        }
    }else{
        for(int index = left; index < right; ++index){
            result.push_back(
                static_cast<int>(static_cast<unsigned char>(
                    text[static_cast<std::size_t>(index)]
                )) + 1
            );
        }
    }
    return result;
}

inline std::vector<int> concatenate(
    const std::vector<int>& first,
    const std::vector<int>& second
){
    std::vector<int> result;
    result.reserve(first.size() + second.size() + 1);
    result.insert(result.end(), first.begin(), first.end());
    result.push_back(0);
    result.insert(result.end(), second.begin(), second.end());
    return result;
}

inline int z_at(const std::vector<int>& z, int index){
    if(index < 0 || static_cast<int>(z.size()) <= index) return 0;
    return z[static_cast<std::size_t>(index)];
}

inline void append_crossing_repetitions(
    std::vector<TandemRepeat>& result,
    int shift,
    bool center_is_left,
    int center,
    int half_length,
    int left_extension,
    int right_extension
){
    const int first_left_part =
        std::max(1, half_length - right_extension);
    const int last_left_part =
        std::min(half_length, left_extension);
    for(int left_part = first_left_part;
        left_part <= last_left_part;
        ++left_part){
        if(center_is_left && left_part == half_length) break;
        const int begin = shift
            + (center_is_left
                ? center - left_part
                : center - half_length - left_part + 1);
        result.push_back({begin, half_length});
    }
}

inline void enumerate(
    const std::string& text,
    int left,
    int right,
    std::vector<TandemRepeat>& result
){
    const int size = right - left;
    if(size <= 1) return;
    const int left_size = size / 2;
    const int right_size = size - left_size;
    const int middle = left + left_size;

    const std::vector<int> first = encode(text, left, middle, false);
    const std::vector<int> second = encode(text, middle, right, false);
    const std::vector<int> reversed_first =
        encode(text, left, middle, true);
    const std::vector<int> reversed_second =
        encode(text, middle, right, true);

    const std::vector<int> z_reversed_first = z_function(reversed_first);
    const std::vector<int> z_second_first =
        z_function(concatenate(second, first));
    const std::vector<int> z_first_second_reversed =
        z_function(concatenate(reversed_first, reversed_second));
    const std::vector<int> z_second = z_function(second);

    for(int center = 0; center < size; ++center){
        int half_length = 0;
        int left_extension = 0;
        int right_extension = 0;
        if(center < left_size){
            half_length = left_size - center;
            left_extension = z_at(z_reversed_first, left_size - center);
            right_extension =
                z_at(z_second_first, right_size + 1 + center);
        }else{
            half_length = center - left_size + 1;
            left_extension = z_at(
                z_first_second_reversed,
                left_size + right_size - (center - left_size)
            );
            right_extension = z_at(z_second, center - left_size + 1);
        }
        if(left_extension + right_extension >= half_length){
            append_crossing_repetitions(
                result,
                left,
                center < left_size,
                center,
                half_length,
                left_extension,
                right_extension
            );
        }
    }

    enumerate(text, left, middle, result);
    enumerate(text, middle, right, result);
}

}  // namespace main_lorentz_internal

inline std::vector<TandemRepeat> main_lorentz_tandem_repeats(
    const std::string& text
){
    if(text.size()
        > static_cast<std::size_t>(std::numeric_limits<int>::max())){
        throw std::length_error(
            "main_lorentz_tandem_repeats: text is too long"
        );
    }
    std::vector<TandemRepeat> result;
    main_lorentz_internal::enumerate(
        text, 0, static_cast<int>(text.size()), result
    );
    std::sort(result.begin(), result.end(), [](const TandemRepeat& left,
                                              const TandemRepeat& right){
        return std::pair(left.begin, left.half_length)
            < std::pair(right.begin, right.half_length);
    });
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_MAIN_LORENTZ_HPP_INCLUDED
