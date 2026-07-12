// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>
#include "../../src/structure/wavelet_matrix/compressed_weighted_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/functional_wavelet_matrix.hpp"

int main(){
    int input_n, q;
    if(std::cin >> input_n >> q){
        std::vector<int> input_value(static_cast<std::size_t>(input_n));
        std::vector<long long> input_weight(static_cast<std::size_t>(input_n));
        for(int& value: input_value) std::cin >> value;
        for(auto& weight_value: input_weight) std::cin >> weight_value;
        FunctionalWaveletMatrix<int, 512, long long> functional(input_value);
        CompressedWeightedWaveletMatrix<int, long long, 512> weighted(
            input_value, input_weight
        );
        while(q--){
            std::string type;
            int l, r;
            std::cin >> type >> l >> r;
            if(type == "FSUM"){
                std::cout << functional.sum(l, r) << '\n';
            }else if(type == "WSUM"){
                std::cout << weighted.sum(l, r) << '\n';
            }else if(type == "FRANGE" || type == "WRANGE" || type == "FREQ"){
                int lower, upper;
                std::cin >> lower >> upper;
                if(type == "FRANGE") std::cout << functional.range_sum(l, r, lower, upper) << '\n';
                if(type == "WRANGE") std::cout << weighted.range_sum(l, r, lower, upper) << '\n';
                if(type == "FREQ") std::cout << functional.range_freq(l, r, lower, upper) << '\n';
            }else if(type == "FKSMALL" || type == "FKLARGE" ||
                     type == "WKSMALL" || type == "WKLARGE"){
                int k;
                std::cin >> k;
                if(type == "FKSMALL") std::cout << functional.sum_k_smallest(l, r, k) << '\n';
                if(type == "FKLARGE") std::cout << functional.sum_k_largest(l, r, k) << '\n';
                if(type == "WKSMALL") std::cout << weighted.sum_k_smallest(l, r, k) << '\n';
                if(type == "WKLARGE") std::cout << weighted.sum_k_largest(l, r, k) << '\n';
            }else if(type == "MFLOOR"){
                std::cout << functional.median_floor(l, r) << '\n';
            }else if(type == "MCEIL"){
                std::cout << functional.median_ceil(l, r) << '\n';
            }
        }
        return 0;
    }

    constexpr int n = 157;
    std::mt19937 rng(57721);
    std::vector<int> values(n);
    std::vector<long long> weights(n);
    for(int k = 0; k < n; k++){
        values[k] = k * 5 - 380;
        weights[k] = static_cast<int>(rng() % 121) - 40;
    }
    std::shuffle(values.begin(), values.end(), rng);

    FunctionalWaveletMatrix<int, 200, long long> functional(values);
    CompressedWeightedWaveletMatrix<int, long long, 200> weighted(values, weights);

    for(int k = 0; k < n; k++){
        assert(functional[k] == values[k]);
        assert(weighted[k] == values[k]);
    }

    for(int step = 0; step < 3000; step++){
        int l = static_cast<int>(rng() % (n + 1));
        int r = static_cast<int>(rng() % (n + 1));
        if(l > r) std::swap(l, r);
        int x = static_cast<int>(rng() % 901) - 450;
        int y = static_cast<int>(rng() % 901) - 450;
        if(x > y) std::swap(x, y);

        long long value_sum = 0, value_range_sum = 0;
        long long weight_sum = 0, weight_range_sum = 0;
        int count = 0;
        std::vector<int> sorted_values;
        std::vector<std::pair<int, long long>> sorted_weighted;
        for(int k = l; k < r; k++){
            value_sum += values[k];
            weight_sum += weights[k];
            sorted_values.push_back(values[k]);
            sorted_weighted.emplace_back(values[k], weights[k]);
            if(x <= values[k] && values[k] < y){
                count++;
                value_range_sum += values[k];
                weight_range_sum += weights[k];
            }
        }
        std::sort(sorted_values.begin(), sorted_values.end());
        std::sort(sorted_weighted.begin(), sorted_weighted.end());

        assert(functional.sum(l, r) == value_sum);
        assert(functional.range_sum(l, r, x, y) == value_range_sum);
        assert(functional.range_freq(l, r, x, y) == count);
        assert(weighted.sum(l, r) == weight_sum);
        assert(weighted.range_sum(l, r, x, y) == weight_range_sum);
        assert(weighted.range_freq(l, r, x, y) == count);

        if(l != r){
            int take = static_cast<int>(rng() % (sorted_values.size() + 1));
            long long expected_value_small = std::accumulate(
                sorted_values.begin(), sorted_values.begin() + take, 0LL);
            long long expected_value_large = std::accumulate(
                sorted_values.end() - take, sorted_values.end(), 0LL);
            assert(functional.sum_k_smallest(l, r, take) == expected_value_small);
            assert(functional.sum_k_largest(l, r, take) == expected_value_large);

            long long expected_weight_small = 0, expected_weight_large = 0;
            for(int k = 0; k < take; k++) expected_weight_small += sorted_weighted[k].second;
            for(int k = 0; k < take; k++){
                expected_weight_large += sorted_weighted[sorted_weighted.size() - 1 - k].second;
            }
            assert(weighted.sum_k_smallest(l, r, take) == expected_weight_small);
            assert(weighted.sum_k_largest(l, r, take) == expected_weight_large);
            assert(functional.median_floor(l, r) ==
                sorted_values[(sorted_values.size() - 1) / 2]);
            assert(functional.median_ceil(l, r) == sorted_values[sorted_values.size() / 2]);
        }
    }
}
