// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/number_theory/arithmetic/continued_fraction.hpp"
#include "../../src/algorithm/math/number_theory/arithmetic/farey_sequence.hpp"
#include "../../src/algorithm/math/number_theory/arithmetic/pell_equation.hpp"
#include "../../src/algorithm/math/number_theory/modular/discrete_root_mod_prime.hpp"
#include "../../src/algorithm/math/number_theory/modular/garner_algorithm.hpp"
#include "../../src/algorithm/math/number_theory/modular/modular_kth_root.hpp"
#include "../../src/algorithm/math/number_theory/modular/rational_reconstruction.hpp"

namespace{

std::optional<math::u64> brute_minimum_root(
    math::u64 value,
    math::u64 exponent,
    math::u64 prime
){
    value %= prime;
    for(math::u64 root = 0; root < prime; ++root){
        if(math::pow_mod_u64(root, exponent, prime) == value) return root;
    }
    return std::nullopt;
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    try{
        (void)math::discrete_root_mod_prime(1, 0, 3);
        return 7;
    }catch(const std::runtime_error&){
    }
    try{
        (void)math::discrete_root_mod_prime(1, 1, 9);
        return 8;
    }catch(const std::runtime_error&){
    }
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int kind;
        std::cin >> kind;
        if(kind == 0){
            math::u64 value;
            math::u64 exponent;
            math::u64 prime;
            std::cin >> value >> exponent >> prime;
            auto roots =
                math::modular_kth_roots_prime(value, exponent, prime);
            auto one =
                math::discrete_root_mod_prime(value, exponent, prime);
            const auto brute =
                brute_minimum_root(value, exponent, prime);
            if(one != brute
                || one.has_value() != !roots.empty()
                || (one && *one != roots.front())) return 2;
            std::cout << roots.size();
            for(auto root: roots) std::cout << ' ' << root;
            std::cout << '\n';
        }else if(kind == 1){
            int count;
            math::u64 output_modulus;
            std::cin >> count >> output_modulus;
            std::vector<math::u64> remainders(
                static_cast<std::size_t>(count)
            );
            std::vector<math::u64> moduli(
                static_cast<std::size_t>(count)
            );
            for(auto& value: remainders) std::cin >> value;
            for(auto& value: moduli) std::cin >> value;
            std::cout << math::garner_algorithm(
                remainders, moduli, output_modulus
            ) << '\n';
        }else if(kind == 2){
            long long numerator;
            long long denominator;
            std::cin >> numerator >> denominator;
            auto coefficients =
                math::continued_fraction_expansion(numerator, denominator);
            auto convergents =
                math::continued_fraction_convergents(coefficients);
            if(convergents.empty()) return 3;
            const auto [final_numerator, final_denominator] =
                convergents.back();
            if(final_numerator * denominator
                != final_denominator * numerator) return 4;
            std::cout << coefficients.size();
            for(long long value: coefficients) std::cout << ' ' << value;
            std::cout << '\n';
        }else if(kind == 3){
            int order;
            std::cin >> order;
            auto sequence = math::farey_sequence(order);
            std::cout << sequence.size();
            for(auto [numerator, denominator]: sequence){
                std::cout << ' ' << numerator << '/' << denominator;
            }
            std::cout << '\n';
        }else if(kind == 4){
            long long residue;
            long long modulus;
            long long numerator_bound;
            long long denominator_bound;
            std::cin >> residue >> modulus
                     >> numerator_bound >> denominator_bound;
            auto result = math::rational_reconstruction(
                residue, modulus, numerator_bound, denominator_bound
            );
            if(result){
                std::cout << result->first << ' ' << result->second << '\n';
            }else{
                std::cout << "none\n";
            }
        }else{
            long long nonsquare;
            std::cin >> nonsquare;
            auto result =
                math::minimum_pell_equation_solution(nonsquare);
            if(result){
                std::cout << result->x << ' ' << result->y << '\n';
            }else{
                std::cout << "none\n";
            }
        }
    }
}
