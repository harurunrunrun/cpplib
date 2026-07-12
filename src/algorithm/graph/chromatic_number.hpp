#pragma once

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>

int chromatic_number(const std::vector<std::vector<int>>& graph){
    const int n = static_cast<int>(graph.size());
    if(n > 24)[[unlikely]]{
        throw std::runtime_error("library assertion fault: n is too large (chromatic_number).");
    }
    std::vector<std::uint32_t> adjacent(static_cast<std::size_t>(n), 0);
    for(int v = 0; v < n; v++){
        for(int to: graph[static_cast<std::size_t>(v)]){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (chromatic_number).");
            }
            if(to != v) adjacent[static_cast<std::size_t>(v)] |= std::uint32_t(1) << to;
        }
    }
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            bool a = adjacent[static_cast<std::size_t>(i)] >> j & 1U;
            bool b = adjacent[static_cast<std::size_t>(j)] >> i & 1U;
            if(a != b)[[unlikely]]{
                throw std::runtime_error("library assertion fault: graph must be undirected (chromatic_number).");
            }
        }
    }

    const int size = 1 << n;
    std::vector<char> independent(static_cast<std::size_t>(size), 0);
    independent[0] = 1;
    for(int mask = 1; mask < size; mask++){
        int v = __builtin_ctz(static_cast<unsigned>(mask));
        int rest = mask ^ (1 << v);
        independent[static_cast<std::size_t>(mask)] =
            independent[static_cast<std::size_t>(rest)] &&
            ((adjacent[static_cast<std::size_t>(v)] & static_cast<std::uint32_t>(rest)) == 0);
    }

    std::vector<int> dp(static_cast<std::size_t>(size), n + 1);
    dp[0] = 0;
    for(int mask = 1; mask < size; mask++){
        int bit = mask & -mask;
        for(int sub = mask; sub; sub = (sub - 1) & mask){
            if((sub & bit) && independent[static_cast<std::size_t>(sub)]){
                dp[static_cast<std::size_t>(mask)] =
                    std::min(dp[static_cast<std::size_t>(mask)], dp[static_cast<std::size_t>(mask ^ sub)] + 1);
            }
        }
    }
    return dp[static_cast<std::size_t>(size - 1)];
}
