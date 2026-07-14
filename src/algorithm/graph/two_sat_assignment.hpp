#pragma once

#include <optional>
#include <vector>

#include "two_sat.hpp"

inline std::optional<std::vector<int>> two_sat_assignment(TwoSat& solver){
    if(!solver.satisfiable()) return std::nullopt;
    return solver.answer;
}
