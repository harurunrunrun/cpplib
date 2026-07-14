// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <unordered_set>

#include "../../src/approximate/streaming/hyperloglog.hpp"
#include "../../src/approximate/streaming/linear_counting.hpp"
#include "../../src/approximate/streaming/minhash.hpp"
#include "../../src/approximate/streaming/simhash.hpp"

int main(){
    int first_count, second_count;
    if(!(std::cin >> first_count >> second_count)) return 0;
    HyperLogLog<12> hyperloglog(314159);
    LinearCounting<65536> linear(314159);
    MinHash<256> first_hash(271828), second_hash(271828);
    SimHash<64> first_simhash(161803), second_simhash(161803);
    std::unordered_set<std::uint64_t> first_set, second_set;
    assert((HyperLogLog<12>::register_count() == 4096U));
    assert(HyperLogLog<12>::relative_standard_error() > 0.0L);
    assert((MinHash<256>::signature_size() == 256U));
    assert(linear.occupied_bits() == 0U);
    for(int i = 0; i < first_count; ++i){
        std::uint64_t value;
        std::cin >> value;
        first_set.insert(value);
        hyperloglog.add(value);
        linear.add(value);
        first_hash.add(value);
        first_simhash.add(value);
    }
    for(int i = 0; i < second_count; ++i){
        std::uint64_t value;
        std::cin >> value;
        second_set.insert(value);
        second_hash.add(value);
        second_simhash.add(value);
    }
    const long double exact_cardinality = static_cast<long double>(first_set.size());
    assert(linear.occupied_bits() <= LinearCounting<65536>::bit_count());
    assert(linear.occupied_bits() <= first_set.size());
    assert((linear.occupied_bits() == 0U) == first_set.empty());
    assert(std::fabs(hyperloglog.estimate() - exact_cardinality) <= exact_cardinality * 0.12L + 2.0L);
    assert(std::fabs(linear.estimate() - exact_cardinality) <= exact_cardinality * 0.08L + 2.0L);
    std::size_t intersection = 0;
    for(const auto value: first_set) intersection += second_set.count(value);
    const std::size_t union_size = first_set.size() + second_set.size() - intersection;
    const long double jaccard = union_size == 0U ? 1.0L
        : static_cast<long double>(intersection) / static_cast<long double>(union_size);
    assert(std::fabs(first_hash.similarity(second_hash) - jaccard) <= 0.16L);
    assert(SimHash<64>::hamming_distance(first_simhash.signature(), first_simhash.signature()) == 0U);
    assert(first_simhash.similarity(first_simhash) == 1.0L);

    assert(hyperloglog.seed() == 314159U);
    assert((HyperLogLog<12>::register_count() == 4096U));
    assert(HyperLogLog<12>::relative_standard_error() > 0.0L);
    HyperLogLog<8> hll_left(99), hll_right(99), hll_union(99);
    for(std::uint64_t value = 0; value < 100; ++value){
        hll_left.add(value);
        hll_union.add(value);
    }
    for(std::uint64_t value = 100; value < 200; ++value){
        hll_right.add(value);
        hll_union.add(value);
    }
    hll_left.merge(hll_right);
    assert(hll_left.estimate() == hll_union.estimate());
    bool thrown = false;
    try{
        HyperLogLog<8> different_seed(100);
        hll_left.merge(different_seed);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    hll_left.clear();
    assert(hll_left.estimate() == 0.0L);
    assert(hll_left.seed() == 99U);

    assert(linear.seed() == 314159U);
    assert((LinearCounting<65536>::bit_count() == 65536U));
    LinearCounting<1024> linear_left(77), linear_right(77), linear_union(77);
    for(std::uint64_t value = 0; value < 80; ++value){
        linear_left.add(value);
        linear_union.add(value);
    }
    for(std::uint64_t value = 80; value < 160; ++value){
        linear_right.add(value);
        linear_union.add(value);
    }
    linear_left.merge(linear_right);
    assert(linear_left.occupied_bits() == linear_union.occupied_bits());
    assert(linear_left.estimate() == linear_union.estimate());
    thrown = false;
    try{
        LinearCounting<1024> different_seed(78);
        linear_left.merge(different_seed);
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    linear_left.clear();
    assert(linear_left.occupied_bits() == 0U);
    assert(linear_left.estimate() == 0.0L);

    assert((MinHash<256>::signature_size() == 256U));
    assert(first_hash.seed() == 271828U);
    MinHash<32> minhash_left(55), minhash_right(55), minhash_union(55);
    for(std::uint64_t value = 0; value < 40; ++value){
        minhash_left.add(value);
        minhash_union.add(value);
    }
    for(std::uint64_t value = 40; value < 80; ++value){
        minhash_right.add(value);
        minhash_union.add(value);
    }
    minhash_left.merge(minhash_right);
    assert(minhash_left.signature() == minhash_union.signature());
    assert(!minhash_left.empty());
    minhash_left.clear();
    assert(minhash_left.empty());
    for(std::uint64_t value: minhash_left.signature()){
        assert(value == std::numeric_limits<std::uint64_t>::max());
    }

    assert(first_simhash.seed() == 161803U);
    assert((SimHash<64>::bit_count() == 64U));
    first_simhash.clear();
    assert(first_simhash.similarity(first_simhash) == 1.0L);
    std::cout << "OK\n";
}
