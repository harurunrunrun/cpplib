// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <optional>
#include <random>
#include <unordered_map>
#include <vector>

#include "../../src/algorithm/range/range_majority_query.hpp"

std::optional<int> naive_majority(
    const std::vector<int>& values,
    int left,
    int right
){
    std::unordered_map<int, int> frequency;
    for(int index = left; index < right; ++index){
        const int value = values[static_cast<std::size_t>(index)];
        if((right - left) / 2 < ++frequency[value]) return value;
    }
    return std::nullopt;
}

void self_test(){
    {
        RangeMajorityQuery<int> query({1, 2, 1, 1, 3, 1});
        assert(query.size() == 6);
        assert(query.get(4) == 3);
        assert(query.majority(0, 6) == 1);
        assert(!query.majority(1, 5));
        assert(!query.majority(2, 2));
        query.set(3, 2);
        assert(!query.majority(0, 6));
        query.set(4, 2);
        assert(query.majority(1, 5) == 2);
    }

    std::mt19937_64 random(2026071301);
    for(int iteration = 0; iteration < 300; ++iteration){
        const int size = 1 + static_cast<int>(random() % 80);
        std::vector<int> values(static_cast<std::size_t>(size));
        for(int& value: values){
            value = static_cast<int>(random() % 17) - 8;
        }
        RangeMajorityQuery<int> query(values);
        for(int operation = 0; operation < 1000; ++operation){
            if(random() % 3 == 0){
                const int index = static_cast<int>(random() % size);
                const int value = static_cast<int>(random() % 17) - 8;
                values[static_cast<std::size_t>(index)] = value;
                query.set(index, value);
            }else{
                int left = static_cast<int>(random() % (size + 1));
                int right = static_cast<int>(random() % (size + 1));
                if(left > right) std::swap(left, right);
                assert(query.majority(left, right) ==
                       naive_majority(values, left, right));
            }
        }
    }
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size, operation_count;
    if(!(std::cin >> size >> operation_count)){
        self_test();
        return 0;
    }
    std::vector<int> values(static_cast<std::size_t>(size));
    for(int& value: values) std::cin >> value;
    RangeMajorityQuery<int> query(std::move(values));
    while(operation_count--){
        int type, first, second;
        std::cin >> type >> first >> second;
        if(type == 0){
            query.set(first, second);
        }else{
            const auto result = query.majority(first, second);
            std::cout << (result ? *result : -1) << '\n';
        }
    }
}
