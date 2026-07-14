// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../../src/structure/hash/associative_array.hpp"

struct ConstantHash{
    std::size_t operator()(int) const{ return 0; }
};

void test_random(){
    std::mt19937_64 rng(2026071401);
    AssociativeArray<std::uint64_t, long long, 512> table;
    std::unordered_map<std::uint64_t, long long> expected;
    for(int step = 0; step < 20000; ++step){
        const std::uint64_t key = rng() % 700;
        const int operation = static_cast<int>(rng() % 8);
        if(operation < 3){
            const long long value = static_cast<long long>(rng());
            if(expected.size() < 512 || expected.contains(key)){
                const bool inserted = table.insert_or_assign(key, value);
                assert(inserted == !expected.contains(key));
                expected[key] = value;
            }
        }else if(operation == 3){
            const bool erased = table.erase(key);
            assert(erased == (expected.erase(key) != 0));
        }else if(operation == 4 && (expected.size() < 512 || expected.contains(key))){
            table[key] += 7;
            expected[key] += 7;
        }else{
            const auto iterator = expected.find(key);
            const long long* value = table.find(key);
            assert((value != nullptr) == (iterator != expected.end()));
            if(value != nullptr) assert(*value == iterator->second);
            assert(table.contains(key) == (iterator != expected.end()));
        }
        assert(table.size() == static_cast<int>(expected.size()));
        assert(table.empty() == expected.empty());
    }
}

void test_collisions_and_erase(){
    AssociativeArray<int, int, 128, ConstantHash> table;
    static_assert(decltype(table)::max_size() == 128);
    static_assert(decltype(table)::capacity() >= 256);
    for(int key = 0; key < 128; ++key) table[key] = key * key;
    assert(table.at(3) == 9);
    table.at(3) = 10;
    assert(table.at(3) == 10);
    for(int key = 0; key < 128; key += 2) assert(table.erase(key));
    for(int key = 0; key < 128; ++key){
        const int* value = table.find(key);
        if(key % 2 == 0){
            assert(value == nullptr);
        }else{
            assert(value != nullptr && *value == (key == 3 ? 10 : key * key));
        }
    }
    for(int key = 128; key < 192; ++key) table[key] = -key;
    assert(table.size() == 128);
    bool thrown = false;
    try{ table[1000] = 1; }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    assert(table.size() == 128);
    table.clear();
    assert(table.empty());
    for(int key = 0; key < 128; ++key) assert(!table.contains(key));
    table[200] = 17;
    const auto& const_table = table;
    assert(const_table.find(200) != nullptr);
    assert(*const_table.find(200) == 17);
    assert(const_table.at(200) == 17);
    table.clear();

    AssociativeArray<int, int, 4, ConstantHash> configured(
        123, ConstantHash{}, std::equal_to<int>{});
    assert(configured.insert_or_assign(1, 2));
    assert(!configured.insert_or_assign(1, 3));
    assert(configured.at(1) == 3);

    thrown = false;
    try{ (void)table.at(7); }catch(const std::out_of_range&){ thrown = true; }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    if(!(std::cin >> query_count)){
        test_random();
        test_collisions_and_erase();
        return 0;
    }

    AssociativeArray<std::uint64_t, std::int64_t, 200000> table;
    while(query_count-- > 0){
        std::string operation;
        std::cin >> operation;
        if(operation == "SET"){
            std::uint64_t key;
            std::int64_t value;
            std::cin >> key >> value;
            table.insert_or_assign(key, value);
        }else if(operation == "ADD"){
            std::uint64_t key;
            std::int64_t value;
            std::cin >> key >> value;
            table[key] += value;
        }else if(operation == "GET"){
            std::uint64_t key;
            std::cin >> key;
            const std::int64_t* value = table.find(key);
            std::cout << (value == nullptr ? 0 : *value) << '\n';
        }else if(operation == "ERASE"){
            std::uint64_t key;
            std::cin >> key;
            std::cout << table.erase(key) << '\n';
        }else if(operation == "CONTAINS"){
            std::uint64_t key;
            std::cin >> key;
            std::cout << table.contains(key) << '\n';
        }else if(operation == "SIZE"){
            std::cout << table.size() << '\n';
        }else if(operation == "CLEAR"){
            table.clear();
        }
    }
}
