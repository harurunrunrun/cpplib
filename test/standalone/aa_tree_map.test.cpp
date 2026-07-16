// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <random>
#include <stdexcept>
#include <memory>
#include <string>
#include <utility>

#include "../../src/structure/bbst/aa_tree_map.hpp"

static void verify(
    const AATreeMap<int, long long, 500>& tree,
    const std::map<int, long long>& reference
){
    assert(tree.size() == static_cast<int>(reference.size()));
    int index = 0;
    for(const auto& [key, value]: reference){
        assert(tree.contains(key));
        assert(tree.at(key) == value);
        assert(tree.kth(index) == std::make_pair(key, value));
        assert(tree.order_of_key(key) == index);
        ++index;
    }
    for(int probe = -110; probe <= 110; ++probe){
        const auto lower = reference.lower_bound(probe);
        const auto upper = reference.upper_bound(probe);
        const auto* actual_lower = tree.lower_bound(probe);
        const auto* actual_upper = tree.upper_bound(probe);
        assert((actual_lower == nullptr) == (lower == reference.end()));
        assert((actual_upper == nullptr) == (upper == reference.end()));
        if(actual_lower != nullptr){
            assert(actual_lower->first == lower->first);
            assert(actual_lower->second == lower->second);
        }
        if(actual_upper != nullptr){
            assert(actual_upper->first == upper->first);
            assert(actual_upper->second == upper->second);
        }
        assert(tree.order_of_key(probe) ==
            std::distance(reference.begin(), lower));
    }
}

static void test_random(){
    std::mt19937_64 rng(2026071624);
    for(int iteration = 0; iteration < 300; ++iteration){
        AATreeMap<int, long long, 500> tree;
        std::map<int, long long> reference;
        for(int step = 0; step < 3000; ++step){
            const int key = static_cast<int>(rng() % 201) - 100;
            const long long value = static_cast<long long>(rng());
            const int operation = static_cast<int>(rng() % 5);
            if(operation == 0){
                const bool expected = reference.emplace(key, value).second;
                assert(tree.insert(key, value) == expected);
            }else if(operation == 1){
                const bool expected = reference.find(key) == reference.end();
                reference[key] = value;
                assert(tree.insert_or_assign(key, value) == expected);
            }else if(operation == 2){
                assert(tree.erase(key) == (reference.erase(key) != 0));
            }else if(operation == 3){
                const auto iterator = reference.find(key);
                assert((tree.find(key) == nullptr) == (iterator == reference.end()));
                if(iterator != reference.end()) assert(*tree.find(key) == iterator->second);
            }else if(!reference.empty()){
                const int index = static_cast<int>(rng() % reference.size());
                auto iterator = reference.begin();
                std::advance(iterator, index);
                const auto actual = tree.kth(index);
                assert(actual.first == iterator->first);
                assert(actual.second == iterator->second);
            }
            if(step % 100 == 0) verify(tree, reference);
        }
        verify(tree, reference);
        tree.clear();
        assert(tree.empty());
    }
}

static void test_capacity_compare_and_errors(){
    AATreeMap<int, int, 3> small;
    assert(small.insert(2, 20));
    assert(small.insert(1, 10));
    assert(small.insert(3, 30));
    assert(!small.insert(2, 99));
    assert(small.at(2) == 20);
    bool thrown = false;
    try{ small.insert(4, 40); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    assert(small.erase(2));
    assert(small.insert(4, 40));
    thrown = false;
    try{ (void)small.at(2); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)small.kth(3); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    AATreeMap<int, int, 10, std::greater<int>> descending;
    for(int value: {1, 4, 2, 9}) descending.insert(value, value * 10);
    assert(descending.kth(0).first == 9);
    assert(descending.kth(3).first == 1);
    assert(descending.order_of_key(4) == 1);
}

static void test_large(){
    constexpr int size = 100000;
    auto tree = std::make_unique<AATreeMap<int, int, size>>();
    for(int value = 0; value < size; ++value){
        const int key = static_cast<int>(
            static_cast<long long>(value) * 48271 % size
        );
        assert(tree->insert(key, value));
    }
    assert(tree->size() == size);
    for(int index = 0; index < size; index += 997){
        assert(tree->kth(index).first == index);
        assert(tree->order_of_key(index) == index);
    }
    for(int key = 0; key < size; key += 2) assert(tree->erase(key));
    assert(tree->size() == size / 2);
    for(int index = 0; index < tree->size(); index += 499){
        assert(tree->kth(index).first == index * 2 + 1);
    }
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t query_count;
    if(!(std::cin >> query_count)){
        test_random();
        test_capacity_compare_and_errors();
        test_large();
        return 0;
    }
    auto tree_storage = std::make_unique<AATreeMap<long long, long long, 200000>>();
    auto& tree = *tree_storage;
    while(query_count-- > 0){
        std::string operation;
        std::cin >> operation;
        if(operation == "INSERT" || operation == "SET"){
            long long key, value;
            std::cin >> key >> value;
            const bool inserted = operation == "INSERT"
                ? tree.insert(key, value)
                : tree.insert_or_assign(key, value);
            std::cout << inserted << '\n';
        }else if(operation == "ERASE"){
            long long key;
            std::cin >> key;
            std::cout << tree.erase(key) << '\n';
        }else if(operation == "GET"){
            long long key;
            std::cin >> key;
            const long long* value = tree.find(key);
            if(value == nullptr) std::cout << "NONE\n";
            else std::cout << *value << '\n';
        }else if(operation == "LOWER" || operation == "UPPER"){
            long long key;
            std::cin >> key;
            const auto* value = operation == "LOWER"
                ? tree.lower_bound(key) : tree.upper_bound(key);
            if(value == nullptr) std::cout << "NONE\n";
            else std::cout << value->first << ' ' << value->second << '\n';
        }else if(operation == "ORDER"){
            long long key;
            std::cin >> key;
            std::cout << tree.order_of_key(key) << '\n';
        }else if(operation == "KTH"){
            int index;
            std::cin >> index;
            const auto& value = tree.kth(index);
            std::cout << value.first << ' ' << value.second << '\n';
        }else if(operation == "SIZE"){
            std::cout << tree.size() << '\n';
        }else{
            throw std::runtime_error("unknown AATreeMap operation");
        }
    }
}

