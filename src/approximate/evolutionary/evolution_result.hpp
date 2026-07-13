#pragma once

#include <cstddef>

namespace approximate::evolutionary {

template<class Individual, class Score>
struct EvolutionResult{
    Individual individual;
    Score score;
    std::size_t iterations = 0;
    std::size_t evaluations = 0;
};

}  // namespace approximate::evolutionary
