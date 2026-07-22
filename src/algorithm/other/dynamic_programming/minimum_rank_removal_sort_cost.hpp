#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MINIMUM_RANK_REMOVAL_SORT_COST_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MINIMUM_RANK_REMOVAL_SORT_COST_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

inline long long minimum_rank_removal_sort_cost(
    const std::vector<int>& permutation
){
    if(permutation.size() >
       static_cast<std::size_t>(std::numeric_limits<int>::max() - 2))[[unlikely]]{
        throw std::length_error(
            "library assertion fault: permutation is too long "
            "(minimum_rank_removal_sort_cost)."
        );
    }
    const int n = static_cast<int>(permutation.size());
    if(n == 0) return 0;

    std::vector<int> position(static_cast<std::size_t>(n + 1), -1);
    for(int index = 0; index < n; index++){
        const int value = permutation[static_cast<std::size_t>(index)];
        if(value < 1 ||
           value > n ||
           position[static_cast<std::size_t>(value)] != -1)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: input is not a permutation of [1, N] "
                "(minimum_rank_removal_sort_cost)."
            );
        }
        position[static_cast<std::size_t>(value)] = index;
    }

    std::vector<std::vector<int>> before_prefix(
        static_cast<std::size_t>(n + 1),
        std::vector<int>(static_cast<std::size_t>(n + 1))
    );
    for(int selected = 1; selected <= n; selected++){
        for(int value = 1; value <= n; value++){
            before_prefix[static_cast<std::size_t>(selected)]
                         [static_cast<std::size_t>(value)] =
                before_prefix[static_cast<std::size_t>(selected)]
                             [static_cast<std::size_t>(value - 1)] +
                (position[static_cast<std::size_t>(value)] <
                         position[static_cast<std::size_t>(selected)]
                     ? 1
                     : 0);
        }
    }

    std::vector<std::vector<long long>> minimum_cost(
        static_cast<std::size_t>(n + 2),
        std::vector<long long>(static_cast<std::size_t>(n + 2))
    );
    for(int value = 1; value <= n; value++){
        minimum_cost[static_cast<std::size_t>(value)]
                    [static_cast<std::size_t>(value)] =
            position[static_cast<std::size_t>(value)] + 1LL;
    }

    for(int length = 2; length <= n; length++){
        for(int left = 1; left + length - 1 <= n; left++){
            const int right = left + length - 1;
            const int removed_before_left =
                before_prefix[static_cast<std::size_t>(left)]
                             [static_cast<std::size_t>(right)] -
                before_prefix[static_cast<std::size_t>(left)]
                             [static_cast<std::size_t>(left)];
            const int rank_left =
                position[static_cast<std::size_t>(left)] + 1 -
                removed_before_left;

            const int removed_before_right =
                before_prefix[static_cast<std::size_t>(right)]
                             [static_cast<std::size_t>(right - 1)] -
                before_prefix[static_cast<std::size_t>(right)]
                             [static_cast<std::size_t>(left - 1)];
            const int rank_right =
                position[static_cast<std::size_t>(right)] + 1 -
                removed_before_right;

            const __int128 remove_left =
                static_cast<__int128>(
                    minimum_cost[static_cast<std::size_t>(left + 1)]
                                [static_cast<std::size_t>(right)]
                ) +
                static_cast<__int128>(rank_left) * length;
            const __int128 remove_right =
                static_cast<__int128>(
                    minimum_cost[static_cast<std::size_t>(left)]
                                [static_cast<std::size_t>(right - 1)]
                ) +
                static_cast<__int128>(rank_right) * length;
            const __int128 best = std::min(remove_left, remove_right);
            if(best > std::numeric_limits<long long>::max())[[unlikely]]{
                throw std::overflow_error(
                    "library assertion fault: result overflow "
                    "(minimum_rank_removal_sort_cost)."
                );
            }
            minimum_cost[static_cast<std::size_t>(left)]
                        [static_cast<std::size_t>(right)] =
                static_cast<long long>(best);
        }
    }
    return minimum_cost[1][static_cast<std::size_t>(n)];
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MINIMUM_RANK_REMOVAL_SORT_COST_HPP_INCLUDED
