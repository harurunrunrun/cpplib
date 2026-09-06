// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <vector>

#include "../../src/algorithm/graph/specialized/construction/grid_hamiltonian_path_with_direction_counts.hpp"

namespace{

using Counts = std::array<long long, 4>;
constexpr int dr[] = {0, 0, -1, 1};
constexpr int dc[] = {-1, 1, 0, 0};

void validate(int n, int m, const Counts& expected, const std::string& path){
    assert(path.size() == static_cast<std::size_t>(n) * m - 1);
    std::vector<bool> seen(static_cast<std::size_t>(n) * m, false);
    int row = 0, column = 0;
    seen[0] = true;
    Counts actual{};
    for(char move: path){
        const std::size_t direction = std::string("LRUD").find(move);
        assert(direction < 4);
        ++actual[direction];
        row += dr[direction];
        column += dc[direction];
        assert(0 <= row && row < n && 0 <= column && column < m);
        const std::size_t vertex = static_cast<std::size_t>(row) * m + column;
        assert(!seen[vertex]);
        seen[vertex] = true;
    }
    assert(row == n - 1 && column == m - 1);
    assert(actual == expected);
    for(bool visited: seen) assert(visited);
}

bool possible(long long n, long long m, const Counts& counts){
    return can_construct_grid_hamiltonian_path_with_direction_counts(
        n, m, counts[0], counts[1], counts[2], counts[3]
    );
}

std::optional<std::string> construct(
    long long n, long long m, const Counts& counts
){
    return grid_hamiltonian_path_with_direction_counts(
        n, m, counts[0], counts[1], counts[2], counts[3]
    );
}

// Enumerate paths themselves; the oracle does not use the feasibility formula.
std::vector<bool> brute_force(int n, int m){
    const int area = n * m;
    std::vector<bool> feasible(static_cast<std::size_t>(area), false);
    const auto dfs = [&](auto&& self, int vertex, std::uint64_t seen,
                         int visited, int left_count) -> void{
        if(vertex == area - 1){
            if(visited == area) feasible[static_cast<std::size_t>(left_count)] = true;
            return;
        }
        const int row = vertex / m, column = vertex % m;
        for(int direction = 0; direction < 4; ++direction){
            const int next_row = row + dr[direction];
            const int next_column = column + dc[direction];
            if(next_row < 0 || next_row >= n ||
               next_column < 0 || next_column >= m) continue;
            const int next = next_row * m + next_column;
            const std::uint64_t bit = std::uint64_t(1) << next;
            if(seen & bit) continue;
            self(self, next, seen | bit, visited + 1,
                 left_count + (direction == 0));
        }
    };
    dfs(dfs, 0, 1, 1, 0);
    return feasible;
}

void exhaustive_small(){
    for(int n = 1; n <= 25; ++n){
        for(int m = 1; n * m <= 25; ++m){
            const auto feasible = brute_force(n, m);
            const int budget = (n - 1) * (m - 1) / 2;
            for(int l = 0; l < n * m; ++l){
                const int u = budget - l;
                const Counts counts{l, l + m - 1, u, u + n - 1};
                const bool expected = feasible[static_cast<std::size_t>(l)];
                assert(possible(n, m, counts) == expected);
                const auto path = construct(n, m, counts);
                assert(path.has_value() == expected);
                if(path) validate(n, m, counts, *path);
            }
            // Enumerate all count tuples on the smallest grids to check
            // rejection of wrong lengths and displacements independently.
            if(n * m > 6) continue;
            for(int l = 0; l <= n * m; ++l)
            for(int r = 0; r <= n * m; ++r)
            for(int u = 0; u <= n * m; ++u)
            for(int d = 0; d <= n * m; ++d){
                const Counts counts{l, r, u, d};
                const bool expected =
                    l < n * m && feasible[static_cast<std::size_t>(l)] &&
                    r - l == m - 1 && d - u == n - 1 &&
                    l + r + u + d == n * m - 1;
                assert(possible(n, m, counts) == expected);
                const auto path = construct(n, m, counts);
                assert(path.has_value() == expected);
                if(path) validate(n, m, counts, *path);
            }
        }
    }
}

void construction_tests(){
    for(int n = 1; n <= 25; ++n){
        for(int m = 1; m <= 25; ++m){
            const int budget = (n - 1) * (m - 1) / 2;
            for(int l = 0; l <= budget; ++l){
                const int u = budget - l;
                const Counts counts{l, l + m - 1, u, u + n - 1};
                const auto path = construct(n, m, counts);
                assert(path.has_value() == possible(n, m, counts));
                const Counts transposed{u, u + n - 1, l, l + m - 1};
                assert(possible(n, m, counts) == possible(m, n, transposed));
                if(path) validate(n, m, counts, *path);
            }
        }
    }
    const auto check_large = [](int n, int m, long long l){
        const long long u = (static_cast<long long>(n) - 1) * (m - 1) / 2 - l;
        const Counts counts{l, l + m - 1, u, u + n - 1};
        const auto path = construct(n, m, counts);
        assert(path);
        validate(n, m, counts, *path);
    };
    check_large(1, 1000000, 0);
    check_large(1000000, 1, 0);
    check_large(1001, 1001, 0);
    check_large(1001, 1001, 250002);
    check_large(1001, 1001, 500000);
    check_large(1000, 1001, 499000);
    check_large(1001, 1000, 502);
}

void boundary_tests(){
    const long long hi = std::numeric_limits<long long>::max();
    const long long lo = std::numeric_limits<long long>::min();
    for(long long n: {lo, -1LL, 0LL}){
        assert(!possible(n, 1, {0, 0, 0, 0}));
        assert(!construct(n, 1, {0, 0, 0, 0}));
        assert(!possible(1, n, {0, 0, 0, 0}));
        assert(!construct(1, n, {0, 0, 0, 0}));
    }
    for(int direction = 0; direction < 4; ++direction){
        for(long long value: {lo, -1LL, hi}){
            Counts counts{};
            counts[direction] = value;
            assert(!possible(1, 1, counts));
            assert(!construct(1, 1, counts));
        }
    }
    assert(possible(1, hi, {0, hi - 1, 0, 0}));
    assert(possible(hi, 1, {0, 0, 0, hi - 1}));
    // Valid input whose area and count sum exceed signed 64-bit range.
    const long long width = hi / 3 + 1;
    assert(possible(3, width, {0, width - 1, width - 1, width + 1}));
    assert(possible(width, 3, {width - 1, width + 1, 0, width - 1}));
    assert(!possible(hi, hi, {hi, hi, hi, hi}));
    assert(!construct(hi, hi, {hi, hi, hi, hi}));
    assert(!possible(2, 2, {0, 1, 0, 1}));
    assert(!possible(3, 3, {1, 3, 1, 3}));
    assert(!possible(2, 5, {2, 6, 0, 1}));
    assert(construct(1, 1, {0, 0, 0, 0}) == std::optional<std::string>(""));
    assert(construct(3, 3, {2, 4, 0, 2}) == std::optional<std::string>("RRDLLDRR"));

    if(__int128(hi) - 1 > __int128(std::string().max_size())){
        bool threw = false;
        try{
            (void)construct(1, hi, {0, hi - 1, 0, 0});
        }catch(const std::length_error&){
            threw = true;
        }
        assert(threw);
    }
}

} // namespace

int main(){
    exhaustive_small();
    construction_tests();
    boundary_tests();
}
