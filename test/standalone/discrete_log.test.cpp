// competitive-verifier: STANDALONE

#include <cassert>
#include <stdexcept>

#include "../../src/algorithm/math/basic_number_theory.hpp"
#include "../../src/algorithm/math/discrete_log.hpp"

void test_coprime(){
    assert(math::discrete_log(2, 1, 13) == 0);
    assert(math::discrete_log(2, 8, 13) == 3);
    assert(math::discrete_log(5, 8, 23) == 6);
    assert(math::discrete_log(2, 3, 7) == -1);
    assert(math::discrete_log_coprime(2, 8, 13) == 3);
}

void test_non_coprime(){
    assert(math::discrete_log(2, 8, 12) == 3);
    assert(math::discrete_log(4, 2, 14) == 2);
    assert(math::discrete_log(2, 3, 8) == -1);
    assert(math::discrete_log(6, 6, 15) == 1);
    assert(math::discrete_log(0, 0, 17) == 1);
    assert(math::discrete_log(0, 1, 17) == 0);
    assert(math::discrete_log(0, 2, 17) == -1);
}

void test_against_naive(){
    for(long long mod = 2; mod <= 80; mod++){
        for(long long a = 0; a < mod; a++){
            for(long long target = 0; target < mod; target++){
                long long expected = -1;
                long long cur = 1 % mod;
                for(long long x = 0; x <= mod + 5; x++){
                    if(cur == target){
                        expected = x;
                        break;
                    }
                    cur = (__int128)cur * a % mod;
                }
                assert(math::discrete_log(a, target, mod) == expected);
            }
        }
    }
}

void test_exceptions(){
    bool thrown = false;
    try{
        (void)math::discrete_log(2, 3, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)math::discrete_log_coprime(2, 4, 8);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    test_coprime();
    test_non_coprime();
    test_against_naive();
    test_exceptions();
}
