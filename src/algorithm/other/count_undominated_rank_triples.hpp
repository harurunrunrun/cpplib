#ifndef CPPLIB_SRC_ALGORITHM_OTHER_COUNT_UNDOMINATED_RANK_TRIPLES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_COUNT_UNDOMINATED_RANK_TRIPLES_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

#include "../../structure/segtree/segtree.hpp"
#include "../../structure/types/common_monoids.hpp"

template<int MAX_SIZE = 100000>
int count_undominated_rank_triples(
    const std::vector<std::array<int, 3>>& ranks
){
    const int size = static_cast<int>(ranks.size());
    std::vector<std::array<int, 3>> ordered = ranks;
    for(const auto& rank: ordered){
        for(int value: rank){
            if(value < 0 || size <= value)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: rank out of range "
                    "(count_undominated_rank_triples)."
                );
            }
        }
    }
    std::sort(ordered.begin(), ordered.end());
    constexpr MinMonoid<int> minimum{};
    std::vector<int> initial(
        static_cast<std::size_t>(size),
        std::numeric_limits<int>::max()
    );
    auto tree = std::make_unique<Segtree<minimum, MAX_SIZE>>(initial);
    int answer = 0;
    for(const auto& rank: ordered){
        if(tree->prod(0, rank[1]) >= rank[2]) ++answer;
        tree->set(rank[1], std::min(tree->get(rank[1]), rank[2]));
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_COUNT_UNDOMINATED_RANK_TRIPLES_HPP_INCLUDED
