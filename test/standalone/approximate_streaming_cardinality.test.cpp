// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
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
    std::cout << "OK\n";
}
