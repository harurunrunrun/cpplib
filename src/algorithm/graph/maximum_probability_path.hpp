#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_MAXIMUM_PROBABILITY_PATH_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_MAXIMUM_PROBABILITY_PATH_HPP_INCLUDED

#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
struct ProbabilityEdge{
    int to;
    T probability;
};

template<class T>
struct MaximumProbabilityPathResult{
    std::vector<T> probability;
    std::vector<int> parent;
    std::vector<char> reachable;
};

template<class T>
MaximumProbabilityPathResult<T> maximum_probability_path(
    const std::vector<std::vector<ProbabilityEdge<T>>>& graph,
    int source
){
    const int n = static_cast<int>(graph.size());
    if(source < 0 || n <= source)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (maximum_probability_path)."
        );
    }
    for(const auto& edges: graph){
        for(const auto& edge: edges){
            if(
                edge.to < 0 || n <= edge.to ||
                !(T(0) <= edge.probability && edge.probability <= T(1))
            )[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation (maximum_probability_path)."
                );
            }
        }
    }

    MaximumProbabilityPathResult<T> result;
    result.probability.assign(static_cast<std::size_t>(n), T(0));
    result.parent.assign(static_cast<std::size_t>(n), -1);
    result.reachable.assign(static_cast<std::size_t>(n), 0);

    using State = std::pair<T, int>;
    std::priority_queue<State> queue;
    result.probability[static_cast<std::size_t>(source)] = T(1);
    result.reachable[static_cast<std::size_t>(source)] = 1;
    queue.push({T(1), source});

    while(!queue.empty()){
        const auto [probability, vertex] = queue.top();
        queue.pop();
        if(probability != result.probability[static_cast<std::size_t>(vertex)]) continue;
        for(const auto& edge: graph[static_cast<std::size_t>(vertex)]){
            const T candidate = probability * edge.probability;
            const std::size_t to = static_cast<std::size_t>(edge.to);
            if(!result.reachable[to] || result.probability[to] < candidate){
                result.probability[to] = candidate;
                result.parent[to] = vertex;
                result.reachable[to] = 1;
                queue.push({candidate, edge.to});
            }
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_MAXIMUM_PROBABILITY_PATH_HPP_INCLUDED
