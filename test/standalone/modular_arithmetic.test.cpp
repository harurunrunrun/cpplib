// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/math/modular_arithmetic.hpp"

void test_chinese_remainder(){
    auto result = math::chinese_remainder(2, 3, 3, 5);
    assert(result);
    assert(result->remainder == 8);
    assert(result->modulus == 15);

    result = math::chinese_remainder(1, 4, 3, 6);
    assert(result);
    assert(result->remainder == 9);
    assert(result->modulus == 12);
    assert(!math::chinese_remainder(0, 2, 1, 4));

    result = math::chinese_remainder(
        std::vector<long long>{2, 3, 2},
        std::vector<long long>{3, 5, 7}
    );
    assert(result && result->remainder == 23 && result->modulus == 105);
    result = math::chinese_remainder(std::vector<long long>{}, std::vector<long long>{});
    assert(result && result->remainder == 0 && result->modulus == 1);
}

void test_primitive_root(){
    assert(math::primitive_root(2) == 1);
    assert(math::primitive_root(3) == 2);
    assert(math::primitive_root(17) == 3);
    assert(math::primitive_root(998244353) == 3);

    bool thrown = false;
    try{
        (void)math::primitive_root(15);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int query_count;
    if(std::cin >> query_count){
        while(query_count--){
            std::string type;
            std::cin >> type;
            if(type == "CRT"){
                int count;
                std::cin >> count;
                std::vector<long long> remainders(static_cast<std::size_t>(count));
                std::vector<long long> moduli(static_cast<std::size_t>(count));
                for(int i = 0; i < count; i++){
                    std::cin >> remainders[static_cast<std::size_t>(i)] >>
                        moduli[static_cast<std::size_t>(i)];
                }
                const auto result = math::chinese_remainder(remainders, moduli);
                if(result) std::cout << result->remainder << ' ' << result->modulus << '\n';
                else std::cout << "NONE\n";
            }else if(type == "ROOT"){
                unsigned long long prime;
                std::cin >> prime;
                std::cout << math::primitive_root(prime) << '\n';
            }
        }
        return 0;
    }
    test_chinese_remainder();
    test_primitive_root();
}
