#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_MINIMUM_COST_CIRCULATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_MINIMUM_COST_CIRCULATION_HPP_INCLUDED

#include <optional>
#include <vector>

#include "minimum_cost_b_flow.hpp"

using MinimumCostCirculationEdge = MinimumCostBFlowEdge;

inline std::optional<MinimumCostBFlowResult> minimum_cost_circulation(
    int vertex_count,
    const std::vector<MinimumCostCirculationEdge>& edges
){
    MinimumCostBFlow solver(vertex_count);
    for(const auto& edge: edges){
        solver.add_edge(
            edge.from, edge.to, edge.lower, edge.upper, edge.cost
        );
    }
    return solver.solve();
}

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_FLOW_MINIMUM_COST_CIRCULATION_HPP_INCLUDED
