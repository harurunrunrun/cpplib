#ifndef CPPLIB_SRC_ALGORITHM_FFT_MIN_PLUS_MAX_PLUS_CONVOLUTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_MIN_PLUS_MAX_PLUS_CONVOLUTION_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

#include "min_plus_convolution_concave.hpp"
#include "../../other/utility/smawk.hpp"

namespace max_plus_convolution_internal{
using wide=__int128_t;
using index=std::ptrdiff_t;
inline void check_size(std::size_t n,std::size_t m){
    const auto limit=static_cast<std::size_t>(std::numeric_limits<int>::max());
    if(n>limit || m>limit || n+m-1>limit) throw std::length_error("max-plus convolution input is too large");
}
} // namespace max_plus_convolution_internal

// Inputs are signed 64 bit; exact sums and results are signed 128 bit.
inline std::vector<__int128_t> max_plus_convolution_concave_arbitrary(
    const std::vector<long long>& concave,const std::vector<long long>& arbitrary
){
    using namespace max_plus_convolution_internal;
    if(concave.empty() || arbitrary.empty()) return {};
    check_size(concave.size(),arbitrary.size());
    if(!is_discrete_concave(concave)) throw std::invalid_argument("max-plus requires a concave first sequence");
    const auto [min_a,max_a]=std::minmax_element(concave.begin(),concave.end());
    const auto [min_b,max_b]=std::minmax_element(arbitrary.begin(),arbitrary.end());
    const wide penalty=wide(*max_a)-*min_a+wide(*max_b)-*min_b+1;
    const int n=static_cast<int>(concave.size()),m=static_cast<int>(arbitrary.size());
    const int length=static_cast<int>(concave.size()+arbitrary.size()-1);
    const auto minima=smawk_row_minima(length,m,[&](int row,int column){
        const int i=row-column;
        wide value;
        if(i<0) value=-wide(concave.front())-wide(i)*penalty;
        else if(i>=n) value=-wide(concave.back())+wide(i-n+1)*penalty;
        else value=-wide(concave[i]);
        return value-wide(arbitrary[column]);
    });
    std::vector<wide> result(length);
    for(int row=0;row<length;++row){
        const int column=minima[row],i=row-column;
        if(i<0 || i>=n) throw std::logic_error("invalid padded max-plus candidate");
        result[row]=wide(concave[i])+arbitrary[column];
    }
    return result;
}

inline std::vector<__int128_t> max_plus_convolution_concave_concave(
    const std::vector<long long>& left,const std::vector<long long>& right
){
    using namespace max_plus_convolution_internal;
    if(left.empty() || right.empty()) return {};
    check_size(left.size(),right.size());
    if(!is_discrete_concave(left) || !is_discrete_concave(right))
        throw std::invalid_argument("max-plus requires two concave sequences");
    std::vector<wide> result;result.reserve(left.size()+right.size()-1);
    std::size_t i=0,j=0;
    result.push_back(wide(left[0])+right[0]);
    while(i+1<left.size() || j+1<right.size()){
        const bool take_left=j+1==right.size() || (i+1<left.size() &&
            wide(left[i+1])-left[i]>=wide(right[j+1])-right[j]);
        if(take_left) ++i;else ++j;
        result.push_back(wide(left[i])+right[j]);
    }
    return result;
}

inline std::vector<__int128_t> max_plus_convolution_convex_arbitrary(
    const std::vector<long long>& convex,const std::vector<long long>& arbitrary
){
    using namespace max_plus_convolution_internal;
    if(convex.empty() || arbitrary.empty()) return {};
    check_size(convex.size(),arbitrary.size());
    if(!is_discrete_convex(convex)) throw std::invalid_argument("max-plus requires a convex first sequence");
    const index n=static_cast<index>(convex.size()),m=static_cast<index>(arbitrary.size());
    const index length=n+m-1;
    std::vector<std::optional<wide>> best(length);
    auto minimum_column=[&](index row){return std::max<index>(0,row-n+1);};
    auto maximum_column=[&](index row){return std::min<index>(m-1,row);};
    auto update=[&](index row,wide value){if(!best[row] || value<*best[row])best[row]=value;};
    std::function<void(index,index,index,index)> solve;
    solve=[&](index l,index r,index a,index b){
        if(l>r || a>b) return;
        if(maximum_column(l)>=b && a>=minimum_column(r)){
            auto evaluate=[&](index row,index column){
                const index j=b-column;
                return -wide(arbitrary[j])-wide(convex[l+row-j]);
            };
            auto report=[&](index row,wide value){update(l+row,value);};
            min_plus_convolution_concave_internal::monotone_minima<wide>(
                0,r-l,0,b-a,evaluate,report);
            return;
        }
        const index rows=r-l+1,columns=b-a+1;
        if(rows<=1024 && columns<=1024/rows){
            for(index row=l;row<=r;++row)
                for(index j=std::max(a,minimum_column(row));j<=std::min(b,maximum_column(row));++j)
                    update(row,-wide(arbitrary[j])-wide(convex[row-j]));
        }else if(rows>columns){
            const index middle=l+(r-l)/2;
            solve(l,middle,a,std::min(b,maximum_column(middle)));
            solve(middle+1,r,std::max(a,minimum_column(middle)),b);
        }else{
            const index middle=a+(b-a)/2;
            solve(l,std::min(r,middle+n-1),a,middle);
            solve(std::max(l,middle),r,middle+1,b);
        }
    };
    solve(0,length-1,0,m-1);
    std::vector<wide> result(length);
    for(index row=0;row<length;++row){
        if(!best[row])throw std::logic_error("missing max-plus candidate");
        result[row]=-*best[row];
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_MIN_PLUS_MAX_PLUS_CONVOLUTION_HPP_INCLUDED
