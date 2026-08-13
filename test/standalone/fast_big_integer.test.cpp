// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/math/integer/fast_big_integer.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t query_count;
    if(!(std::cin >> query_count)){
        const BigInteger a("123456789012345678901234567890");
        const BigInteger b("98765432109876543210");
        const auto [quotient, remainder] = BigInteger::divmod(a, b);
        assert(a == quotient * b + remainder);
        assert(abs(remainder) < abs(b));
        assert((BigInteger(-3) >> 1) == -2);
        return 0;
    }
    while(query_count-- > 0){
        std::string operation, left_text;
        std::cin >> operation >> left_text;
        const BigInteger left(left_text);
        if(operation == "ECHO"){
            std::cout << left << '\n';
            continue;
        }
        std::string right_text;
        std::cin >> right_text;
        const BigInteger right(right_text);
        if(operation == "ADD") std::cout << left + right << '\n';
        else if(operation == "SUB") std::cout << left - right << '\n';
        else if(operation == "MUL") std::cout << left * right << '\n';
        else if(operation == "DIV") std::cout << left / right << '\n';
        else if(operation == "MOD") std::cout << left % right << '\n';
        else if(operation == "CMP"){
            std::cout << (left < right ? -1 : left > right) << '\n';
        }else if(operation == "SHL"){
            std::cout << (left << right.checked_to<std::size_t>()) << '\n';
        }else if(operation == "SHR"){
            std::cout << (left >> right.checked_to<std::size_t>()) << '\n';
        }else{
            throw std::runtime_error("unknown BigInteger operation");
        }
    }
}
