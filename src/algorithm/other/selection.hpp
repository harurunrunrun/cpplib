#ifndef CPPLIB_SRC_ALGORITHM_OTHER_SELECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_SELECTION_HPP_INCLUDED

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace deterministic_selection_internal{

template<std::random_access_iterator Iterator, class Compare>
void insertion_sort(Iterator first, Iterator last, Compare& compare){
    for(Iterator current = first + 1; current < last; ++current){
        Iterator position = current;
        while(first < position && std::invoke(compare, *position, *(position - 1))){
            std::iter_swap(position, position - 1);
            --position;
        }
    }
}

template<std::random_access_iterator Iterator, class Compare>
Iterator select(Iterator first, Iterator last, std::size_t rank, Compare& compare){
    using value_type = std::iter_value_t<Iterator>;
    while(true){
        const std::size_t size = static_cast<std::size_t>(last - first);
        if(size <= 32){
            insertion_sort(first, last, compare);
            return first + static_cast<std::iter_difference_t<Iterator>>(rank);
        }

        const std::size_t group_count = (size + 4) / 5;
        for(std::size_t group = 0; group < group_count; ++group){
            Iterator group_first = first
                + static_cast<std::iter_difference_t<Iterator>>(group * 5);
            const std::size_t remaining = size - group * 5;
            Iterator group_last = group_first
                + static_cast<std::iter_difference_t<Iterator>>(std::min<std::size_t>(5, remaining));
            insertion_sort(group_first, group_last, compare);
            Iterator median = group_first + (group_last - group_first) / 2;
            std::iter_swap(
                first + static_cast<std::iter_difference_t<Iterator>>(group),
                median
            );
        }

        Iterator pivot_iterator = select(
            first,
            first + static_cast<std::iter_difference_t<Iterator>>(group_count),
            group_count / 2,
            compare
        );
        const value_type pivot = *pivot_iterator;

        Iterator less_end = first;
        Iterator current = first;
        Iterator greater_begin = last;
        while(current < greater_begin){
            if(std::invoke(compare, *current, pivot)){
                std::iter_swap(less_end, current);
                ++less_end;
                ++current;
            }else if(std::invoke(compare, pivot, *current)){
                --greater_begin;
                std::iter_swap(current, greater_begin);
            }else{
                ++current;
            }
        }

        const std::size_t less_count = static_cast<std::size_t>(less_end - first);
        const std::size_t equal_count = static_cast<std::size_t>(greater_begin - less_end);
        if(rank < less_count){
            last = less_end;
        }else if(rank < less_count + equal_count){
            return less_end;
        }else{
            rank -= less_count + equal_count;
            first = greater_begin;
        }
    }
}

} // namespace deterministic_selection_internal

template<std::random_access_iterator Iterator, class Compare = std::less<>>
requires std::copy_constructible<std::iter_value_t<Iterator>>
Iterator median_of_medians_select(
    Iterator first,
    Iterator last,
    std::size_t rank,
    Compare compare = {}
){
    if(last < first)[[unlikely]]{
        throw std::invalid_argument("median_of_medians_select received a reversed range");
    }
    const std::size_t size = static_cast<std::size_t>(last - first);
    if(size <= rank)[[unlikely]]{
        throw std::out_of_range("median_of_medians_select rank is outside the range");
    }
    return deterministic_selection_internal::select(first, last, rank, compare);
}

template<std::forward_iterator Iterator>
requires std::equality_comparable<std::iter_value_t<Iterator>>
std::optional<std::iter_value_t<Iterator>> boyer_moore_majority(
    Iterator first,
    Iterator last
){
    using value_type = std::iter_value_t<Iterator>;
    std::optional<value_type> candidate;
    std::size_t balance = 0;
    std::size_t size = 0;
    for(Iterator iterator = first; iterator != last; ++iterator){
        ++size;
        if(balance == 0){
            candidate = *iterator;
            balance = 1;
        }else if(*candidate == *iterator){
            ++balance;
        }else{
            --balance;
        }
    }
    if(!candidate) return std::nullopt;

    std::size_t count = 0;
    for(Iterator iterator = first; iterator != last; ++iterator){
        if(*iterator == *candidate) ++count;
    }
    if(size / 2 < count) return candidate;
    return std::nullopt;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_SELECTION_HPP_INCLUDED
