// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/structure/other/range_assign_frequency.hpp"

void verify(const RangeAssignFrequency<int>& structure, const std::vector<int>& values){
    assert(structure.size() == values.size());
    std::map<int, std::size_t> counts;
    for(std::size_t index = 0; index < values.size(); ++index){
        assert(structure.get(index) == values[index]);
        ++counts[values[index]];
        const auto interval = structure.segment(index);
        assert(interval.left <= index && index < interval.right);
        assert(interval.value == values[index]);
    }
    for(int value = -15; value <= 15; ++value){
        assert(structure.frequency(value) == counts[value]);
    }
    const auto segments = structure.segments();
    std::size_t position = 0;
    for(const auto& segment: segments){
        assert(segment.left == position);
        assert(segment.left < segment.right);
        for(std::size_t index = segment.left; index < segment.right; ++index){
            assert(values[index] == segment.value);
        }
        position = segment.right;
    }
    assert(position == values.size());
    assert(segments.size() == structure.segment_count());
}

void self_test(){
    RangeAssignFrequency<int> empty;
    assert(empty.empty());
    assert(empty.frequency(3) == 0);
    empty.assign(0, 0, 7);

    RangeAssignFrequency<int> uniform(10, 4);
    assert(uniform.frequency(4) == 10);
    uniform.assign(2, 8, 9);
    uniform.assign(4, 6, 4);
    verify(uniform, {4, 4, 9, 9, 4, 4, 9, 9, 4, 4});

    bool rejected = false;
    try{ (void)uniform.get(10); }
    catch(const std::runtime_error&){ rejected = true; }
    assert(rejected);
    rejected = false;
    try{ uniform.assign(5, 11, 0); }
    catch(const std::runtime_error&){ rejected = true; }
    assert(rejected);

    RangeAssignFrequency<int, std::greater<int>> reverse(
        std::vector<int>{1, 1, 2, 2, 1}, std::greater<int>{}
    );
    assert(reverse.frequency(1) == 3);
    reverse.assign(1, 4, 3);
    assert(reverse.frequency(3) == 3);

    std::mt19937_64 random(2026071615);
    for(int trial = 0; trial < 100; ++trial){
        const std::size_t size = random() % 151;
        std::vector<int> values(size);
        for(int& value: values) value = static_cast<int>(random() % 21) - 10;
        RangeAssignFrequency<int> structure(values);
        verify(structure, values);
        for(int query = 0; query < 2000; ++query){
            std::size_t left = size == 0 ? 0 : random() % (size + 1);
            std::size_t right = size == 0 ? 0 : random() % (size + 1);
            if(left > right) std::swap(left, right);
            const int value = static_cast<int>(random() % 21) - 10;
            structure.assign(left, right, value);
            std::fill(values.begin() + static_cast<std::ptrdiff_t>(left),
                values.begin() + static_cast<std::ptrdiff_t>(right), value);
            if(query % 31 == 0) verify(structure, values);
        }
        verify(structure, values);
    }

    constexpr std::size_t large_size = 500000;
    RangeAssignFrequency<int> large(large_size, 0);
    for(std::size_t index = 0; index < large_size; ++index){
        large.assign(index, index + 1, static_cast<int>(index & 1U));
    }
    assert(large.frequency(0) == large_size / 2);
    assert(large.frequency(1) == large_size / 2);
    large.assign(1, large_size - 1, 7);
    assert(large.frequency(7) == large_size - 2);
    assert(large.segment_count() == 3);
}

int main(){
    std::size_t size;
    std::size_t query_count;
    if(!(std::cin >> size >> query_count)){
        self_test();
        return 0;
    }
    std::vector<long long> values(size);
    for(long long& value: values) std::cin >> value;
    RangeAssignFrequency<long long> structure(values);
    for(std::size_t query = 0; query < query_count; ++query){
        int type;
        std::cin >> type;
        if(type == 1){
            std::size_t left, right;
            long long value;
            std::cin >> left >> right >> value;
            structure.assign(left, right, value);
        }else{
            long long value;
            std::cin >> value;
            std::cout << structure.frequency(value) << '\n';
        }
    }
}
