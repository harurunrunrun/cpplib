#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_GRID_SQUARE_COUNT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_GRID_SQUARE_COUNT_HPP_INCLUDED

#include <limits>
#include <stdexcept>

inline long long grid_square_count(long long side_length){
    if(side_length < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative side length (grid_square_count)."
        );
    }
    const __int128 result = static_cast<__int128>(side_length) *
                            (side_length + 1) *
                            (2 * static_cast<__int128>(side_length) + 1) / 6;
    if(result > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "library assertion fault: result overflow (grid_square_count)."
        );
    }
    return static_cast<long long>(result);
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_GRID_SQUARE_COUNT_HPP_INCLUDED
