// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/wavelet_matrix/dynamic_wavelet_matrix.hpp"

int main(){
    int input_n, q;
    if(std::cin >> input_n >> q){
        std::vector<int> input(static_cast<std::size_t>(input_n));
        for(int& value: input) std::cin >> value;
        DynamicWaveletMatrix<int, 512, 32, 32> matrix(input);
        auto print_optional = [](const std::optional<int>& value){
            if(value) std::cout << *value << '\n';
            else std::cout << "NONE\n";
        };
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "SET"){
                int k, value;
                std::cin >> k >> value;
                matrix.set(k, value);
            }else if(type == "ACCESS"){
                int k;
                std::cin >> k;
                std::cout << matrix[k] << '\n';
            }else if(type == "RANK"){
                int value, l, r;
                std::cin >> value >> l >> r;
                std::cout << matrix.rank(value, l, r) << '\n';
            }else if(type == "SELECT"){
                int value, k;
                std::cin >> value >> k;
                std::cout << matrix.select(value, k) << '\n';
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
            }else if(type == "PREV" || type == "NEXT"){
                int l, r, value;
                std::cin >> l >> r >> value;
                if(type == "PREV") print_optional(matrix.prev_value(l, r, value));
                else print_optional(matrix.next_value(l, r, value));
            }
        }
        return 0;
    }

    constexpr int n = 257;
    std::mt19937 rng(20260712);
    std::vector<int> values(n);
    for(int& value: values) value = static_cast<int>(rng() % 2001) - 1000;

    DynamicWaveletMatrix<int, 300, 32, 32> matrix(values);
    for(int k = 0; k < n; k++) assert(matrix[k] == values[k]);

    for(int step = 0; step < 3000; step++){
        if(rng() % 4 == 0){
            int k = static_cast<int>(rng() % n);
            int value = static_cast<int>(rng() % 2001) - 1000;
            values[k] = value;
            matrix.set(k, value);
            assert(matrix[k] == value);
        }

        int l = static_cast<int>(rng() % (n + 1));
        int r = static_cast<int>(rng() % (n + 1));
        if(l > r) std::swap(l, r);
        int x = static_cast<int>(rng() % 2401) - 1200;
        int y = static_cast<int>(rng() % 2401) - 1200;
        if(x > y) std::swap(x, y);

        assert(matrix.rank(x, l, r) ==
            std::count(values.begin() + l, values.begin() + r, x));
        assert(matrix.range_freq(l, r, x) ==
            std::count_if(values.begin() + l, values.begin() + r,
                [&](int value){ return value < x; }));
        assert(matrix.range_freq(l, r, x, y) ==
            std::count_if(values.begin() + l, values.begin() + r,
                [&](int value){ return x <= value && value < y; }));

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

    for(int x = -1050; x <= 1050; x++){
        int occurrence = 0;
        for(int k = 0; k < n; k++){
            if(values[k] == x) assert(matrix.select(x, occurrence++) == k);
        }
        assert(matrix.select(x, occurrence) == n);
    }

    std::vector<unsigned> small = {0, 1, 7, 3, 12, 31, 4, 8};
    DynamicWaveletMatrix<unsigned, 16, 5, 4> small_matrix(small);
    assert(small_matrix.range_freq(0, 8, 32U) == 8);
    small_matrix.set(2, 15U);
    small[2] = 15U;
    for(int k = 0; k < static_cast<int>(small.size()); k++){
        assert(small_matrix[k] == small[static_cast<std::size_t>(k)]);
    }

    bool thrown = false;
    try{
        small_matrix.set(0, 32U);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}
