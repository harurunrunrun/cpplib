#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONSTRAINTS_COLORING_CHROMATIC_POLYNOMIAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONSTRAINTS_COLORING_CHROMATIC_POLYNOMIAL_HPP_INCLUDED

#include <bit>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../../../math/polynomial/set_power_series_composition.hpp"

template<class T>
std::vector<T> chromatic_polynomial(int vertex_count, const std::vector<std::pair<int,int>>& edges){
    if(vertex_count < 0 || vertex_count > 24)
        throw std::invalid_argument("chromatic_polynomial: vertex count outside [0,24]");
    std::vector<unsigned int> adjacency(vertex_count);
    bool has_loop = false;
    for(const auto& [u,v]:edges){
        if(u<0 || u>=vertex_count || v<0 || v>=vertex_count)
            throw std::out_of_range("chromatic_polynomial: invalid vertex");
        has_loop |= u==v;
        adjacency[u] |= 1U<<v; adjacency[v] |= 1U<<u;
    }
    if(has_loop) return std::vector<T>(vertex_count+1);
    const std::size_t count = std::size_t(1)<<vertex_count;
    std::vector<T> independent(count), weights(count);
    independent[0]=T(1);
    for(unsigned int mask=1; mask<count; ++mask){
        const int vertex=std::countr_zero(mask);
        const unsigned int rest=mask&(mask-1);
        if((adjacency[vertex]&rest)==0) independent[mask]=independent[rest];
    }
    independent[0]=T{};
    weights.back()=T(1);
    const auto ordered_partitions=set_power_series_power_projection(independent,weights,vertex_count+1);
    std::vector<T> answer(vertex_count+1), falling{T(1)};
    T inverse_factorial(1);
    for(int k=0; k<=vertex_count; ++k){
        if(k!=0) inverse_factorial/=T(k);
        const T scale=ordered_partitions[k]*inverse_factorial;
        for(int degree=0; degree<=k; ++degree) answer[degree]+=scale*falling[degree];
        falling.push_back(T{});
        for(int degree=k+1; degree>=1; --degree)
            falling[degree]=falling[degree-1]-T(k)*falling[degree];
        falling[0]*=-T(k);
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONSTRAINTS_COLORING_CHROMATIC_POLYNOMIAL_HPP_INCLUDED
