// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/wavelet_matrix/dynamic_functional_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/dynamic_weighted_wavelet_matrix.hpp"

int main(){
    int input_n, q;
    if(std::cin >> input_n >> q){
        std::vector<int> input_value(static_cast<std::size_t>(input_n));
        std::vector<long long> input_weight(static_cast<std::size_t>(input_n));
        for(int& value: input_value) std::cin >> value;
        for(auto& weight_value: input_weight) std::cin >> weight_value;
        DynamicWeightedWaveletMatrix<int, long long, 512, 32, 24> matrix(
            input_value, input_weight
        );
        auto print_optional = [](const std::optional<int>& value){
            if(value) std::cout << *value << '\n';
            else std::cout << "NONE\n";
        };
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "SET"){
                int k, value;
                long long weight_value;
                std::cin >> k >> value >> weight_value;
                matrix.set(k, value, weight_value);
            }else if(type == "SETV"){
                int k, value;
                std::cin >> k >> value;
                matrix.set_value(k, value);
            }else if(type == "SETW"){
                int k;
                long long weight_value;
                std::cin >> k >> weight_value;
                matrix.set_weight(k, weight_value);
            }else if(type == "GET"){
                int k;
                std::cin >> k;
                std::cout << matrix[k] << ' ' << matrix.weight(k) << '\n';
            }else if(type == "SUM"){
                int l, r;
                std::cin >> l >> r;
                std::cout << matrix.sum(l, r) << '\n';
            }else if(type == "FREQ" || type == "RSUM"){
                int l, r, lower, upper;
                std::cin >> l >> r >> lower >> upper;
                if(type == "FREQ") std::cout << matrix.range_freq(l, r, lower, upper) << '\n';
                else std::cout << matrix.range_sum(l, r, lower, upper) << '\n';
            }else if(type == "KTH" || type == "KLARG"){
                int l, r, k;
                std::cin >> l >> r >> k;
                if(type == "KTH") std::cout << matrix.kth_smallest(l, r, k) << '\n';
                else std::cout << matrix.kth_largest(l, r, k) << '\n';
            }else if(type == "KSMALL" || type == "KLARGE"){
                int l, r, k;
                std::cin >> l >> r >> k;
                if(type == "KSMALL") std::cout << matrix.sum_k_smallest(l, r, k) << '\n';
                else std::cout << matrix.sum_k_largest(l, r, k) << '\n';
            }else if(type == "PREV" || type == "NEXT"){
                int l, r, value;
                std::cin >> l >> r >> value;
                if(type == "PREV") print_optional(matrix.prev_value(l, r, value));
                else print_optional(matrix.next_value(l, r, value));
            }
        }
        return 0;
    }

    constexpr int n = 181;
    std::mt19937 rng(31415926);
    std::vector<int> values(n);
    std::vector<long long> weights(n);
    for(int i = 0; i < n; i++){
        values[i] = static_cast<int>(rng() % 301) - 150;
        weights[i] = static_cast<int>(rng() % 2001) - 1000;
    }

    DynamicWeightedWaveletMatrix<int, long long, 220, 32, 24> matrix(values, weights);
    for(int i = 0; i < n; i++){
        assert(matrix[i] == values[i]);
        assert(matrix.weight(i) == weights[i]);
    }

    for(int step = 0; step < 2500; step++){
        if(rng() % 3 == 0){
            int k = static_cast<int>(rng() % n);
            values[k] = static_cast<int>(rng() % 301) - 150;
            weights[k] = static_cast<int>(rng() % 2001) - 1000;
            matrix.set(k, values[k], weights[k]);
            assert(matrix[k] == values[k]);
            assert(matrix.weight(k) == weights[k]);
        }else if(rng() % 2 == 0){
            int k = static_cast<int>(rng() % n);
            weights[k] = static_cast<int>(rng() % 2001) - 1000;
            matrix.set_weight(k, weights[k]);
            assert(matrix[k] == values[k]);
            assert(matrix.weight(k) == weights[k]);
        }

        int l = static_cast<int>(rng() % (n + 1));
        int r = static_cast<int>(rng() % (n + 1));
        if(l > r) std::swap(l, r);
        int x = static_cast<int>(rng() % 401) - 200;
        int y = static_cast<int>(rng() % 401) - 200;
        if(x > y) std::swap(x, y);

        long long sum = 0;
        long long sum_less = 0;
        long long sum_range = 0;
        int freq_less = 0;
        int freq_range = 0;
        for(int i = l; i < r; i++){
            sum += weights[i];
            if(values[i] < x){
                freq_less++;
                sum_less += weights[i];
            }
            if(x <= values[i] && values[i] < y){
                freq_range++;
                sum_range += weights[i];
            }
        }
        assert(matrix.sum(l, r) == sum);
        assert(matrix.range_freq(l, r, x) == freq_less);
        assert(matrix.range_freq(l, r, x, y) == freq_range);
        assert(matrix.range_sum(l, r, x) == sum_less);
        assert(matrix.range_sum(l, r, x, y) == sum_range);

        if(l != r){
            std::vector<int> order(r - l);
            std::iota(order.begin(), order.end(), l);
            std::stable_sort(order.begin(), order.end(), [&](int lhs, int rhs){
                return values[lhs] < values[rhs];
            });
            int k = static_cast<int>(rng() % (r - l + 1));
            long long small_sum = 0;
            for(int i = 0; i < k; i++) small_sum += weights[order[i]];
            assert(matrix.sum_k_smallest(l, r, k) == small_sum);
            long long large_sum = 0;
            for(int i = 0; i < k; i++) large_sum += weights[order[order.size() - 1 - i]];
            assert(matrix.sum_k_largest(l, r, k) == large_sum);
            if(k < r - l){
                assert(matrix.kth_smallest(l, r, k) == values[order[k]]);
                assert(matrix.kth_largest(l, r, k) == values[order[order.size() - 1 - k]]);
            }
        }
    }

    std::vector<int> functional_values = {3, -1, 4, 1, 5, -9, 2};
    DynamicFunctionalWaveletMatrix<int, 16, long long> functional(functional_values);
    assert(functional.sum(0, 7) == 5);
    assert(functional.range_sum(0, 7, 0) == -10);
    functional.set(1, 6);
    functional_values[1] = 6;
    assert(functional[1] == 6);
    assert(functional.weight(1) == 6);
    assert(functional.sum(0, 7) == 12);

    bool thrown = false;
    try{
        DynamicWeightedWaveletMatrix<unsigned, int, 4, 3, 2> small({0U, 7U}, {1, 2});
        small.set_value(0, 8U);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}
