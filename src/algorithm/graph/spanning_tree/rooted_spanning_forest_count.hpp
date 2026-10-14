#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_SPANNING_TREE_ROOTED_SPANNING_FOREST_COUNT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_SPANNING_TREE_ROOTED_SPANNING_FOREST_COUNT_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include "../../math/linear_algebra/elimination/determinant_mod_composite.hpp"
#include "../../math/linear_algebra/elimination/sparse_determinant.hpp"

struct RootedForestEdge{int first,second;long long weight;};

inline long long count_rooted_spanning_forests_mod(
    const std::vector<long long>& root_weights,const std::vector<RootedForestEdge>& edges,long long modulus
){
    if(modulus<1 || modulus>2147483647LL)throw std::invalid_argument("count_rooted_spanning_forests_mod: invalid modulus");
    const int n=static_cast<int>(root_weights.size());
    std::vector<std::vector<long long>> matrix(n,std::vector<long long>(n));
    for(int i=0;i<n;++i)matrix[i][i]=root_weights[i]%modulus;
    for(const auto& e:edges){
        if(e.first<0 || e.first>=n || e.second<0 || e.second>=n)throw std::out_of_range("count_rooted_spanning_forests_mod: invalid edge");
        if(e.first==e.second)continue;
        const long long weight=e.weight%modulus;
        for(int v:{e.first,e.second})matrix[v][v]=(matrix[v][v]+weight)%modulus;
        matrix[e.first][e.second]=(matrix[e.first][e.second]-weight)%modulus;
        matrix[e.second][e.first]=(matrix[e.second][e.first]-weight)%modulus;
    }
    return determinant_mod_composite(std::move(matrix),modulus);
}

template<int MOD=998244353,int MAX_SIZE=-1>
Modint<MOD> count_rooted_spanning_forests_prime(
    const std::vector<long long>& root_weights,const std::vector<RootedForestEdge>& edges,std::uint64_t seed=std::random_device{}()
){
    using Mint=Modint<MOD>;
    const int n=static_cast<int>(root_weights.size());
    std::vector<Mint> diagonal(root_weights.begin(),root_weights.end());
    std::vector<SparseMatrixEntry<Mint>> entries;
    for(const auto& e:edges){
        if(e.first<0 || e.first>=n || e.second<0 || e.second>=n)throw std::out_of_range("count_rooted_spanning_forests_prime: invalid edge");
        if(e.first==e.second)continue;
        const Mint weight(e.weight);diagonal[e.first]+=weight;diagonal[e.second]+=weight;
        entries.push_back({e.first,e.second,-weight});entries.push_back({e.second,e.first,-weight});
    }
    for(int i=0;i<n;++i)entries.push_back({i,i,diagonal[i]});
    return sparse_determinant<Mint,MAX_SIZE>(n,entries,seed);
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_SPANNING_TREE_ROOTED_SPANNING_FOREST_COUNT_HPP_INCLUDED
