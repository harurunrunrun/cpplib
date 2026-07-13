#pragma once

#include <cmath>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace approximate::evolutionary::detail {

template<class Evaluate, class Individual>
auto evaluate_objective(Evaluate& evaluate, const Individual& individual){
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const Individual&>
    >;
    Score score = std::invoke(evaluate, individual);
    if constexpr(std::is_floating_point_v<Score>){
        if(!std::isfinite(score))[[unlikely]]{
            throw std::domain_error("objective function returned a non-finite score");
        }
    }
    return score;
}

}  // namespace approximate::evolutionary::detail
