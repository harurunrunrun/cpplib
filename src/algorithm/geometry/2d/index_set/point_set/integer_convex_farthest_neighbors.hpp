#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INDEX_SET_POINT_SET_INTEGER_CONVEX_FARTHEST_NEIGHBORS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INDEX_SET_POINT_SET_INTEGER_CONVEX_FARTHEST_NEIGHBORS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

inline std::vector<int> integer_convex_farthest_neighbors(
    const std::vector<std::pair<long long,long long>>& points
){
    const int n=static_cast<int>(points.size());
    for(const auto& [x,y]:points){
        constexpr long long limit=1LL<<62;
        if(x < -limit || x >= limit || y < -limit || y >= limit)
            throw std::out_of_range("integer_convex_farthest_neighbors: coordinate exceeds supported range");
    }
    const auto distance=[&](int a,int b)->__int128{
        const __int128 dx=static_cast<__int128>(points[a].first)-points[b].first;
        const __int128 dy=static_cast<__int128>(points[a].second)-points[b].second;
        return dx*dx+dy*dy;
    };
    std::vector<int> answer(n),temporary(n);
    std::iota(answer.begin(),answer.end(),0);
    const auto smawk=[&](auto&& self,const std::vector<int>& rows,const std::vector<int>& columns)->void{
        if(rows.empty())return;
        std::vector<int> reduced;
        reduced.reserve(std::min(rows.size(),columns.size()));
        for(int column:columns){
            while(!reduced.empty() && distance(rows[reduced.size()-1],column)>distance(rows[reduced.size()-1],reduced.back()))
                reduced.pop_back();
            if(reduced.size()<rows.size())reduced.push_back(column);
        }
        std::vector<int> odd;
        for(std::size_t i=1;i<rows.size();i+=2)odd.push_back(rows[i]);
        self(self,odd,reduced);
        std::size_t left=0;
        for(std::size_t i=0;i<rows.size();i+=2){
            if(i)while(reduced[left]!=temporary[rows[i-1]])++left;
            std::size_t right=reduced.size()-1;
            if(i+1<rows.size()){
                right=left;while(reduced[right]!=temporary[rows[i+1]])++right;
            }
            int best=reduced[left];
            for(std::size_t j=left+1;j<=right;++j)
                if(distance(rows[i],reduced[j])>distance(rows[i],best))best=reduced[j];
            temporary[rows[i]]=best;
        }
    };
    const auto solve=[&](auto&& self,int left,int right)->void{
        if(right-left<=24){
            for(int i=left;i<right;++i)for(int j=left;j<right;++j)
                if(distance(i,j)>distance(i,answer[i]))answer[i]=j;
            return;
        }
        const int middle=(left+right)/2;
        std::vector<int> first(middle-left),second(right-middle);
        std::iota(first.begin(),first.end(),left);std::iota(second.begin(),second.end(),middle);
        smawk(smawk,first,second);
        for(int i:first)if(distance(i,temporary[i])>distance(i,answer[i]))answer[i]=temporary[i];
        smawk(smawk,second,first);
        for(int i:second)if(distance(i,temporary[i])>distance(i,answer[i]))answer[i]=temporary[i];
        self(self,left,middle);self(self,middle,right);
    };
    solve(solve,0,n);
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_INDEX_SET_POINT_SET_INTEGER_CONVEX_FARTHEST_NEIGHBORS_HPP_INCLUDED
