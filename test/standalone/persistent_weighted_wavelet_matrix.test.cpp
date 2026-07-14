// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/wavelet_matrix/detail/persistent_weighted_wavelet_matrix_base.hpp"
#include "../../src/structure/wavelet_matrix/partially_persistent_functional_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/partially_persistent_weighted_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/persistent_functional_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/persistent_weighted_wavelet_matrix.hpp"

int main(){
    int input_n, input_q;
    if(std::cin >> input_n >> input_q){
        std::vector<int> input_value(static_cast<std::size_t>(input_n));
        std::vector<long long> input_weight(static_cast<std::size_t>(input_n));
        for(int& value: input_value) std::cin >> value;
        for(long long& weight: input_weight) std::cin >> weight;
        PersistentWeightedWaveletMatrix<
            int, long long, 128, 700, 32, 20
        > matrix(input_value, input_weight);
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "SET"){
                int version, k, value;
                long long weight;
                std::cin >> version >> k >> value >> weight;
                std::cout << matrix.set(
                    version, k, value, weight
                ) << '\n';
            }else if(type == "SETV"){
                int version, k, value;
                std::cin >> version >> k >> value;
                std::cout << matrix.set_value(version, k, value) << '\n';
            }else if(type == "SETW"){
                int version, k;
                long long weight;
                std::cin >> version >> k >> weight;
                std::cout << matrix.set_weight(version, k, weight) << '\n';
            }else if(type == "FORK"){
                int version;
                std::cin >> version;
                std::cout << matrix.fork(version) << '\n';
            }else if(type == "SUM"){
                int version, l, r;
                std::cin >> version >> l >> r;
                std::cout << matrix.sum(version, l, r) << '\n';
            }else if(type == "FREQ"){
                int version, l, r, lower, upper;
                std::cin >> version >> l >> r >> lower >> upper;
                std::cout << matrix.range_freq(
                    version, l, r, lower, upper
                ) << '\n';
            }else if(type == "RSUM"){
                int version, l, r, lower, upper;
                std::cin >> version >> l >> r >> lower >> upper;
                std::cout << matrix.range_sum(
                    version, l, r, lower, upper
                ) << '\n';
            }else if(type == "KTH"){
                int version, l, r, k;
                std::cin >> version >> l >> r >> k;
                std::cout << matrix.kth_smallest(version, l, r, k) << '\n';
            }else if(type == "KSMALL"){
                int version, l, r, k;
                std::cin >> version >> l >> r >> k;
                std::cout << matrix.sum_k_smallest(
                    version, l, r, k
                ) << '\n';
            }
        }
        return 0;
    }
    constexpr int n = 139;
    constexpr int max_version = 800;
    std::mt19937 rng(987654321);
    std::vector<int> initial(n);
    std::vector<long long> initial_weight(n);
    for(int i = 0; i < n; i++){
        initial[i] = static_cast<int>(rng() % 301) - 150;
        initial_weight[i] = static_cast<int>(rng() % 2001) - 1000;
    }

    PersistentWeightedWaveletMatrix<int, long long, 180, max_version, 32, 20> persistent(
        initial, initial_weight);
    std::vector<std::vector<int>> values(1, initial);
    std::vector<std::vector<long long>> weights(1, initial_weight);

    for(int q = 0; q < max_version; q++){
        int base = static_cast<int>(rng() % values.size());
        int version;
        if(rng() % 6 == 0){
            version = persistent.fork(base);
            values.push_back(values[static_cast<std::size_t>(base)]);
            weights.push_back(weights[static_cast<std::size_t>(base)]);
        }else{
            int k = static_cast<int>(rng() % n);
            int value = static_cast<int>(rng() % 301) - 150;
            long long weight = static_cast<int>(rng() % 2001) - 1000;
            auto next_value = values[static_cast<std::size_t>(base)];
            auto next_weight = weights[static_cast<std::size_t>(base)];
            next_value[k] = value;
            next_weight[k] = weight;
            version = persistent.set(base, k, value, weight);
            values.push_back(std::move(next_value));
            weights.push_back(std::move(next_weight));
        }
        assert(version == static_cast<int>(values.size()) - 1);

        for(int rep = 0; rep < 8; rep++){
            int v = static_cast<int>(rng() % values.size());
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
                sum += weights[static_cast<std::size_t>(v)][i];
                if(values[static_cast<std::size_t>(v)][i] < x){
                    freq_less++;
                    sum_less += weights[static_cast<std::size_t>(v)][i];
                }
                if(x <= values[static_cast<std::size_t>(v)][i] &&
                    values[static_cast<std::size_t>(v)][i] < y
                ){
                    freq_range++;
                    sum_range += weights[static_cast<std::size_t>(v)][i];
                }
            }
            assert(persistent.sum(v, l, r) == sum);
            assert(persistent.range_freq(v, l, r, x) == freq_less);
            assert(persistent.range_freq(v, l, r, x, y) == freq_range);
            assert(persistent.range_sum(v, l, r, x) == sum_less);
            assert(persistent.range_sum(v, l, r, x, y) == sum_range);

            if(l != r){
                std::vector<int> order(r - l);
                std::iota(order.begin(), order.end(), l);
                std::stable_sort(order.begin(), order.end(), [&](int lhs, int rhs){
                    return values[static_cast<std::size_t>(v)][lhs] <
                        values[static_cast<std::size_t>(v)][rhs];
                });
                int k = static_cast<int>(rng() % (r - l + 1));
                long long small_sum = 0;
                for(int i = 0; i < k; i++) small_sum += weights[static_cast<std::size_t>(v)][order[i]];
                assert(persistent.sum_k_smallest(v, l, r, k) == small_sum);
                if(k < r - l){
                    assert(persistent.kth_smallest(v, l, r, k) ==
                        values[static_cast<std::size_t>(v)][order[k]]);
                }
            }
        }
    }

    PartiallyPersistentWeightedWaveletMatrix<int, long long, 180, max_version, 32, 20> partial(
        initial, initial_weight);
    std::vector<std::vector<int>> history_value(1, initial);
    std::vector<std::vector<long long>> history_weight(1, initial_weight);
    for(int q = 0; q < max_version; q++){
        int k = static_cast<int>(rng() % n);
        int value = static_cast<int>(rng() % 301) - 150;
        long long weight = static_cast<int>(rng() % 2001) - 1000;
        auto next_value = history_value.back();
        auto next_weight = history_weight.back();
        next_value[k] = value;
        next_weight[k] = weight;
        int version = partial.set(k, value, weight);
        history_value.push_back(std::move(next_value));
        history_weight.push_back(std::move(next_weight));
        assert(version == static_cast<int>(history_value.size()) - 1);

        for(int rep = 0; rep < 6; rep++){
            int v = static_cast<int>(rng() % history_value.size());
            int l = static_cast<int>(rng() % (n + 1));
            int r = static_cast<int>(rng() % (n + 1));
            if(l > r) std::swap(l, r);
            int x = static_cast<int>(rng() % 401) - 200;
            long long expected = 0;
            for(int i = l; i < r; i++){
                if(history_value[static_cast<std::size_t>(v)][i] < x){
                    expected += history_weight[static_cast<std::size_t>(v)][i];
                }
            }
            assert(partial.range_sum(v, l, r, x) == expected);
        }
    }

    std::vector<int> f = {3, -1, 4, 1, 5, -9, 2};
    PersistentFunctionalWaveletMatrix<int, 16, 8, long long> functional(f);
    int f_version = functional.set(0, 1, 6);
    assert(functional.sum(0, 0, 7) == 5);
    assert(functional.sum(f_version, 0, 7) == 12);
    PartiallyPersistentFunctionalWaveletMatrix<int, 16, 8, long long> partial_functional(f);
    int pf_version = partial_functional.set(1, 6);
    assert(partial_functional.sum(pf_version, 0, 7) == 12);

    bool thrown = false;
    try{
        PersistentWeightedWaveletMatrix<unsigned, int, 4, 3, 3, 2> small({0U, 7U}, {1, 2});
        small.set_value(0, 0, 8U);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}
