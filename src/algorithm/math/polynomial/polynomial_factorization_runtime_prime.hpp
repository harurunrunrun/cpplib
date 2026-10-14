#ifndef CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_FACTORIZATION_RUNTIME_PRIME_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_FACTORIZATION_RUNTIME_PRIME_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

namespace math{

struct RuntimePrimePolynomialFactor{
    std::vector<std::uint32_t> polynomial;
    std::size_t multiplicity=0;
};

struct RuntimePrimePolynomialFactorization{
    std::uint32_t leading_coefficient=0;
    std::vector<RuntimePrimePolynomialFactor> factors;
};

// Cantor--Zassenhaus over a prime selected at run time. Quadratic polynomial
// arithmetic is intended for small degrees (including the degree-100 task).
class RuntimePrimePolynomialFactorizer{
    using poly=std::vector<std::uint32_t>;
    std::uint32_t prime_;
    std::uint64_t state_=0x9e3779b97f4a7c15ULL;

    static void trim(poly& f){while(!f.empty() && f.back()==0) f.pop_back();}
    static bool one(const poly& f){return f.size()==1 && f[0]==1;}
    std::uint32_t mul(std::uint32_t a,std::uint32_t b) const{
        return static_cast<std::uint64_t>(a)*b%prime_;
    }
    std::uint32_t sub(std::uint32_t a,std::uint32_t b) const{
        return a>=b?a-b:a+prime_-b;
    }
    std::uint32_t pow(std::uint32_t a,std::uint64_t exponent) const{
        std::uint32_t result=1;
        while(exponent){if(exponent%2) result=mul(result,a);a=mul(a,a);exponent/=2;}
        return result;
    }
    poly monic(poly f) const{
        trim(f);
        if(f.empty()) return f;
        const auto inverse=pow(f.back(),prime_-2);
        for(auto& a:f) a=mul(a,inverse);
        return f;
    }
    std::pair<poly,poly> divide(poly f,const poly& g) const{
        trim(f);
        if(g.empty()) throw std::invalid_argument("polynomial division by zero");
        if(f.size()<g.size()) return {{},std::move(f)};
        poly quotient(f.size()-g.size()+1);
        const auto inverse=pow(g.back(),prime_-2);
        for(std::size_t k=quotient.size();k-- >0;){
            const auto coefficient=mul(f[k+g.size()-1],inverse);
            quotient[k]=coefficient;
            for(std::size_t j=0;j<g.size();++j) f[k+j]=sub(f[k+j],mul(coefficient,g[j]));
        }
        trim(f);trim(quotient);
        return {std::move(quotient),std::move(f)};
    }
    poly exact(const poly& f,const poly& g) const{
        auto division=divide(f,g);
        if(!division.second.empty()) throw std::logic_error("inexact polynomial factorization division");
        return std::move(division.first);
    }
    poly gcd(poly f,poly g) const{
        while(!g.empty()){
            auto remainder=divide(f,g).second;
            f=std::move(g);g=std::move(remainder);
        }
        return monic(std::move(f));
    }
    poly subtract(poly f,const poly& g) const{
        f.resize(std::max(f.size(),g.size()));
        for(std::size_t i=0;i<g.size();++i) f[i]=sub(f[i],g[i]);
        trim(f);return f;
    }
    poly multiply_mod(const poly& f,const poly& g,const poly& modulus) const{
        if(f.empty() || g.empty()) return {};
        poly product(f.size()+g.size()-1);
        for(std::size_t i=0;i<f.size();++i){
            for(std::size_t j=0;j<g.size();++j){
                product[i+j]=(product[i+j]+static_cast<std::uint64_t>(f[i])*g[j])%prime_;
            }
        }
        return divide(std::move(product),modulus).second;
    }
    poly power_mod(poly f,std::uint64_t exponent,const poly& modulus) const{
        f=divide(std::move(f),modulus).second;
        poly result{1};
        while(exponent){
            if(exponent%2) result=multiply_mod(result,f,modulus);
            exponent/=2;
            if(exponent) f=multiply_mod(f,f,modulus);
        }
        return result;
    }
    void square_free(poly f,std::size_t scale,
        std::vector<std::pair<poly,std::size_t>>& result) const{
        if(f.size()<=1) return;
        poly derivative(f.size()-1);
        for(std::size_t i=1;i<f.size();++i) derivative[i-1]=mul(f[i],i%prime_);
        trim(derivative);
        poly repeated=gcd(f,derivative),remaining=exact(f,repeated);
        for(std::size_t exponent=1;!one(remaining);++exponent){
            auto shared=gcd(remaining,repeated);
            auto component=exact(remaining,shared);
            if(!one(component)) result.emplace_back(std::move(component),scale*exponent);
            remaining=std::move(shared);
            repeated=exact(repeated,remaining);
        }
        if(!one(repeated)){
            poly root((repeated.size()-1)/prime_+1);
            for(std::size_t i=0;i<root.size();++i) root[i]=repeated[i*prime_];
            square_free(std::move(root),scale*prime_,result);
        }
    }
    std::uint32_t random(){
        state_^=state_>>12;state_^=state_<<25;state_^=state_>>27;
        return (state_*0x2545f4914f6cdd1dULL)%prime_;
    }
    void equal_degree(poly f,std::size_t degree,std::size_t multiplicity,
        std::vector<RuntimePrimePolynomialFactor>& result){
        if(f.size()-1==degree){result.push_back({std::move(f),multiplicity});return;}
        while(true){
            poly candidate(f.size()-1);
            for(auto& x:candidate) x=random();
            poly splitter;
            if(prime_==2){
                for(std::size_t i=0;i<degree;++i){
                    splitter=subtract(std::move(splitter),candidate);
                    candidate=power_mod(std::move(candidate),2,f);
                }
            }else{
                splitter={1};
                for(std::size_t i=0;i<degree;++i){
                    splitter=multiply_mod(splitter,power_mod(candidate,(prime_-1)/2,f),f);
                    if(i+1<degree) candidate=power_mod(std::move(candidate),prime_,f);
                }
                splitter=subtract(std::move(splitter),{1});
            }
            auto divisor=gcd(f,splitter);
            if(one(divisor) || divisor==f) continue;
            auto quotient=exact(f,divisor);
            equal_degree(std::move(divisor),degree,multiplicity,result);
            equal_degree(std::move(quotient),degree,multiplicity,result);
            return;
        }
    }

public:
    explicit RuntimePrimePolynomialFactorizer(std::uint32_t prime):prime_(prime){
        if(prime<2 || prime>2'147'483'647U) throw std::invalid_argument("invalid prime modulus");
        for(std::uint32_t d=2;static_cast<std::uint64_t>(d)*d<=prime;++d){
            if(prime%d==0) throw std::invalid_argument("nonprime polynomial modulus");
        }
    }

    RuntimePrimePolynomialFactorization operator()(poly f){
        for(auto& x:f) x%=prime_;
        trim(f);
        if(f.empty()) throw std::invalid_argument("zero polynomial has no finite factorization");
        RuntimePrimePolynomialFactorization result{f.back(),{}};
        f=monic(std::move(f));
        std::vector<std::pair<poly,std::size_t>> components;
        square_free(std::move(f),1,components);
        for(auto& [component,multiplicity]:components){
            poly frobenius{0,1};
            std::size_t degree=1;
            while(component.size()>1 && degree<=(component.size()-1)/2){
                frobenius=power_mod(std::move(frobenius),prime_,component);
                auto factor=gcd(component,subtract(frobenius,{0,1}));
                if(!one(factor)){
                    component=exact(component,factor);
                    equal_degree(std::move(factor),degree,multiplicity,result.factors);
                    if(one(component)) break;
                    frobenius=divide(std::move(frobenius),component).second;
                }
                ++degree;
            }
            if(!one(component)) result.factors.push_back({std::move(component),multiplicity});
        }
        std::sort(result.factors.begin(),result.factors.end(),[](const auto& a,const auto& b){
            return a.polynomial<b.polynomial;
        });
        return result;
    }
};

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_POLYNOMIAL_POLYNOMIAL_FACTORIZATION_RUNTIME_PRIME_HPP_INCLUDED
