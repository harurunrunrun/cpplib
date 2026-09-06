#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_CONSTRUCTION_GRID_HAMILTONIAN_PATH_WITH_DIRECTION_COUNTS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_CONSTRUCTION_GRID_HAMILTONIAN_PATH_WITH_DIRECTION_COUNTS_HPP_INCLUDED

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

inline bool can_construct_grid_hamiltonian_path_with_direction_counts(
    long long n, long long m,
    long long l, long long r, long long u, long long d
){
    if(n <= 0 || m <= 0 || l < 0 || r < 0 || u < 0 || d < 0){
        return false;
    }
    if(n % 2 == 0 && m % 2 == 0) return false;
    if(r - l != m - 1 || d - u != n - 1) return false;
    using Wide = __int128;
    if(Wide(l) + r + u + d != Wide(n) * m - 1) return false;

    // Transpose so that the number of columns is odd.
    if(m % 2 == 0){
        std::swap(n, m);
        std::swap(l, u);
    }
    return l % 2 == 0 && Wide(l) <= Wide((n - 1) / 2) * (m - 1);
}

// Coordinates are (row, column); D increases the row and R the column.
// The returned moves visit every cell once, from (1, 1) to (n, m).
inline std::optional<std::string> grid_hamiltonian_path_with_direction_counts(
    long long n, long long m,
    long long l, long long r, long long u, long long d
){
    if(!can_construct_grid_hamiltonian_path_with_direction_counts(
        n, m, l, r, u, d
    )) return std::nullopt;

    std::string path;
    const __int128 length = __int128(n) * m - 1;
    if(length > __int128(path.max_size())){
        throw std::length_error(
            "grid_hamiltonian_path_with_direction_counts path too large"
        );
    }
    path.reserve(static_cast<std::size_t>(length));

    const bool transposed = m % 2 == 0;
    if(transposed){
        std::swap(n, m);
        std::swap(l, u);
    }
    const auto append = [&](char direction, long long count){
        if(transposed){
            switch(direction){
                case 'L': direction = 'U'; break;
                case 'R': direction = 'D'; break;
                case 'U': direction = 'L'; break;
                case 'D': direction = 'R'; break;
            }
        }
        path.append(static_cast<std::size_t>(count), direction);
    };

    // A full two-row band consumes m - 1 left moves and no up moves.
    while(m > 1 && l >= m - 1){
        append('R', m - 1);
        append('D', 1);
        append('L', m - 1);
        append('D', 1);
        n -= 2;
        l -= m - 1;
    }
    if(l == 0){
        for(long long column = 0; column < m; ++column){
            append(column % 2 == 0 ? 'D' : 'U', n - 1);
            if(column + 1 < m) append('R', 1);
        }
    }else{
        // Cover the top two rows of an odd-width (l + 1) prefix.
        append('R', l);
        append('D', 1);
        append('L', l);
        append('D', 1);
        for(long long column = 0; column <= l; ++column){
            append(column % 2 == 0 ? 'D' : 'U', n - 3);
            if(column < l) append('R', 1);
        }
        // The remaining even number of columns starts at the bottom.
        const long long remaining_columns = m - l - 1;
        for(long long column = 0; column < remaining_columns; ++column){
            append('R', 1);
            append(column % 2 == 0 ? 'U' : 'D', n - 1);
        }
    }
    return path;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_CONSTRUCTION_GRID_HAMILTONIAN_PATH_WITH_DIRECTION_COUNTS_HPP_INCLUDED
