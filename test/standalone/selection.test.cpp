// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "../../src/algorithm/other/sequence/selection.hpp"

std::optional<long long> naive_majority(const std::vector<long long>& values){
    std::unordered_map<long long, int> count;
    for(long long value: values) ++count[value];
    for(const auto& [value, frequency]: count){
        if(values.size() / 2 < static_cast<std::size_t>(frequency)) return value;
    }
    return std::nullopt;
}

void self_test(){
    const std::vector<long long> empty;
    assert(!boyer_moore_majority(empty.begin(), empty.end()));

    {
        const std::vector<long long> original{5, 1, 9, 1, 3, 5, 1, 1, 1};
        auto sorted = original;
        std::sort(sorted.begin(), sorted.end());
        for(std::size_t rank = 0; rank < original.size(); ++rank){
            auto values = original;
            const auto iterator = median_of_medians_select(
                values.begin(), values.end(), rank
            );
            assert(*iterator == sorted[rank]);
        }
        assert(boyer_moore_majority(original.begin(), original.end()) == 1);
    }
    {
        std::vector<int> values{1, 4, 2, 9, 3, 9};
        const auto iterator = median_of_medians_select(
            values.begin(), values.end(), 1, std::greater<>{}
        );
        assert(*iterator == 9);
        assert(!boyer_moore_majority(values.begin(), values.end()));
    }
    {
        std::vector<int> values(1000, 7);
        assert(*median_of_medians_select(
            values.begin(), values.end(), 731
        ) == 7);
        assert(boyer_moore_majority(values.begin(), values.end()) == 7);
    }

    bool thrown = false;
    std::vector<int> one{1};
    try{
        (void)median_of_medians_select(one.begin(), one.end(), 1);
    }catch(const std::out_of_range&){
        thrown = true;
    }
    assert(thrown);

    std::mt19937_64 random(2026071308);
    for(int iteration = 0; iteration < 5000; ++iteration){
        const int size = 1 + static_cast<int>(random() % 200);
        std::vector<long long> values(static_cast<std::size_t>(size));
        for(long long& value: values){
            value = static_cast<long long>(random() % 31) - 15;
        }
        const std::size_t rank = static_cast<std::size_t>(random() % values.size());
        auto sorted = values;
        std::sort(sorted.begin(), sorted.end());
        auto work = values;
        assert(*median_of_medians_select(
            work.begin(), work.end(), rank
        ) == sorted[rank]);
        assert(boyer_moore_majority(values.begin(), values.end())
            == naive_majority(values));
    }

    constexpr int large_size = 500000;
    std::vector<int> increasing(static_cast<std::size_t>(large_size));
    for(int index = 0; index < large_size; ++index){
        increasing[static_cast<std::size_t>(index)] = index;
    }
    assert(*median_of_medians_select(
        increasing.begin(), increasing.end(), large_size / 2
    ) == large_size / 2);
    std::reverse(increasing.begin(), increasing.end());
    assert(*median_of_medians_select(
        increasing.begin(), increasing.end(), large_size / 3
    ) == large_size / 3);
}

int main(){
    int size;
    std::size_t rank;
    if(!(std::cin >> size >> rank)){
        self_test();
        return 0;
    }
    std::vector<long long> values(static_cast<std::size_t>(size));
    for(long long& value: values) std::cin >> value;
    const auto majority = boyer_moore_majority(values.begin(), values.end());
    const auto selected = median_of_medians_select(
        values.begin(), values.end(), rank
    );
    std::cout << *selected << '\n';
    if(majority) std::cout << *majority << '\n';
    else std::cout << "NONE\n";
}
