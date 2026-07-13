// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/structure/other/priority_sum_structure.hpp"

long long naive_smallest(const std::multiset<long long>& values, std::size_t k){
    long long result = 0;
    auto iterator = values.begin();
    for(std::size_t count = 0; count < k; ++count, ++iterator) result += *iterator;
    return result;
}

long long naive_largest(const std::multiset<long long>& values, std::size_t k){
    long long result = 0;
    auto iterator = values.rbegin();
    for(std::size_t count = 0; count < k; ++count, ++iterator) result += *iterator;
    return result;
}

void check_state(const PrioritySumStructure<long long, long long>& structure,
                 const std::multiset<long long>& values,
                 std::size_t k){
    assert(structure.size() == values.size());
    assert(structure.empty() == values.empty());
    assert(structure.k() == k);
    assert(structure.smallest_sum() == naive_smallest(values, k));
    assert(structure.largest_sum() == naive_largest(values, k));
}

void test_random(){
    std::mt19937_64 rng(2026071323);
    for(int iteration = 0; iteration < 300; ++iteration){
        std::vector<long long> initial;
        const std::size_t initial_size = static_cast<std::size_t>(rng() % 25ULL);
        initial.reserve(initial_size);
        for(std::size_t index = 0; index < initial_size; ++index){
            initial.push_back(static_cast<long long>(rng() % 31ULL) - 15);
        }
        std::multiset<long long> reference(initial.begin(), initial.end());
        std::size_t k = static_cast<std::size_t>(rng() % (initial_size + 1));
        PrioritySumStructure<long long, long long> structure(initial, k);
        check_state(structure, reference, k);

        for(int step = 0; step < 400; ++step){
            const int operation = static_cast<int>(rng() % 10ULL);
            if(operation < 4 || reference.empty()){
                const long long value = static_cast<long long>(rng() % 41ULL) - 20;
                structure.insert(value);
                reference.insert(value);
            }else if(operation < 7 && reference.size() > k){
                auto iterator = reference.begin();
                std::advance(iterator, static_cast<long long>(rng() % reference.size()));
                const long long value = *iterator;
                structure.erase(value);
                reference.erase(iterator);
            }else{
                k = static_cast<std::size_t>(rng() % (reference.size() + 1));
                structure.set_k(k);
            }
            check_state(structure, reference, k);
            const long long probe = static_cast<long long>(rng() % 45ULL) - 22;
            assert(structure.contains(probe) == (reference.find(probe) != reference.end()));
        }
    }
}

void test_duplicates_boundaries_and_errors(){
    PrioritySumStructure<long long, long long> structure(
        std::vector<long long>{5, -2, 5, 7, -2}, 0
    );
    assert(structure.smallest_sum() == 0);
    assert(structure.largest_sum() == 0);
    structure.set_k(structure.size());
    assert(structure.smallest_sum() == 13);
    assert(structure.largest_sum() == 13);

    bool thrown = false;
    try{ structure.erase(5); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    assert(structure.size() == 5);
    assert(structure.k() == 5);

    structure.set_k(4);
    structure.erase(5);
    assert(structure.size() == 4);
    assert(structure.smallest_sum() == 8);
    assert(structure.largest_sum() == 8);
    structure.set_k(2);
    assert(structure.smallest_sum() == -4);
    assert(structure.largest_sum() == 12);
    structure.erase(-2);
    assert(structure.smallest_sum() == 3);
    assert(structure.largest_sum() == 12);

    thrown = false;
    try{ structure.erase(123456); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ structure.set_k(4); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    thrown = false;
    try{
        (void)PrioritySumStructure<int, long long>(std::vector<int>{1, 2}, 3);
    }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    PrioritySumStructure<int, long long> wide_sum;
    for(int count = 0; count < 20; ++count) wide_sum.insert(1000000000);
    wide_sum.set_k(20);
    assert(wide_sum.smallest_sum() == 20000000000LL);
    assert(wide_sum.largest_sum() == 20000000000LL);
}

struct Item{
    int priority;
    long long cost;
};

struct ItemCompare{
    bool operator()(const Item& left, const Item& right) const{
        return left.priority < right.priority;
    }
};

struct ItemProjection{
    long long operator()(const Item& item) const{
        return item.cost;
    }
};

void test_generic_order_and_projection(){
    PrioritySumStructure<Item, long long, ItemCompare, ItemProjection> projected;
    projected.insert(Item{10, 100});
    projected.insert(Item{-2, 7});
    projected.insert(Item{4, -30});
    projected.insert(Item{8, 11});
    projected.set_k(2);
    assert(projected.smallest_sum() == -23);
    assert(projected.largest_sum() == 111);

    PrioritySumStructure<int, long long, std::greater<int>> reversed;
    for(const int value: std::vector<int>{1, 9, -3, 4}) reversed.insert(value);
    reversed.set_k(2);
    assert(reversed.smallest_sum() == 13);
    assert(reversed.largest_sum() == -2);
}

void test_large(){
    constexpr std::size_t size = 120000;
    PrioritySumStructure<int, long long> structure;
    std::vector<int> sorted;
    sorted.reserve(size);
    for(std::size_t index = 0; index < size; ++index){
        const int value = static_cast<int>(index % 1000) - 500;
        structure.insert(value);
        sorted.push_back(value);
    }
    std::sort(sorted.begin(), sorted.end());
    std::vector<long long> prefix(size + 1);
    for(std::size_t index = 0; index < size; ++index){
        prefix[index + 1] = prefix[index] + sorted[index];
    }
    const long long total = prefix.back();
    std::mt19937_64 rng(2026071324);
    for(int iteration = 0; iteration < 20000; ++iteration){
        const std::size_t k = static_cast<std::size_t>(rng() % (size + 1));
        structure.set_k(k);
        assert(structure.smallest_sum() == prefix[k]);
        assert(structure.largest_sum() == total - prefix[size - k]);
    }
    structure.set_k(0);
    long long removed_sum = 0;
    for(std::size_t index = 0; index < 30000; ++index){
        const int value = static_cast<int>(index % 1000) - 500;
        structure.erase(value);
        removed_sum += value;
    }
    assert(structure.size() == size - 30000);
    structure.set_k(structure.size());
    assert(structure.smallest_sum() == total - removed_sum);
    assert(structure.largest_sum() == total - removed_sum);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t n, k, query_count;
    if(!(std::cin >> n >> k >> query_count)){
        test_random();
        test_duplicates_boundaries_and_errors();
        test_generic_order_and_projection();
        test_large();
        return 0;
    }
    std::vector<long long> initial(n);
    for(long long& value: initial) std::cin >> value;
    PrioritySumStructure<long long, long long> structure(initial, k);
    while(query_count-- > 0){
        std::string operation;
        std::cin >> operation;
        if(operation == "INSERT"){
            long long value;
            std::cin >> value;
            structure.insert(value);
        }else if(operation == "ERASE"){
            long long value;
            std::cin >> value;
            structure.erase(value);
        }else if(operation == "SETK"){
            std::size_t next_k;
            std::cin >> next_k;
            structure.set_k(next_k);
        }else if(operation == "SMALL"){
            std::cout << structure.smallest_sum() << '\n';
        }else if(operation == "LARGE"){
            std::cout << structure.largest_sum() << '\n';
        }else if(operation == "SIZE"){
            std::cout << structure.size() << '\n';
        }else if(operation == "K"){
            std::cout << structure.k() << '\n';
        }else{
            throw std::runtime_error("unknown PrioritySumStructure operation");
        }
    }
}
