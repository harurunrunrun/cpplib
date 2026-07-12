// competitive-verifier: STANDALONE

#include <cassert>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/segmented_sieve.hpp"

void test_basic(){
    const auto primes = math::primes_in_range(0, 30);
    assert((primes == std::vector<long long>{2, 3, 5, 7, 11, 13, 17, 19, 23, 29}));

    const auto is_prime = math::segmented_sieve(10, 20);
    assert(is_prime.size() == 10);
    assert(!is_prime[0]);
    assert(is_prime[1]);
    assert(!is_prime[2]);
    assert(is_prime[3]);
    assert(!is_prime[4]);
    assert(!is_prime[5]);
    assert(!is_prime[6]);
    assert(is_prime[7]);
    assert(!is_prime[8]);
    assert(is_prime[9]);
}

void test_ranges(){
    assert((math::primes_in_range(1, 2) == std::vector<long long>{}));
    assert((math::primes_in_range(2, 3) == std::vector<long long>{2}));
    assert((math::primes_in_range(100, 130) == std::vector<long long>{101, 103, 107, 109, 113, 127}));
    assert((math::primes_in_range(999999990000LL, 999999990050LL) == std::vector<long long>{999999990047LL}));
}

void test_empty(){
    const auto is_prime = math::segmented_sieve(100, 100);
    assert(is_prime.empty());
    assert(math::primes_in_range(100, 100).empty());
}

void test_exceptions(){
    bool thrown = false;
    try{
        (void)math::segmented_sieve(-1, 10);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)math::segmented_sieve(10, 9);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    test_basic();
    test_ranges();
    test_empty();
    test_exceptions();
}
