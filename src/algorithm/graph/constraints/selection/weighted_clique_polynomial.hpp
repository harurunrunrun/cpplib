#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONSTRAINTS_SELECTION_WEIGHTED_CLIQUE_POLYNOMIAL_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONSTRAINTS_SELECTION_WEIGHTED_CLIQUE_POLYNOMIAL_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <cstdint>
#include <functional>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
std::vector<T> weighted_clique_polynomial(const std::vector<T>& weights,
    const std::vector<std::pair<int,int>>& edges){
    const int n=static_cast<int>(weights.size());
    std::vector<std::vector<int>> adjacency(n);
    for(const auto& [u,v]:edges){
        if(u<0 || u>=n || v<0 || v>=n || u==v)
            throw std::invalid_argument("weighted_clique_polynomial: invalid edge");
        adjacency[u].push_back(v); adjacency[v].push_back(u);
    }
    using Entry=std::pair<int,int>;
    std::priority_queue<Entry,std::vector<Entry>,std::greater<Entry>> heap;
    std::vector<int> degree(n),rank(n),order;
    for(int v=0;v<n;++v){
        auto& row=adjacency[v]; std::sort(row.begin(),row.end());
        row.erase(std::unique(row.begin(),row.end()),row.end());
        degree[v]=static_cast<int>(row.size()); heap.push({degree[v],v});
    }
    std::vector<unsigned char> removed(n);
    while(!heap.empty()){
        const auto [d,v]=heap.top(); heap.pop();
        if(removed[v] || d!=degree[v]) continue;
        removed[v]=1; rank[v]=static_cast<int>(order.size()); order.push_back(v);
        for(int u:adjacency[v]) if(!removed[u]) heap.push({--degree[u],u});
    }
    std::vector<T> answer(n+1); answer[0]=T(1);
    for(int v:order){
        std::vector<int> next;
        for(int u:adjacency[v]) if(rank[u]>rank[v]) next.push_back(u);
        const int d=static_cast<int>(next.size());
        if(d>63) throw std::length_error("weighted_clique_polynomial: degeneracy exceeds 63");
        std::vector<std::uint64_t> local(d);
        for(int i=0;i<d;++i) for(int j=i+1;j<d;++j)
            if(std::binary_search(adjacency[next[i]].begin(),adjacency[next[i]].end(),next[j]))
                local[i]|=std::uint64_t(1)<<j;
        const auto visit=[&](auto&& self,std::uint64_t candidates,int size,T product)->void{
            answer[size]+=product;
            while(candidates){
                const int bit=std::countr_zero(candidates); candidates&=candidates-1;
                self(self,candidates&local[bit],size+1,product*weights[next[bit]]);
            }
        };
        visit(visit,(std::uint64_t(1)<<d)-1,1,weights[v]);
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONSTRAINTS_SELECTION_WEIGHTED_CLIQUE_POLYNOMIAL_HPP_INCLUDED
