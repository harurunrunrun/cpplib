#ifndef CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_BURROWS_WHEELER_TRANSFORM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_BURROWS_WHEELER_TRANSFORM_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

struct BurrowsWheelerTransformResult{
    std::string transformed;
    int primary_index;
};

inline BurrowsWheelerTransformResult burrows_wheeler_transform(
    const std::string& text
){
    const int size = static_cast<int>(text.size());
    if(size == 0) return {"", 0};
    std::vector<int> order(static_cast<std::size_t>(size));
    std::vector<int> rank(static_cast<std::size_t>(size));
    std::vector<int> next_rank(static_cast<std::size_t>(size));
    std::vector<int> shifted(static_cast<std::size_t>(size));
    std::vector<int> count(static_cast<std::size_t>(std::max(256, size)), 0);
    for(int index = 0; index < size; ++index){
        ++count[static_cast<unsigned char>(
            text[static_cast<std::size_t>(index)]
        )];
    }
    for(int symbol = 1; symbol < 256; ++symbol){
        count[static_cast<std::size_t>(symbol)] +=
            count[static_cast<std::size_t>(symbol - 1)];
    }
    for(int index = size; index-- > 0;){
        const unsigned char symbol = static_cast<unsigned char>(
            text[static_cast<std::size_t>(index)]
        );
        order[static_cast<std::size_t>(
            --count[static_cast<std::size_t>(symbol)]
        )] = index;
    }
    int classes = 0;
    for(int index = 0; index < size; ++index){
        if(index == 0 || text[static_cast<std::size_t>(
            order[static_cast<std::size_t>(index - 1)]
        )] != text[static_cast<std::size_t>(
            order[static_cast<std::size_t>(index)]
        )]) ++classes;
        rank[static_cast<std::size_t>(
            order[static_cast<std::size_t>(index)]
        )] = classes - 1;
    }
    for(int length = 1; length < size; length <<= 1){
        std::fill(count.begin(), count.begin() + classes, 0);
        for(int index = 0; index < size; ++index){
            int start = order[static_cast<std::size_t>(index)] - length;
            if(start < 0) start += size;
            shifted[static_cast<std::size_t>(index)] = start;
            ++count[static_cast<std::size_t>(rank[static_cast<std::size_t>(
                start
            )])];
        }
        for(int id = 1; id < classes; ++id){
            count[static_cast<std::size_t>(id)] +=
                count[static_cast<std::size_t>(id - 1)];
        }
        for(int index = size; index-- > 0;){
            const int start = shifted[static_cast<std::size_t>(index)];
            order[static_cast<std::size_t>(--count[static_cast<std::size_t>(
                rank[static_cast<std::size_t>(start)]
            )])] = start;
        }
        next_rank[static_cast<std::size_t>(order[0])] = 0;
        int next_classes = 1;
        for(int index = 1; index < size; ++index){
            const int previous = order[static_cast<std::size_t>(index - 1)];
            const int current = order[static_cast<std::size_t>(index)];
            if(rank[static_cast<std::size_t>(previous)]
                    != rank[static_cast<std::size_t>(current)]
                || rank[static_cast<std::size_t>((previous + length) % size)]
                    != rank[static_cast<std::size_t>(
                        (current + length) % size
                    )]) ++next_classes;
            next_rank[static_cast<std::size_t>(current)] = next_classes - 1;
        }
        rank.swap(next_rank);
        classes = next_classes;
        if(classes == size) break;
        if(length > size / 2) break;
    }
    std::fill(count.begin(), count.begin() + classes, 0);
    for(int start = 0; start < size; ++start){
        ++count[static_cast<std::size_t>(
            rank[static_cast<std::size_t>(start)]
        )];
    }
    int prefix = 0;
    for(int id = 0; id < classes; ++id){
        const int amount = count[static_cast<std::size_t>(id)];
        count[static_cast<std::size_t>(id)] = prefix;
        prefix += amount;
    }
    for(int start = 0; start < size; ++start){
        order[static_cast<std::size_t>(count[static_cast<std::size_t>(
            rank[static_cast<std::size_t>(start)]
        )]++)] = start;
    }
    BurrowsWheelerTransformResult result;
    result.transformed.resize(static_cast<std::size_t>(size));
    result.primary_index = -1;
    for(int index = 0; index < size; ++index){
        const int start = order[static_cast<std::size_t>(index)];
        result.transformed[static_cast<std::size_t>(index)] =
            text[static_cast<std::size_t>((start + size - 1) % size)];
        if(start == 0) result.primary_index = index;
    }
    return result;
}

inline std::string inverse_burrows_wheeler_transform(
    const std::string& transformed,
    int primary_index
){
    const int size = static_cast<int>(transformed.size());
    if(size == 0){
        if(primary_index != 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid primary index "
                "(inverse_burrows_wheeler_transform)."
            );
        }
        return "";
    }
    if(primary_index < 0 || size <= primary_index)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid primary index "
            "(inverse_burrows_wheeler_transform)."
        );
    }
    std::vector<int> occurrence(static_cast<std::size_t>(size));
    std::vector<int> count(256, 0);
    for(int index = 0; index < size; ++index){
        const unsigned char symbol =
            transformed[static_cast<std::size_t>(index)];
        occurrence[static_cast<std::size_t>(index)] =
            count[static_cast<std::size_t>(symbol)]++;
    }
    int prefix = 0;
    for(int symbol = 0; symbol < 256; ++symbol){
        const int frequency = count[static_cast<std::size_t>(symbol)];
        count[static_cast<std::size_t>(symbol)] = prefix;
        prefix += frequency;
    }
    std::string result(static_cast<std::size_t>(size), '\0');
    int row = primary_index;
    for(int position = size; position-- > 0;){
        const unsigned char symbol =
            transformed[static_cast<std::size_t>(row)];
        result[static_cast<std::size_t>(position)] =
            static_cast<char>(symbol);
        row = count[static_cast<std::size_t>(symbol)]
            + occurrence[static_cast<std::size_t>(row)];
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_BURROWS_WHEELER_TRANSFORM_HPP_INCLUDED
