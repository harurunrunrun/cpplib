#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_STOER_WAGNER_MINIMUM_CUT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_STOER_WAGNER_MINIMUM_CUT_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

struct StoerWagnerEdge{
    int from;
    int to;
    long long capacity;
};

struct StoerWagnerMinimumCutResult{
    long long capacity;
    std::vector<int> side;
};

inline StoerWagnerMinimumCutResult stoer_wagner_minimum_cut(
    int vertex_count,
    const std::vector<StoerWagnerEdge>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation "
            "(stoer_wagner_minimum_cut)."
        );
    }
    std::vector<std::vector<long long>> capacity(
        static_cast<std::size_t>(vertex_count),
        std::vector<long long>(static_cast<std::size_t>(vertex_count), 0)
    );
    __int128 total_capacity = 0;
    for(const auto& edge: edges){
        if(edge.from < 0 || vertex_count <= edge.from
            || edge.to < 0 || vertex_count <= edge.to
            || edge.capacity < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(stoer_wagner_minimum_cut)."
            );
        }
        if(edge.from == edge.to) continue;
        total_capacity += static_cast<__int128>(edge.capacity);
        if(total_capacity > std::numeric_limits<long long>::max())[[unlikely]]{
            throw std::overflow_error(
                "stoer_wagner_minimum_cut capacity sum overflows long long"
            );
        }
        capacity[static_cast<std::size_t>(edge.from)]
                [static_cast<std::size_t>(edge.to)] += edge.capacity;
        capacity[static_cast<std::size_t>(edge.to)]
                [static_cast<std::size_t>(edge.from)] += edge.capacity;
    }
    if(vertex_count <= 1){
        std::vector<int> side;
        if(vertex_count == 1) side.push_back(0);
        return {0, std::move(side)};
    }

    std::vector<int> active(static_cast<std::size_t>(vertex_count));
    std::vector<std::vector<int>> merged(static_cast<std::size_t>(vertex_count));
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        active[static_cast<std::size_t>(vertex)] = vertex;
        merged[static_cast<std::size_t>(vertex)].push_back(vertex);
    }
    long long best = std::numeric_limits<long long>::max();
    std::vector<int> best_side;
    while(active.size() > 1){
        std::vector<long long> weight(
            static_cast<std::size_t>(vertex_count), 0
        );
        std::vector<unsigned char> added(
            static_cast<std::size_t>(vertex_count), 0
        );
        int previous = -1;
        for(std::size_t phase = 0; phase < active.size(); ++phase){
            int selected = -1;
            for(int vertex: active){
                if(added[static_cast<std::size_t>(vertex)] == 0
                    && (selected == -1
                        || weight[static_cast<std::size_t>(selected)]
                            < weight[static_cast<std::size_t>(vertex)])){
                    selected = vertex;
                }
            }
            if(phase + 1 == active.size()){
                long long cut = weight[static_cast<std::size_t>(selected)];
                if(cut < best){
                    best = cut;
                    best_side = merged[static_cast<std::size_t>(selected)];
                }
                for(int vertex: active){
                    if(vertex == selected || vertex == previous) continue;
                    capacity[static_cast<std::size_t>(previous)]
                            [static_cast<std::size_t>(vertex)] +=
                        capacity[static_cast<std::size_t>(selected)]
                                [static_cast<std::size_t>(vertex)];
                    capacity[static_cast<std::size_t>(vertex)]
                            [static_cast<std::size_t>(previous)] =
                        capacity[static_cast<std::size_t>(previous)]
                                [static_cast<std::size_t>(vertex)];
                }
                auto& destination = merged[static_cast<std::size_t>(previous)];
                auto& source = merged[static_cast<std::size_t>(selected)];
                destination.insert(destination.end(), source.begin(), source.end());
                active.erase(
                    std::find(active.begin(), active.end(), selected)
                );
                break;
            }
            added[static_cast<std::size_t>(selected)] = 1;
            for(int vertex: active){
                if(added[static_cast<std::size_t>(vertex)] == 0){
                    weight[static_cast<std::size_t>(vertex)] +=
                        capacity[static_cast<std::size_t>(selected)]
                                [static_cast<std::size_t>(vertex)];
                }
            }
            previous = selected;
        }
    }
    std::sort(best_side.begin(), best_side.end());
    return {best, std::move(best_side)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_STOER_WAGNER_MINIMUM_CUT_HPP_INCLUDED
