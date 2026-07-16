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

#include "../../src/structure/other/dynamic_median_multiset.hpp"

namespace{

void assert_matches(
    const DynamicMedianMultiset<int>& values,
    const std::multiset<int>& expected
){
    assert(values.size() == expected.size());
    assert(values.empty() == expected.empty());
    if(expected.empty()) return;

    const auto lower = std::next(
        expected.begin(),
        static_cast<std::ptrdiff_t>((expected.size() - 1) / 2)
    );
    const auto upper = std::next(
        expected.begin(),
        static_cast<std::ptrdiff_t>(expected.size() / 2)
    );
    assert(values.lower_median() == *lower);
    assert(values.upper_median() == *upper);
    if((expected.size() & 1U) != 0U){
        assert(values.odd_median() == *lower);
    }else{
        assert((values.even_medians() == std::pair<int, int>(*lower, *upper)));
    }
}

void self_test(){
    DynamicMedianMultiset<int> values;
    std::multiset<int> expected;
    assert_matches(values, expected);

    bool empty_thrown = false;
    try{
        (void)values.lower_median();
    }catch(const std::out_of_range&){
        empty_thrown = true;
    }
    assert(empty_thrown);

    for(const int value: {5, 1, 5, -4, 8, 5}){
        values.insert(value);
        expected.insert(value);
        assert_matches(values, expected);
    }
    assert(values.contains(5));
    assert(values.count(5) == 3);
    assert(values.erase_one(5));
    expected.erase(expected.find(5));
    assert_matches(values, expected);
    assert(values.erase_all(5) == 2);
    expected.erase(5);
    assert_matches(values, expected);
    assert(!values.erase_one(100));
    assert(values.erase_all(100) == 0);

    values.clear();
    expected.clear();
    values.insert(1);
    values.insert(2);
    bool parity_thrown = false;
    try{
        (void)values.odd_median();
    }catch(const std::logic_error&){
        parity_thrown = true;
    }
    assert(parity_thrown);
    values.insert(3);
    parity_thrown = false;
    try{
        (void)values.even_medians();
    }catch(const std::logic_error&){
        parity_thrown = true;
    }
    assert(parity_thrown);

    DynamicMedianMultiset<int, std::greater<int>> descending;
    for(const int value: {1, 2, 3, 4}) descending.insert(value);
    assert(descending.lower_median() == 3);
    assert(descending.upper_median() == 2);
    assert((descending.even_medians() == std::pair<int, int>(3, 2)));

    values.clear();
    expected.clear();
    std::mt19937_64 engine(2026071602ULL);
    for(int step = 0; step < 30000; ++step){
        const int value = static_cast<int>(engine() % 101) - 50;
        const unsigned operation = static_cast<unsigned>(engine() % 10);
        if(operation < 4 || expected.empty()){
            values.insert(value);
            expected.insert(value);
        }else if(operation < 7){
            const bool erased = values.erase_one(value);
            const auto iterator = expected.find(value);
            assert(erased == (iterator != expected.end()));
            if(iterator != expected.end()) expected.erase(iterator);
        }else if(operation == 7){
            const std::size_t erased = values.erase_all(value);
            assert(erased == expected.count(value));
            expected.erase(value);
        }else if(operation == 8){
            assert(values.contains(value) == expected.contains(value));
            assert(values.count(value) == expected.count(value));
        }else if((engine() & 31U) == 0U){
            values.clear();
            expected.clear();
        }
        assert_matches(values, expected);
    }

    values.clear();
    for(int index = 0; index < 60000; ++index){
        values.insert(index % 1000);
    }
    assert(values.size() == 60000);
    assert(values.lower_median() == 499);
    assert(values.upper_median() == 500);
    for(int value = 0; value < 1000; ++value){
        assert(values.erase_all(value) == 60);
    }
    assert(values.empty());
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    if(!(std::cin >> query_count)){
        self_test();
        return 0;
    }

    DynamicMedianMultiset<long long> values;
    while(query_count-- != 0){
        std::string operation;
        std::cin >> operation;
        if(operation == "INSERT"){
            long long value;
            std::cin >> value;
            values.insert(value);
        }else if(operation == "ERASE"){
            long long value;
            std::cin >> value;
            std::cout << values.erase_one(value) << '\n';
        }else if(operation == "ERASE_ALL"){
            long long value;
            std::cin >> value;
            std::cout << values.erase_all(value) << '\n';
        }else if(operation == "CONTAINS"){
            long long value;
            std::cin >> value;
            std::cout << values.contains(value) << '\n';
        }else if(operation == "COUNT"){
            long long value;
            std::cin >> value;
            std::cout << values.count(value) << '\n';
        }else if(operation == "LOWER"){
            std::cout << values.lower_median() << '\n';
        }else if(operation == "UPPER"){
            std::cout << values.upper_median() << '\n';
        }else if(operation == "ODD"){
            std::cout << values.odd_median() << '\n';
        }else if(operation == "EVEN"){
            const auto [lower, upper] = values.even_medians();
            std::cout << lower << ' ' << upper << '\n';
        }else if(operation == "SIZE"){
            std::cout << values.size() << '\n';
        }else if(operation == "EMPTY"){
            std::cout << values.empty() << '\n';
        }else if(operation == "CLEAR"){
            values.clear();
        }else{
            throw std::invalid_argument("unknown operation");
        }
    }
}
