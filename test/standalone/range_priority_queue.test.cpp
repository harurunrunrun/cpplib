// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/structure/other/range_priority_queue.hpp"

static std::pair<int, std::size_t> naive_best(
    const std::vector<std::multiset<int>>& values,
    std::size_t left,
    std::size_t right
){
    bool found = false;
    std::pair<int, std::size_t> answer{};
    for(std::size_t index = left; index < right; ++index){
        if(values[index].empty()) continue;
        const int candidate = *values[index].rbegin();
        if(!found || candidate > answer.first ||
           (candidate == answer.first && index < answer.second)){
            answer = {candidate, index};
            found = true;
        }
    }
    if(!found) throw std::runtime_error("empty naive range");
    return answer;
}

static void test_random(){
    std::mt19937_64 rng(2026071601);
    for(int iteration = 0; iteration < 80; ++iteration){
        const std::size_t bucket_count = 1 + rng() % 40;
        RangePriorityQueue<int> queue(bucket_count);
        std::vector<std::multiset<int>> values(bucket_count);
        for(int step = 0; step < 3000; ++step){
            std::size_t left = rng() % (bucket_count + 1);
            std::size_t right = rng() % (bucket_count + 1);
            if(left > right) std::swap(left, right);
            const int operation = static_cast<int>(rng() % 10);
            if(operation < 5 || left == right){
                const std::size_t index = rng() % bucket_count;
                const int value = static_cast<int>(rng() % 2001) - 1000;
                queue.push(index, value);
                values[index].insert(value);
            }else{
                bool empty = true;
                for(std::size_t index = left; index < right; ++index){
                    empty = empty && values[index].empty();
                }
                assert(queue.range_empty(left, right) == empty);
                if(empty) continue;
                const auto expected = naive_best(values, left, right);
                assert(queue.top(left, right) == expected.first);
                assert(queue.top_with_index(left, right) == expected);
                if(operation >= 8){
                    assert(queue.pop(left, right) == expected.first);
                    auto iterator = values[expected.second].find(expected.first);
                    values[expected.second].erase(iterator);
                }
            }
        }
        for(std::size_t index = 0; index < bucket_count; ++index){
            assert(queue.bucket_element_count(index) == values[index].size());
            while(!values[index].empty()){
                assert(queue.bucket_top(index) == *values[index].rbegin());
                assert(queue.pop_bucket(index) == *values[index].rbegin());
                values[index].erase(std::prev(values[index].end()));
            }
            assert(queue.empty(index));
        }
    }
}

static void test_compare_and_errors(){
    RangePriorityQueue<int, std::greater<int>> minimum_queue(3);
    minimum_queue.push(0, 8);
    minimum_queue.push(0, -2);
    minimum_queue.push(2, -2);
    minimum_queue.push(1, 4);
    assert(minimum_queue.top_with_index(0, 3) == std::make_pair(-2, std::size_t{0}));
    assert(minimum_queue.pop(1, 3) == -2);
    assert(minimum_queue.top(0, 3) == -2);

    RangePriorityQueue<int> queue(0);
    assert(queue.size() == 0);
    assert(queue.range_empty(0, 0));
    bool thrown = false;
    try{ (void)queue.top(0, 0); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ queue.push(0, 1); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t bucket_count, query_count;
    if(!(std::cin >> bucket_count >> query_count)){
        test_random();
        test_compare_and_errors();
        return 0;
    }
    RangePriorityQueue<long long> queue(bucket_count);
    while(query_count-- > 0){
        std::string operation;
        std::cin >> operation;
        if(operation == "PUSH"){
            std::size_t index;
            long long value;
            std::cin >> index >> value;
            queue.push(index, value);
        }else if(operation == "TOP"){
            std::size_t left, right;
            std::cin >> left >> right;
            const auto [value, index] = queue.top_with_index(left, right);
            std::cout << value << ' ' << index << '\n';
        }else if(operation == "POP"){
            std::size_t left, right;
            std::cin >> left >> right;
            std::cout << queue.pop(left, right) << '\n';
        }else if(operation == "EMPTY"){
            std::size_t left, right;
            std::cin >> left >> right;
            std::cout << queue.range_empty(left, right) << '\n';
        }else if(operation == "COUNT"){
            std::size_t index;
            std::cin >> index;
            std::cout << queue.bucket_element_count(index) << '\n';
        }else{
            throw std::runtime_error("unknown RangePriorityQueue operation");
        }
    }
}

