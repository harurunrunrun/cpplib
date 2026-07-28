#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_ROUTE_OPTIMIZATION_MINIMUM_MEAN_CYCLE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_ROUTE_OPTIMIZATION_MINIMUM_MEAN_CYCLE_HPP_INCLUDED

#include <limits>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <vector>

struct MinimumMeanCycleEdge{
    int from;
    int to;
    long long weight;
};

struct MinimumMeanCycleResult{
    long long numerator;
    int denominator;
    long double mean;
};

inline std::optional<MinimumMeanCycleResult> minimum_mean_cycle(
    int vertex_count,
    const std::vector<MinimumMeanCycleEdge>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (minimum_mean_cycle)."
        );
    }
    for(const auto& edge: edges){
        if(edge.from < 0 || vertex_count <= edge.from
            || edge.to < 0 || vertex_count <= edge.to)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(minimum_mean_cycle)."
            );
        }
    }
    if(vertex_count == 0) return std::nullopt;
    using Wide = __int128;
    constexpr Wide infinity = Wide{1} << 120;
    std::vector<std::vector<Wide>> distance(
        static_cast<std::size_t>(vertex_count + 1),
        std::vector<Wide>(static_cast<std::size_t>(vertex_count), infinity)
    );
    std::fill(distance[0].begin(), distance[0].end(), Wide{0});
    for(int length = 1; length <= vertex_count; ++length){
        for(const auto& edge: edges){
            Wide previous = distance[static_cast<std::size_t>(length - 1)]
                [static_cast<std::size_t>(edge.from)];
            if(previous == infinity) continue;
            Wide& current = distance[static_cast<std::size_t>(length)]
                [static_cast<std::size_t>(edge.to)];
            current = std::min(
                current, previous + static_cast<Wide>(edge.weight)
            );
        }
    }

    bool found = false;
    Wide best_numerator = 0;
    int best_denominator = 1;
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        Wide final_distance =
            distance[static_cast<std::size_t>(vertex_count)]
                [static_cast<std::size_t>(vertex)];
        if(final_distance == infinity) continue;
        bool has_candidate = false;
        Wide maximum_numerator = 0;
        int maximum_denominator = 1;
        for(int length = 0; length < vertex_count; ++length){
            Wide prefix = distance[static_cast<std::size_t>(length)]
                [static_cast<std::size_t>(vertex)];
            if(prefix == infinity) continue;
            Wide numerator = final_distance - prefix;
            int denominator = vertex_count - length;
            if(!has_candidate
                || maximum_numerator * static_cast<Wide>(denominator)
                    < numerator * static_cast<Wide>(maximum_denominator)){
                has_candidate = true;
                maximum_numerator = numerator;
                maximum_denominator = denominator;
            }
        }
        if(has_candidate
            && (!found
                || maximum_numerator * static_cast<Wide>(best_denominator)
                    < best_numerator
                        * static_cast<Wide>(maximum_denominator))){
            found = true;
            best_numerator = maximum_numerator;
            best_denominator = maximum_denominator;
        }
    }
    if(!found) return std::nullopt;
    Wide absolute = best_numerator < 0 ? -best_numerator : best_numerator;
    long long remainder = static_cast<long long>(
        absolute % static_cast<Wide>(best_denominator)
    );
    int divisor = std::gcd(best_denominator, static_cast<int>(remainder));
    best_numerator /= divisor;
    best_denominator /= divisor;
    if(best_numerator < std::numeric_limits<long long>::min()
        || best_numerator > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "minimum_mean_cycle numerator overflows long long"
        );
    }
    long long numerator = static_cast<long long>(best_numerator);
    return MinimumMeanCycleResult{
        numerator,
        best_denominator,
        static_cast<long double>(numerator)
            / static_cast<long double>(best_denominator)
    };
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPECIALIZED_ROUTE_OPTIMIZATION_MINIMUM_MEAN_CYCLE_HPP_INCLUDED
