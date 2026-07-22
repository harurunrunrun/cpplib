// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/other/utility/coordinate_compression.hpp"

static void test_random(){
    std::mt19937_64 rng(2026071605);
    for(int iteration = 0; iteration < 1000; ++iteration){
        const std::size_t size = rng() % 200;
        std::vector<long long> input(size);
        for(long long& value: input){
            value = static_cast<long long>(rng() % 101) - 50;
        }
        std::vector<long long> sorted = input;
        std::sort(sorted.begin(), sorted.end());
        sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());
        CoordinateCompression<long long, 200> compression(input);
        assert(std::vector<long long>(compression.values().begin(),
                                      compression.values().end()) == sorted);
        assert(compression.size() == sorted.size());
        const std::vector<std::size_t> compressed = compression.compress(input);
        for(std::size_t index = 0; index < input.size(); ++index){
            assert(compression.value(compressed[index]) == input[index]);
        }
        for(long long probe = -55; probe <= 55; ++probe){
            const std::size_t lower = static_cast<std::size_t>(
                std::lower_bound(sorted.begin(), sorted.end(), probe) - sorted.begin());
            const std::size_t upper = static_cast<std::size_t>(
                std::upper_bound(sorted.begin(), sorted.end(), probe) - sorted.begin());
            assert(compression.lower_bound_index(probe) == lower);
            assert(compression.upper_bound_index(probe) == upper);
            assert(compression.contains(probe) ==
                   std::binary_search(sorted.begin(), sorted.end(), probe));
        }
    }
}

static void test_errors_and_rebuild(){
    CoordinateCompression<int, 4> compression;
    assert(compression.empty());
    compression.build({4, 4, -1, 8});
    assert(compression.index(-1) == 0);
    assert(compression.index(4) == 1);
    assert(compression.index(8) == 2);
    bool thrown = false;
    try{ (void)compression.index(5); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)compression.value(3); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ compression.build({1, 1, 1, 1, 1}); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    assert(compression.size() == 3);
    compression.build({7});
    assert(compression.size() == 1 && compression.value(0) == 7);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t n, query_count;
    if(!(std::cin >> n >> query_count)){
        test_random();
        test_errors_and_rebuild();
        return 0;
    }
    std::vector<long long> values(n);
    for(long long& value: values) std::cin >> value;
    CoordinateCompression<long long, 200000> compression(values);
    while(query_count-- > 0){
        std::string operation;
        long long value;
        std::cin >> operation >> value;
        if(operation == "INDEX"){
            std::cout << compression.index(value) << '\n';
        }else if(operation == "LOWER"){
            std::cout << compression.lower_bound_index(value) << '\n';
        }else if(operation == "UPPER"){
            std::cout << compression.upper_bound_index(value) << '\n';
        }else if(operation == "CONTAINS"){
            std::cout << compression.contains(value) << '\n';
        }else if(operation == "VALUE"){
            std::cout << compression.value(static_cast<std::size_t>(value)) << '\n';
        }else{
            throw std::runtime_error("unknown CoordinateCompression operation");
        }
    }
}

