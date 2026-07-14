// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "../../src/approximate/streaming/bloom_filter.hpp"
#include "../../src/approximate/streaming/cuckoo_filter.hpp"
#include "../../src/approximate/streaming/count_min_sketch.hpp"
#include "../../src/approximate/streaming/count_sketch.hpp"
#include "../../src/approximate/streaming/hash.hpp"

int main(){
    int count;
    if(!(std::cin >> count)) return 0;
    using approximate_streaming_detail::key_hash;
    using approximate_streaming_detail::splitmix64;
    using approximate_streaming_detail::unit_open;
    assert(splitmix64(0) == 0xe220a8397b1dcdafULL);
    assert(splitmix64(0) != splitmix64(1));
    assert(key_hash(std::uint64_t{42}, 7) == key_hash(std::uint64_t{42}, 7));
    const long double minimum_unit = unit_open(0);
    const long double maximum_unit = unit_open(~std::uint64_t{0});
    assert(0.0L < minimum_unit && minimum_unit < maximum_unit);
    assert(maximum_unit < 1.0L);

    std::vector<std::uint64_t> values(static_cast<std::size_t>(count));
    BloomFilter<65536, 7> bloom(12345);
    CountingBloomFilter<65536, 7> counting(12345);
    CuckooFilter<2048, 4, 16> cuckoo(54321);
    CountMinSketch<4096, 7> count_min(67890);
    CountSketch<4096, 9> count_sketch(24680);
    std::unordered_map<std::uint64_t, std::int64_t> exact;
    assert((CountingBloomFilter<65536, 7>::counter_count() == 65536U));
    assert((CountMinSketch<4096, 7>::width() == 4096U));
    assert((CountMinSketch<4096, 7>::depth() == 7U));
    assert(std::fabs(CountMinSketch<4096, 7>::additive_error_rate()
        - std::exp(1.0L) / 4096.0L) < 1.0e-18L);
    assert(bloom.estimated_false_positive_probability() == 0.0L);
    assert(cuckoo.load_factor() == 0.0L);
    for(auto& value: values){
        std::cin >> value;
        bloom.add(value);
        counting.add(value);
        assert(cuckoo.add(value));
        count_min.add(value);
        count_sketch.add(value);
        ++exact[value];
    }
    for(const auto& entry: exact){
        assert(bloom.contains(entry.first));
        assert(counting.contains(entry.first));
        assert(cuckoo.contains(entry.first));
        assert(counting.estimate_multiplicity(entry.first)
               >= static_cast<std::uint16_t>(entry.second));
        assert(count_min.estimate(entry.first) >= static_cast<std::uint64_t>(entry.second));
        assert(std::llabs(count_sketch.estimate(entry.first) - entry.second) <= count / 8 + 2);
    }
    for(const auto& entry: exact){
        assert(counting.remove(entry.first));
        assert(cuckoo.erase(entry.first));
        if(entry.second > 1) assert(counting.contains(entry.first));
    }
    assert(bloom.insertions() == static_cast<std::uint64_t>(count));
    assert(count_min.total() == static_cast<std::uint64_t>(count));
    assert((BloomFilter<65536, 7>::bit_count() == 65536U));
    assert((CountMinSketch<4096, 7>::failure_probability_bound() < 0.001L));
    const long double false_positive_probability =
        bloom.estimated_false_positive_probability();
    assert(0.0L <= false_positive_probability);
    assert(false_positive_probability <= 1.0L);
    assert(cuckoo.load_factor() == static_cast<long double>(cuckoo.size())
        / static_cast<long double>(cuckoo.capacity()));
    assert((BloomFilter<65536, 7>::hash_count() == 7U));
    assert((CountingBloomFilter<65536, 7>::hash_count() == 7U));
    assert((CountSketch<4096, 9>::width() == 4096U));
    assert((CountSketch<4096, 9>::depth() == 9U));

    BloomFilter<128, 3> bloom_state(1);
    assert(bloom_state.insertions() == 0U);
    bloom_state.add(42);
    assert(bloom_state.contains(42));
    assert(bloom_state.insertions() == 1U);
    assert(bloom_state.estimated_false_positive_probability() > 0.0L);
    bloom_state.clear();
    assert(!bloom_state.contains(42));
    assert(bloom_state.insertions() == 0U);
    assert(bloom_state.estimated_false_positive_probability() == 0.0L);

    CountingBloomFilter<128, 3> counting_state(2);
    counting_state.add(42);
    counting_state.add(42);
    assert(counting_state.insertions() == 2U);
    assert(counting_state.estimate_multiplicity(42) >= 2U);
    counting_state.clear();
    assert(counting_state.insertions() == 0U);
    assert(!counting_state.contains(42));
    assert(!counting_state.remove(42));

    CuckooFilter<8, 4, 16> cuckoo_state(3);
    assert(cuckoo_state.size() == 0U);
    assert((CuckooFilter<8, 4, 16>::capacity() == 32U));
    assert(cuckoo_state.add(42));
    assert(cuckoo_state.size() == 1U);
    assert(cuckoo_state.load_factor() == 1.0L / 32.0L);
    assert(cuckoo_state.add(42));
    assert(cuckoo_state.size() == 1U);
    cuckoo_state.clear();
    assert(cuckoo_state.size() == 0U);
    assert(cuckoo_state.load_factor() == 0.0L);

    count_min.clear();
    assert(count_min.total() == 0U);
    assert(count_min.estimate(42) == 0U);
    count_sketch.clear();
    count_sketch.add(42, 5);
    count_sketch.add(42, -2);
    assert(count_sketch.estimate(42) == 3);
    count_sketch.clear();
    assert(count_sketch.estimate(42) == 0);
    std::cout << "OK\n";
}
