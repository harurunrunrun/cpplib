// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <random>
#include <vector>

#include "../../src/algorithm/other/sequence/inversion_count.hpp"

template<class T, class Compare>
std::uint64_t naive_inversion_count(
    const std::vector<T>& values,
    Compare compare
){
    std::uint64_t result = 0;
    for(std::size_t i = 0; i < values.size(); ++i){
        for(std::size_t j = i + 1; j < values.size(); ++j){
            if(compare(values[j], values[i])) ++result;
        }
    }
    return result;
}

struct NonCopyable{
    int key;

    explicit NonCopyable(int value) : key(value){}
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = default;
    NonCopyable& operator=(NonCopyable&&) = default;
};

void self_test(){
    assert(inversion_count(std::vector<int>{}) == 0);
    assert(inversion_count(std::vector<int>{1}) == 0);
    assert(inversion_count(std::vector<int>{1, 2, 3, 4}) == 0);
    assert(inversion_count(std::vector<int>{4, 3, 2, 1}) == 6);
    assert(inversion_count(std::vector<int>{3, 1, 2, 1}) == 4);
    assert(inversion_count(
        std::vector<int>{1, 2, 3, 4},
        std::greater<int>{}
    ) == 6);

    std::vector<NonCopyable> records;
    records.reserve(4);
    records.emplace_back(4);
    records.emplace_back(1);
    records.emplace_back(3);
    records.emplace_back(2);
    assert(inversion_count(records, [](const NonCopyable& left, const NonCopyable& right){
        return left.key < right.key;
    }) == 4);

    std::mt19937_64 random(2026071403);
    for(int iteration = 0; iteration < 3000; ++iteration){
        const std::size_t size = static_cast<std::size_t>(random() % 50);
        std::vector<int> values(size);
        for(int& value: values){
            value = static_cast<int>(random() % 21) - 10;
        }
        assert(inversion_count(values)
            == naive_inversion_count(values, std::less<int>{}));
        assert(inversion_count(values, std::greater<int>{})
            == naive_inversion_count(values, std::greater<int>{}));
    }

    constexpr int large_size = 300000;
    std::vector<int> descending(static_cast<std::size_t>(large_size));
    for(int i = 0; i < large_size; ++i){
        descending[static_cast<std::size_t>(i)] = large_size - i;
    }
    assert(inversion_count(descending)
        == static_cast<std::uint64_t>(large_size) * (large_size - 1) / 2);
}

int main(){
    int size;
    if(!(std::cin >> size)){
        self_test();
        return 0;
    }
    std::vector<long long> values(static_cast<std::size_t>(size));
    for(long long& value: values) std::cin >> value;
    std::cout << inversion_count(values) << '\n';
}
