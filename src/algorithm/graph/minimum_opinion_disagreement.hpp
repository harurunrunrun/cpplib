#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

#include "../matching/dinic.hpp"

inline int minimum_opinion_disagreement(
    const std::vector<bool>& beliefs,
    const std::vector<std::pair<int, int>>& friendships
){
    const int n = static_cast<int>(beliefs.size());
    const int source = n;
    const int sink = n + 1;
    Dinic<int> flow(n + 2);
    for(int guard = 0; guard < n; guard++){
        if(beliefs[static_cast<std::size_t>(guard)]){
            flow.add_edge(source, guard, 1);
        }else{
            flow.add_edge(guard, sink, 1);
        }
    }
    for(const auto& [left, right]: friendships){
        if(left < 0 || n <= left || right < 0 || n <= right || left == right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(minimum_opinion_disagreement)."
            );
        }
        flow.add_edge(left, right, 1);
        flow.add_edge(right, left, 1);
    }
    return flow.max_flow(source, sink);
}
