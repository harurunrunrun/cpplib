#ifndef CPPLIB_SRC_APPROXIMATE_ROUTING_DISTANCE_MATRIX_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_ROUTING_DISTANCE_MATRIX_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace approximate::routing {

namespace internal {

template<class DistanceMatrix>
using distance_cost_t = std::remove_cv_t<std::remove_reference_t<decltype(
    std::declval<const DistanceMatrix&>()[std::size_t{}][std::size_t{}]
)>>;

template<class DistanceMatrix>
std::size_t validate_distance_matrix(const DistanceMatrix& distance){
    const std::size_t n = distance.size();
    for(std::size_t i = 0; i < n; ++i){
        if(distance[i].size() != n){
            throw std::invalid_argument("distance matrix must be square");
        }
    }
    return n;
}

inline void validate_vertex(int vertex, std::size_t n){
    if(vertex < 0 || static_cast<std::size_t>(vertex) >= n){
        throw std::out_of_range("tour vertex is outside the distance matrix");
    }
}

inline void validate_tour_vertices(
    const std::vector<int>& tour,
    std::size_t n
){
    for(const int vertex : tour) validate_vertex(vertex, n);
}

}  // namespace internal

}  // namespace approximate::routing

#endif  // CPPLIB_SRC_APPROXIMATE_ROUTING_DISTANCE_MATRIX_HPP_INCLUDED
