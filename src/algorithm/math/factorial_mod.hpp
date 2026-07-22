#ifndef CPPLIB_SRC_ALGORITHM_MATH_FACTORIAL_MOD_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_FACTORIAL_MOD_HPP_INCLUDED

#include <stdexcept>
#include <vector>

namespace math{

struct FactorialModPrimeResult{
    long long value;
    long long exponent;
};

class FactorialModPrime{
    private:
        int p;
        std::vector<int> fact;

        static int validate_prime(int prime){
            if(prime < 2)[[unlikely]]{
                throw std::runtime_error("library assertion fault: invalid argument (FactorialModPrime).");
            }
            return prime;
        }

    public:
        explicit FactorialModPrime(int prime) : p(validate_prime(prime)), fact(p){
            fact[0] = 1;
            for(int i = 1; i < p; i++){
                fact[i] = (long long)fact[i - 1] * i % p;
            }
        }

        int mod()const{
            return p;
        }

        FactorialModPrimeResult factorial(long long n)const{
            if(n < 0)[[unlikely]]{
                throw std::runtime_error("library assertion fault: invalid argument (factorial).");
            }
            long long value = 1;
            long long exponent = 0;
            while(n > 0){
                const long long q = n / p;
                exponent += q;
                if(q & 1){
                    value = value * fact[p - 1] % p;
                }
                value = value * fact[n % p] % p;
                n = q;
            }
            return {value, exponent};
        }
};

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_FACTORIAL_MOD_HPP_INCLUDED
