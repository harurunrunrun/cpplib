// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <vector>
#include "../../src/structure/wavelet_matrix/weighted_wavelet_matrix.hpp"

int main(){
    int input_n, q;
    if(std::cin >> input_n >> q){
        std::vector<int> input_value(static_cast<std::size_t>(input_n));
        std::vector<long long> input_weight(static_cast<std::size_t>(input_n));
        for(int& value: input_value) std::cin >> value;
        for(long long& weight: input_weight) std::cin >> weight;
        WeightedWaveletMatrix<int, long long, 512> matrix(input_value, input_weight);
        auto print_optional = [](const std::optional<int>& value){
            if(value) std::cout << *value << '\n';
            else std::cout << "NONE\n";
        };
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "ACCESS"){
                int k;
                std::cin >> k;
                std::cout << matrix[k] << '\n';
            }else if(type == "RANK"){
                int value, l, r;
                std::cin >> value >> l >> r;
                std::cout << matrix.rank(value, l, r) << '\n';

            }else if(type == "KTH" || type == "KLARG"){
                int l, r, k;
                std::cin >> l >> r >> k;
                if(type == "KTH") std::cout << matrix.kth_smallest(l, r, k) << '\n';
                else std::cout << matrix.kth_largest(l, r, k) << '\n';
            }else if(type == "FREQ"){
                int l, r, upper;
                std::cin >> l >> r >> upper;
                std::cout << matrix.range_freq(l, r, upper) << '\n';
            }else if(type == "RANGE"){
                int l, r, lower, upper;
                std::cin >> l >> r >> lower >> upper;
                std::cout << matrix.range_freq(l, r, lower, upper) << '\n';
            }else if(type == "SUM"){
                int l, r;
                std::cin >> l >> r;
                std::cout << matrix.sum(l, r) << '\n';
            }else if(type == "SUMLESS"){
                int l, r, upper;
                std::cin >> l >> r >> upper;
                std::cout << matrix.range_sum(l, r, upper) << '\n';
            }else if(type == "SUMRANGE"){
                int l, r, lower, upper;
                std::cin >> l >> r >> lower >> upper;
                std::cout << matrix.range_sum(l, r, lower, upper) << '\n';
            }else if(type == "PREV" || type == "NEXT"){
                int l, r, value;
                std::cin >> l >> r >> value;
                if(type == "PREV") print_optional(matrix.prev_value(l, r, value));
                else print_optional(matrix.next_value(l, r, value));
            }
        }
        return 0;
    }

    constexpr int n = 151;
    std::mt19937 rng(314159);
    std::vector<int> values(n);
    std::vector<long long> weights(n);
    for(int k = 0; k < n; k++){
        values[k] = static_cast<int>(rng() % 241) - 120;
        weights[k] = static_cast<int>(rng() % 151) - 50;
    }
    WeightedWaveletMatrix<int, long long, 200> matrix(values, weights);

    for(int k = 0; k < n; k++) assert(matrix[k] == values[k]);
    for(int step = 0; step < 3000; step++){
        int l = static_cast<int>(rng() % (n + 1));
        int r = static_cast<int>(rng() % (n + 1));
        if(l > r) std::swap(l, r);
        int x = static_cast<int>(rng() % 301) - 150;
        int y = static_cast<int>(rng() % 301) - 150;
        if(x > y) std::swap(x, y);

        int count_x = 0, less_x = 0, in_range = 0;
        long long total = 0, sum_less = 0, sum_range = 0;
        for(int k = l; k < r; k++){
            total += weights[k];
            if(values[k] == x) count_x++;
            if(values[k] < x){
                less_x++;
                sum_less += weights[k];
            }
            if(x <= values[k] && values[k] < y){
                in_range++;
                sum_range += weights[k];
            }
        }
        assert(matrix.rank(x, l, r) == count_x);
        assert(matrix.range_freq(l, r, x) == less_x);
        assert(matrix.range_freq(l, r, x, y) == in_range);
        assert(matrix.sum(l, r) == total);
        assert(matrix.range_sum(l, r, x) == sum_less);
        assert(matrix.range_sum(l, r, x, y) == sum_range);

        if(l != r){
            std::vector<int> sorted(values.begin() + l, values.begin() + r);
            std::sort(sorted.begin(), sorted.end());
            int k = static_cast<int>(rng() % sorted.size());
            assert(matrix.kth_smallest(l, r, k) == sorted[k]);
            assert(matrix.kth_largest(l, r, k) == sorted[sorted.size() - 1 - k]);

            auto lower = std::lower_bound(sorted.begin(), sorted.end(), x);
            std::optional<int> previous = lower == sorted.begin()
                ? std::nullopt : std::optional<int>(*std::prev(lower));
            assert(matrix.prev_value(l, r, x) == previous);
            std::optional<int> next = lower == sorted.end()
                ? std::nullopt : std::optional<int>(*lower);
            assert(matrix.next_value(l, r, x) == next);
        }
    }
}
