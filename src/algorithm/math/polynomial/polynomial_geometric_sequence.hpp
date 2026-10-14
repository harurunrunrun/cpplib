#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_GEOMETRIC_SEQUENCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_GEOMETRIC_SEQUENCE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "polynomial_common.hpp"

namespace math{
namespace polynomial_geometric_internal{

template<int MOD>
Polynomial<MOD> batch_inverse(const Polynomial<MOD>& values){
    using mint=Modint<MOD>;
    Polynomial<MOD> prefix(values.size()+1,mint(1)),result(values.size());
    for(std::size_t i=0;i<values.size();++i){
        prefix[i+1]=prefix[i]*(values[i]==mint(0)?mint(1):values[i]);
    }
    mint suffix=prefix.back().inv();
    for(std::size_t i=values.size();i-- >0;){
        if(values[i]!=mint(0)){
            result[i]=prefix[i]*suffix;
            suffix*=values[i];
        }
    }
    return result;
}

} // namespace polynomial_geometric_internal

// Evaluates f(start*ratio^i), 0 <= i < count, by a chirp convolution.
template<int MOD>
Polynomial<MOD> polynomial_evaluate_geometric(
    const Polynomial<MOD>& polynomial, Modint<MOD> start,
    Modint<MOD> ratio, std::size_t count
){
    using mint=Modint<MOD>;
    if(count==0) return {};
    if(polynomial.empty()) return Polynomial<MOD>(count);
    if(start==mint(0)) return Polynomial<MOD>(count,polynomial[0]);
    if(ratio==mint(0)){
        Polynomial<MOD> result(count,polynomial[0]);
        mint value=0;
        for(std::size_t j=polynomial.size();j-- >0;) value=value*start+polynomial[j];
        result[0]=value;
        return result;
    }
    const std::size_t n=polynomial.size();
    Polynomial<MOD> chirp(n+count-1),inverse_chirp(std::max(n,count));
    mint power=1,current=1;
    for(auto& x:chirp){ x=current; current*=power; power*=ratio; }
    const mint inverse_ratio=ratio.inv();
    power=1; current=1;
    for(auto& x:inverse_chirp){ x=current; current*=power; power*=inverse_ratio; }
    Polynomial<MOD> scaled(n);
    power=1;
    for(std::size_t i=0;i<n;++i){
        scaled[n-1-i]=polynomial[i]*power*inverse_chirp[i];
        power*=start;
    }
    const auto product=polynomial_multiply<MOD>(scaled,chirp);
    Polynomial<MOD> result(count);
    for(std::size_t i=0;i<count;++i){
        if(n-1+i<product.size()) result[i]=product[n-1+i]*inverse_chirp[i];
    }
    return result;
}

// Interpolates at distinct points start*ratio^i in O(M(n)) time.
template<int MOD>
Polynomial<MOD> polynomial_interpolate_geometric(
    const Polynomial<MOD>& values, Modint<MOD> start, Modint<MOD> ratio
){
    using mint=Modint<MOD>;
    const std::size_t n=values.size();
    if(n<=1) return values;
    if(start==mint(0) || ratio==mint(1) || n>=static_cast<std::size_t>(MOD)){
        throw std::invalid_argument("geometric interpolation points are not distinct");
    }
    if(ratio==mint(0)){
        if(n!=2) throw std::invalid_argument("geometric interpolation points are not distinct");
        return {values[1],(values[0]-values[1])/start};
    }
    Polynomial<MOD> powers(n+1,mint(1)),products(n,mint(1));
    for(std::size_t i=1;i<=n;++i) powers[i]=powers[i-1]*ratio;
    for(std::size_t i=1;i<n;++i){
        if(powers[i]==mint(1)) throw std::invalid_argument("geometric interpolation points are not distinct");
        products[i]=products[i-1]*(mint(1)-powers[i]);
    }
    Polynomial<MOD> reversed_product(n+1);
    reversed_product[0]=mint(1);
    if(powers[n]==mint(1)){
        reversed_product[n]=mint(-1);
    }else{
        Polynomial<MOD> differences(n);
        for(std::size_t i=0;i<n;++i) differences[i]=mint(1)-powers[i+1];
        const auto inverses=polynomial_geometric_internal::batch_inverse<MOD>(differences);
        for(std::size_t k=1;k<=n;++k){
            reversed_product[k]=-reversed_product[k-1]*powers[k-1]
                *(mint(1)-powers[n-k+1])*inverses[k-1];
        }
    }
    Polynomial<MOD> derivative(n);
    mint factor=1,step=powers[n-2],inverse_ratio=ratio.inv();
    for(std::size_t i=0;i<n;++i){
        derivative[i]=factor*products[i]*products[n-1-i];
        if(i%2) derivative[i]=-derivative[i];
        factor*=step;
        step*=inverse_ratio;
    }
    const auto inverse_derivative=polynomial_geometric_internal::batch_inverse<MOD>(derivative);
    Polynomial<MOD> weights(n);
    for(std::size_t i=0;i<n;++i) weights[i]=values[i]*inverse_derivative[i];
    const auto moments=polynomial_evaluate_geometric<MOD>(weights,mint(1),ratio,n);
    auto reversed_coefficients=polynomial_multiply<MOD>(reversed_product,moments);
    reversed_coefficients.resize(n);
    std::reverse(reversed_coefficients.begin(),reversed_coefficients.end());
    mint scale=1,inverse_start=start.inv();
    for(auto& coefficient:reversed_coefficients){ coefficient*=scale; scale*=inverse_start; }
    return reversed_coefficients;
}

// Shifts samples at 0,...,n-1 to samples at start,...,start+count-1.
template<int MOD>
Polynomial<MOD> polynomial_shift_samples(
    const Polynomial<MOD>& values, Modint<MOD> start, std::size_t count
){
    using mint=Modint<MOD>;
    const std::size_t n=values.size();
    if(count==0) return {};
    if(n==0) return Polynomial<MOD>(count);
    if(n>=static_cast<std::size_t>(MOD)){
        throw std::invalid_argument("sampling points are not distinct");
    }
    Polynomial<MOD> factorial(n,mint(1)),inverse_factorial(n,mint(1));
    for(std::size_t i=1;i<n;++i) factorial[i]=factorial[i-1]*mint(i);
    inverse_factorial.back()=factorial.back().inv();
    for(std::size_t i=n-1;i>0;--i) inverse_factorial[i-1]=inverse_factorial[i]*mint(i);
    Polynomial<MOD> weighted(n),differences(n+count-1);
    for(std::size_t i=0;i<n;++i){
        weighted[i]=values[i]*inverse_factorial[i]*inverse_factorial[n-1-i];
        if((n-1-i)%2) weighted[i]=-weighted[i];
    }
    for(std::size_t i=0;i<differences.size();++i) differences[i]=start-mint(n-1)+mint(i);
    const auto inverses=polynomial_geometric_internal::batch_inverse<MOD>(differences);
    auto product=polynomial_multiply<MOD>(weighted,inverses);
    product.resize(n+count-1);
    mint window_product=1;
    for(std::size_t i=0;i<n;++i){
        if(differences[i]!=mint(0)) window_product*=differences[i];
    }
    Polynomial<MOD> result(count);
    for(std::size_t i=0;i<count;++i){
        const mint point=start+mint(i);
        result[i]=point.val()<static_cast<int>(n) ? values[point.val()]
            : product[n-1+i]*window_product;
        if(i+1<count){
            if(differences[i]!=mint(0)) window_product*=inverses[i];
            if(differences[i+n]!=mint(0)) window_product*=differences[i+n];
        }
    }
    return result;
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_GEOMETRIC_SEQUENCE_HPP_INCLUDED
