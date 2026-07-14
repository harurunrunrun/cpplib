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
    std::cout << "OK\n";
}
