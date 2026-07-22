#ifndef CPPLIB_SRC_ALGORITHM_OTHER_OFFLINE_QUERY_PARALLEL_BINARY_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_OFFLINE_QUERY_PARALLEL_BINARY_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

template<class Reset, class Apply, class Predicate>
std::vector<int> parallel_binary_search(
    int query_count,
    int operation_count,
    Reset&& reset,
    Apply&& apply,
    Predicate&& predicate
){
    if(query_count < 0){
        throw std::invalid_argument("parallel_binary_search: negative query count");
    }
    if(operation_count < 0){
        throw std::invalid_argument("parallel_binary_search: negative operation count");
    }
    if(operation_count == std::numeric_limits<int>::max()){
        throw std::invalid_argument("parallel_binary_search: operation count has no int sentinel");
    }
    if(query_count == 0) return {};

    std::vector<int> lower(static_cast<std::size_t>(query_count), -1);
    std::vector<int> upper(
        static_cast<std::size_t>(query_count),
        operation_count + 1
    );
    std::vector<int> head(static_cast<std::size_t>(operation_count + 1), -1);
    std::vector<int> next(static_cast<std::size_t>(query_count), -1);

    while(true){
        std::fill(head.begin(), head.end(), -1);
        int maximum_middle = -1;

        // Prepending in descending query order makes every bucket ascending.
        for(int query = query_count - 1; query >= 0; --query){
            const std::size_t index = static_cast<std::size_t>(query);
            const long long difference =
                static_cast<long long>(upper[index]) - lower[index];
            if(difference <= 1) continue;
            const int middle = lower[index] + static_cast<int>(difference / 2);
            next[index] = head[static_cast<std::size_t>(middle)];
            head[static_cast<std::size_t>(middle)] = query;
            maximum_middle = std::max(maximum_middle, middle);
        }
        if(maximum_middle < 0) break;

        reset();
        for(int applied_count = 0; applied_count <= maximum_middle; ++applied_count){
            if(applied_count != 0) apply(applied_count - 1);
            for(
                int query = head[static_cast<std::size_t>(applied_count)];
                query != -1;
                query = next[static_cast<std::size_t>(query)]
            ){
                const std::size_t index = static_cast<std::size_t>(query);
                if(static_cast<bool>(predicate(query))){
                    upper[index] = applied_count;
                }else{
                    lower[index] = applied_count;
                }
            }
        }
    }
    return upper;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_OFFLINE_QUERY_PARALLEL_BINARY_SEARCH_HPP_INCLUDED
