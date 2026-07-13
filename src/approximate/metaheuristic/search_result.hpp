#pragma once

#include <cstddef>

namespace approximate::metaheuristic {

template<class State, class Score>
struct SearchResult{
    State state;
    Score score;
    std::size_t iterations = 0;
    std::size_t evaluations = 0;
    std::size_t accepted_moves = 0;
};

}  // namespace approximate::metaheuristic
