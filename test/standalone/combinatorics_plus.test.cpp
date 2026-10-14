// competitive-verifier: STANDALONE

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/combinatorics/binomial_mod_composite.hpp"
#include "../../src/algorithm/math/combinatorics/catalan_numbers.hpp"
#include "../../src/algorithm/math/combinatorics/eulerian_numbers.hpp"
#include "../../src/algorithm/math/combinatorics/generalized_lucas_theorem.hpp"
#include "../../src/algorithm/math/combinatorics/lucas_theorem.hpp"

namespace{

unsigned long long pascal_lucas(
    unsigned long long n,
    unsigned long long k,
    unsigned long long prime
){
    if(k > n) return 0;
    unsigned long long answer = 1;
    while(n != 0 || k != 0){
        const auto n_digit = n % prime;
        const auto k_digit = k % prime;
        if(k_digit > n_digit) return 0;
        std::vector<unsigned long long> row(
            static_cast<std::size_t>(k_digit + 1), 0
        );
        row[0] = 1;
        for(unsigned long long size = 1; size <= n_digit; ++size){
            const auto upper = std::min(size, k_digit);
            for(unsigned long long index = upper; index != 0; --index){
                row[static_cast<std::size_t>(index)] =
                    (row[static_cast<std::size_t>(index)]
                        + row[static_cast<std::size_t>(index - 1)]) % prime;
            }
        }
        answer = static_cast<unsigned long long>(
            static_cast<__uint128_t>(answer)
            * row[static_cast<std::size_t>(k_digit)] % prime
        );
        n /= prime;
        k /= prime;
    }
    return answer;
}

} // namespace

#include <cassert>
#include "../../src/structure/modint/modint.hpp"
#include "../../src/algorithm/math/combinatorics/combinatorics.hpp"
#include "../../src/algorithm/math/combinatorics/binomial_prefix_sum.hpp"
#include "../../src/algorithm/math/combinatorics/stern_brocot_tree.hpp"
#include "../../src/algorithm/math/number_theory/prime/prime_sieve.hpp"
#include "../../src/algorithm/math/number_theory/prime/prime_counting.hpp"
#include "../../src/algorithm/math/sequence/linear_recurrence.hpp"
#include "../../src/algorithm/math/polynomial/polynomial_interpolation.hpp"
#include "../../src/algorithm/math/polynomial/polynomial_discrete_sum.hpp"
#include "../../src/algorithm/math/polynomial/polynomial_resultant.hpp"
#include "../../src/algorithm/math/optimization/linear_programming.hpp"
#include "../../src/algorithm/fft/formal_power_series/bivariate_formal_power_series.hpp"
#include "../../src/algorithm/math/linear_algebra/gf2/inverse_matrix_mod_2.hpp"
#include "../../src/algorithm/math/linear_algebra/gf2/matrix_product_mod_2.hpp"

void dynamic_capacity_math_test(){
 using mint=Modint998244353;
Combinatorics<mint> c(1000); assert(c.combination(10,3)==mint(120));
 BinomialPrefixSum<mint> b(10); assert(b.prefix_sum(4,2)==mint(11));
 math::PrimeSieve<> ps(10000); assert(ps.prime_count()==1229); ps.build(10);assert(ps.prime_count()==4);
 math::PrimeCounting<> pc(10000); assert(pc.count_primes(10000)==1229);pc.build(0);assert(pc.count_primes(0)==0);
 math::SternBrocotTree<> sb; (void)sb;
 LinearRecurrence<mint> lr({0,1},{1,1}); assert(lr.kth_term(20)==mint(6765));
 auto bm=berlekamp_massey(std::vector<mint>{0,1,1,2,3,5,8,13}); assert(bm.kth_term(20)==mint(6765));
 math::PolynomialInterpolation<mint> ip;assert(ip.evaluate_consecutive({0,1,4,9},4,mint(5))==mint(25));
 auto coeff=ip.interpolate_coefficients({0,1,2},{0,1,4},3);assert((coeff==std::vector<mint>{0,0,1}));
 math::PolynomialDiscreteSum<mint> ds;assert(ds.power_sum(5,2)==mint(30));assert(ds.polynomial_prefix_sum({0,0,1},3,5LL)==mint(30));
 assert(polynomial_resultant(std::vector<mint>{-1,1},std::vector<mint>{-2,1})==mint(-1));
 assert(polynomial_resultant(std::vector<mint>{3},std::vector<mint>{1,0,1})==mint(9));
 assert(polynomial_discriminant(std::vector<mint>{-1,0,1})==mint(4));
 LinearProgramming<> lp(2);lp.set_objective({1,1});lp.add_less_equal({1,0},2);lp.add_less_equal({0,1},3);assert(lp.maximize().value==5);
 BivariateFormalPowerSeries<mint> f(2,2);f(1,0)=1;f(0,1)=1;auto e=f.exponential();assert(e(1,1)==mint(1));assert(e.logarithm()==f);
 std::vector<std::vector<bool>> g{{1,1},{1,0}};auto inv=inverse_matrix_mod_2(g);assert(inv);auto prod=matrix_product_mod_2(g,*inv);assert(prod[0][0]&&prod[1][1]&&!prod[0][1]&&!prod[1][0]);
 auto sol=gf2_solve_linear_system(g,std::vector<int>{1,0});assert(sol.consistent&&gf2_rank(g)==2);
}

int main(){
    dynamic_capacity_math_test();
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    try{
        (void)math::lucas_binomial_mod_prime(5, 2, 4);
        return 6;
    }catch(const std::runtime_error&){
    }
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int kind;
        std::cin >> kind;
        if(kind == 0){
            unsigned long long n;
            unsigned long long k;
            unsigned long long prime;
            int exponent;
            unsigned long long modulus;
            std::cin >> n >> k >> prime >> exponent >> modulus;
            auto value = math::generalized_lucas_binomial_mod_prime_power(
                n, k, prime, exponent
            );
            const auto lucas =
                math::lucas_binomial_mod_prime(n, k, prime);
            if(lucas != pascal_lucas(n, k, prime)
                || (exponent == 1 && value != lucas)){
                return 2;
            }
            std::cout << value << ' '
                      << math::binomial_mod_composite(n, k, modulus)
                      << '\n';
        }else if(kind == 1){
            std::size_t maximum_index;
            std::cin >> maximum_index;
            auto values = math::catalan_numbers<998244353>(maximum_index);
            for(std::size_t index = 0; index < values.size(); ++index){
                if(index != 0) std::cout << ' ';
                std::cout << values[index].val();
            }
            std::cout << '\n';
        }else{
            std::size_t order;
            std::cin >> order;
            auto values = math::eulerian_numbers<998244353>(order);
            for(std::size_t index = 0; index < values.size(); ++index){
                if(index != 0) std::cout << ' ';
                std::cout << values[index].val();
            }
            std::cout << '\n';
        }
    }
}
