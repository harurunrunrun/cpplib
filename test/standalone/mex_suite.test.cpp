// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <utility>
#include <vector>

#include "../../src/algorithm/graph/specialized/structural_analysis/sprague_grundy.hpp"
#include "../../src/algorithm/range/offline_query/dynamic_range_mex.hpp"
#include "../../src/algorithm/range/offline_query/offline_range_mex.hpp"
#include "../../src/algorithm/range/sliding_window/window_mex.hpp"
#include "../../src/algorithm/range/static_query/affine_sequence_mexes.hpp"
#include "../../src/algorithm/range/static_query/minimum_excluded.hpp"
#include "../../src/algorithm/range/static_query/subarray_mex_set.hpp"
#include "../../src/structure/array/mex_array.hpp"
#include "../../src/structure/interval/interval_mex_set.hpp"
#include "../../src/structure/ordered_set/integer/mex_multiset.hpp"
#include "../../src/structure/ordered_set/integer/mex_marker.hpp"
#include "../../src/structure/ordered_set/integer/persistent_mex_multiset.hpp"
#include "../../src/structure/ordered_set/integer/static_missing_set.hpp"
#include "../../src/structure/range_query/value_query/static_range_mex.hpp"
#include "../../src/structure/trie/binary/xor_mex_set.hpp"

std::size_t brute_mex(const std::vector<std::int64_t>& values){
    std::set<std::int64_t> present(values.begin(), values.end());
    std::size_t answer = 0;
    while(present.contains(static_cast<std::int64_t>(answer))) ++answer;
    return answer;
}

std::vector<std::size_t> brute_subarray_mex_values(
    const std::vector<std::int64_t>& values,
    std::size_t first
){
    std::set<std::size_t> possible;
    for(std::size_t left = 0; left < values.size(); left++){
        std::set<std::int64_t> present;
        for(std::size_t right = left; right < values.size(); right++){
            present.insert(values[right]);
            std::size_t answer = first;
            while(present.contains(static_cast<std::int64_t>(answer))) ++answer;
            possible.insert(answer);
        }
    }
    return {possible.begin(), possible.end()};
}

void test_mex_multiset(std::mt19937_64& rng){
    constexpr std::size_t universe = 19;
    MexMultiset values(universe);
    std::vector<std::size_t> frequency(universe, 0);
    for(int operation = 0; operation < 500; operation++){
        const std::size_t value = rng() % universe;
        if((rng() & 1U) != 0){
            const bool expected = frequency[value]++ == 0;
            assert(values.insert(value) == expected);
        }else{
            const bool expected = frequency[value] != 0 && --frequency[value] == 0;
            assert(values.erase(value) == expected);
        }
        std::vector<std::size_t> missing;
        std::size_t distinct = 0;
        for(std::size_t x = 0; x < universe; x++){
            if(frequency[x] == 0) missing.push_back(x);
            else ++distinct;
            assert(values.count(x) == frequency[x]);
        }
        assert(values.distinct_size() == distinct);
        assert(values.mex() == (missing.empty() ? universe : missing.front()));
        for(std::size_t index = 0; index <= missing.size(); index++){
            const auto answer = values.kth_missing(index);
            if(index < missing.size()) assert(answer && *answer == missing[index]);
            else assert(!answer);
        }
    }
}

void test_marker_persistence_and_grundy(std::mt19937_64& rng){
    MexMarker marker(17);
    marker.mark(0);
    marker.mark(2);
    assert(marker.marked(2));
    assert(marker.mex() == 1);
    marker.clear();
    assert(marker.mex() == 0);

    PersistentMexMultiset persistent(17);
    std::vector<std::vector<std::size_t>> versions(1, std::vector<std::size_t>(17, 0));
    for(int operation = 0; operation < 250; operation++){
        const std::size_t base = rng() % versions.size();
        const std::size_t value = rng() % 17;
        std::vector<std::size_t> next = versions[base];
        std::size_t version;
        if((rng() & 1U) != 0){
            ++next[value];
            version = persistent.insert(base, value);
        }else{
            if(next[value] != 0) --next[value];
            version = persistent.erase(base, value);
        }
        assert(version == versions.size());
        versions.push_back(next);
        std::size_t expected = 0;
        while(expected < 17 && next[expected] != 0) ++expected;
        assert(persistent.mex(version) == expected);
        for(std::size_t x = 0; x < 17; x++) assert(persistent.count(version, x) == next[x]);
    }

    const std::vector<std::vector<std::size_t>> graph{{1, 2}, {3}, {3, 4}, {}, {}};
    const std::vector<std::size_t> grundy = dag_grundy_numbers(graph);
    assert((grundy == std::vector<std::size_t>{0, 1, 1, 0, 0}));
    assert(normal_play_nim_sum(grundy, {1, 2}) == 0);
}

void test_mex_array(std::mt19937_64& rng){
    std::vector<std::int64_t> values(24);
    for(auto& value: values) value = static_cast<std::int64_t>(rng() % 35) - 5;
    MexArray structure(values);
    for(int operation = 0; operation < 250; operation++){
        const std::size_t index = rng() % values.size();
        const std::int64_t value = static_cast<std::int64_t>(rng() % 40) - 8;
        values[index] = value;
        structure.set(index, value);
        assert(structure[index] == value);
        assert(structure.mex() == brute_mex(values));
    }
}

void test_missing_and_xor(std::mt19937_64& rng){
    std::vector<std::uint64_t> source;
    std::set<std::uint64_t> set_values;
    for(int i = 0; i < 30; i++){
        const std::uint64_t value = rng() % 50;
        source.push_back(value);
        set_values.insert(value);
    }
    StaticMissingSet missing(source);
    std::uint64_t value = 0;
    for(std::uint64_t index = 0; index < 30; index++){
        while(set_values.contains(value)) ++value;
        assert(missing.kth_missing(index) == value);
        ++value;
    }

    XorMexSet<6> structure;
    std::vector<std::size_t> frequency(64, 0);
    for(int operation = 0; operation < 500; operation++){
        const unsigned type = rng() % 3;
        const std::size_t x = rng() % 64;
        if(type == 0){
            const bool expected = frequency[x]++ == 0;
            assert(structure.insert(x) == expected);
        }else if(type == 1){
            const bool expected = frequency[x] != 0;
            if(expected) --frequency[x];
            assert(structure.erase(x) == expected);
        }else{
            structure.apply_xor(x);
            std::vector<std::size_t> transformed(64, 0);
            for(std::size_t old = 0; old < 64; old++) transformed[old ^ x] = frequency[old];
            frequency.swap(transformed);
        }
        std::size_t answer = 0;
        while(answer < 64 && frequency[answer] != 0) ++answer;
        assert(structure.mex() == answer);
    }
}

void test_interval_mex(std::mt19937_64& rng){
    constexpr std::size_t size = 67;
    IntervalMexSet structure(size);
    std::vector<bool> present(size, false);
    for(int operation = 0; operation < 500; operation++){
        std::size_t left = rng() % (size + 1);
        std::size_t right = rng() % (size + 1);
        if(left > right) std::swap(left, right);
        if((rng() & 1U) != 0){
            const bool value = (rng() & 1U) != 0;
            structure.assign(left, right, value);
            std::fill(present.begin() + static_cast<std::ptrdiff_t>(left),
                      present.begin() + static_cast<std::ptrdiff_t>(right), value);
        }else{
            structure.flip(left, right);
            for(std::size_t index = left; index < right; index++) present[index] = !present[index];
        }
        std::size_t answer = 0;
        while(answer < size && present[answer]) ++answer;
        assert(structure.mex() == answer);
        assert(structure.distinct_size() == static_cast<std::size_t>(std::count(present.begin(), present.end(), true)));
    }
}

void test_range_mex(std::mt19937_64& rng){
    const std::size_t size = 35;
    std::vector<std::int64_t> values(size);
    for(auto& value: values) value = static_cast<std::int64_t>(rng() % 50) - 6;
    StaticRangeMex persistent(values);
    std::vector<std::pair<std::size_t, std::size_t>> ranges;
    std::vector<std::size_t> expected;
    for(std::size_t left = 0; left <= size; left++){
        for(std::size_t right = left; right <= size; right++){
            const std::vector<std::int64_t> subarray(values.begin() + static_cast<std::ptrdiff_t>(left),
                values.begin() + static_cast<std::ptrdiff_t>(right));
            const std::size_t answer = brute_mex(subarray);
            assert(persistent.range_mex(left, right) == answer);
            ranges.emplace_back(left, right);
            expected.push_back(answer);
        }
    }
    assert(offline_range_mex(values, ranges) == expected);
}

void test_dynamic_range_mex(std::mt19937_64& rng){
    std::vector<std::int64_t> values(28);
    for(auto& value: values) value = static_cast<std::int64_t>(rng() % 38) - 5;
    OfflineDynamicRangeMex structure(values);
    std::vector<std::size_t> expected;
    for(int operation = 0; operation < 300; operation++){
        if((rng() % 3) == 0){
            const std::size_t position = rng() % values.size();
            const std::int64_t value = static_cast<std::int64_t>(rng() % 45) - 8;
            structure.add_update(position, value);
            values[position] = value;
        }else{
            std::size_t left = rng() % (values.size() + 1);
            std::size_t right = rng() % (values.size() + 1);
            if(left > right) std::swap(left, right);
            assert(structure.add_query(left, right) == expected.size());
            expected.push_back(brute_mex(std::vector<std::int64_t>(
                values.begin() + static_cast<std::ptrdiff_t>(left),
                values.begin() + static_cast<std::ptrdiff_t>(right)
            )));
        }
    }
    assert(structure.solve() == expected);
    assert(structure.solve() == expected);
}

void test_mex_algorithms(std::mt19937_64& rng){
    for(int repetition = 0; repetition < 120; repetition++){
        const std::size_t size = 1 + rng() % 9;
        std::vector<std::int64_t> values(size);
        for(auto& value: values) value = static_cast<std::int64_t>(rng() % 13) - 3;
        assert(minimum_excluded(values) == brute_mex(values));
        const std::size_t window = 1 + rng() % size;
        std::vector<std::size_t> expected_windows;
        for(std::size_t left = 0; left + window <= size; left++){
            expected_windows.push_back(brute_mex(std::vector<std::int64_t>(
                values.begin() + static_cast<std::ptrdiff_t>(left),
                values.begin() + static_cast<std::ptrdiff_t>(left + window)
            )));
        }
        assert(sliding_window_mexes(values, window) == expected_windows);
        assert(minimum_fixed_window_mex(values, window) ==
            *std::min_element(expected_windows.begin(), expected_windows.end()));
        for(const std::size_t first: {std::size_t{0}, std::size_t{1}}){
            assert(subarray_mex_values(values, first) == brute_subarray_mex_values(values, first));
        }

        std::vector<std::int64_t> slopes(size);
        for(auto& slope: slopes) slope = static_cast<std::int64_t>(rng() % 9) - 4;
        const std::size_t steps = 1 + rng() % 8;
        std::vector<std::size_t> expected_affine;
        for(std::size_t step = 1; step <= steps; step++){
            std::vector<std::int64_t> current(size);
            for(std::size_t index = 0; index < size; index++){
                current[index] = values[index] + slopes[index] * static_cast<std::int64_t>(step);
            }
            expected_affine.push_back(brute_mex(current));
        }
        assert(affine_sequence_mexes(values, slopes, steps) == expected_affine);
    }
}

int main(){
    std::uint64_t seed = 20260814;
    int rounds = 4;
    if(std::cin >> seed >> rounds){}
    std::mt19937_64 rng(seed);
    for(int round = 0; round < rounds; round++){
        test_mex_multiset(rng);
        test_marker_persistence_and_grundy(rng);
        test_mex_array(rng);
        test_missing_and_xor(rng);
        test_interval_mex(rng);
        test_range_mex(rng);
        test_dynamic_range_mex(rng);
        test_mex_algorithms(rng);
    }
    std::cout << "OK\n";
}
