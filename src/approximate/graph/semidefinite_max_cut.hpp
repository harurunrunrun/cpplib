#ifndef CPPLIB_SRC_APPROXIMATE_GRAPH_SEMIDEFINITE_MAX_CUT_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_GRAPH_SEMIDEFINITE_MAX_CUT_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "max_cut.hpp"

namespace approximate::graph {

template<class Real, class DirectionRange>
[[nodiscard]] MaxCutResult semidefinite_rounding(
    const std::vector<std::vector<Real>>& unit_vectors,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    const DirectionRange& directions
){
    static_assert(std::is_floating_point_v<Real>);
    if(unit_vectors.empty()) return MaxCutResult{{}, cut_edge_count(0, edges, std::vector<unsigned char>{})};
    const std::size_t dimension = unit_vectors.front().size();
    if(dimension == 0)[[unlikely]]{
        throw std::invalid_argument("SDP vectors must have positive dimension");
    }
    for(const auto& vector : unit_vectors){
        if(vector.size() != dimension)[[unlikely]]{
            throw std::invalid_argument("SDP vector dimensions differ");
        }
        long double norm = 0.0L;
        for(const Real value : vector){
            if(!std::isfinite(value))[[unlikely]]{
                throw std::invalid_argument("SDP vectors must be finite");
            }
            norm += static_cast<long double>(value) * value;
        }
        if(!(norm > 0.0L))[[unlikely]]{
            throw std::invalid_argument("SDP vectors must be nonzero");
        }
    }
    MaxCutResult best;
    bool have_direction = false;
    for(const auto& direction : directions){
        if(direction.size() != dimension)[[unlikely]]{
            throw std::invalid_argument("rounding direction dimension differs");
        }
        MaxCutResult candidate;
        candidate.side.resize(unit_vectors.size());
        for(std::size_t vertex = 0; vertex < unit_vectors.size(); ++vertex){
            long double product = 0.0L;
            for(std::size_t coordinate = 0;
                coordinate < dimension;
                ++coordinate){
                if(!std::isfinite(static_cast<long double>(
                    direction[coordinate]
                )))[[unlikely]]{
                    throw std::invalid_argument(
                        "rounding directions must be finite"
                    );
                }
                product += static_cast<long double>(
                    unit_vectors[vertex][coordinate]
                ) * static_cast<long double>(direction[coordinate]);
            }
            candidate.side[vertex] =
                static_cast<unsigned char>(product >= 0.0L);
        }
        candidate.cut_edges = cut_edge_count(
            unit_vectors.size(), edges, candidate.side
        );
        if(!have_direction || best.cut_edges < candidate.cut_edges){
            best = std::move(candidate);
        }
        have_direction = true;
    }
    if(!have_direction)[[unlikely]]{
        throw std::invalid_argument(
            "semidefinite_rounding requires a non-empty direction range"
        );
    }
    return best;
}

template<class Real, class URBG>
[[nodiscard]] MaxCutResult goemans_williamson_max_cut(
    const std::vector<std::vector<Real>>& unit_vectors,
    const std::vector<std::pair<std::size_t, std::size_t>>& edges,
    std::size_t rounding_trials,
    URBG& random
){
    static_assert(std::is_floating_point_v<Real>);
    if(unit_vectors.empty()) return MaxCutResult{{}, cut_edge_count(0, edges, std::vector<unsigned char>{})};
    if(rounding_trials == 0)[[unlikely]]{
        throw std::invalid_argument(
            "Goemans-Williamson rounding requires positive trials"
        );
    }
    const std::size_t dimension = unit_vectors.front().size();
    std::normal_distribution<Real> normal(Real{}, Real{1});
    std::vector<std::vector<Real>> directions(
        rounding_trials, std::vector<Real>(dimension)
    );
    for(auto& direction : directions){
        for(Real& value : direction) value = normal(random);
    }
    return semidefinite_rounding(unit_vectors, edges, directions);
}

}  // namespace approximate::graph

#endif  // CPPLIB_SRC_APPROXIMATE_GRAPH_SEMIDEFINITE_MAX_CUT_HPP_INCLUDED
