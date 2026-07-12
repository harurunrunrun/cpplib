// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <vector>
#include "../../src/structure/wavelet_matrix/wavelet_matrix.hpp"

int main(){
    int input_n, q;
    if(std::cin >> input_n >> q){
        std::vector<int> input(static_cast<std::size_t>(input_n));
        for(int& value: input) std::cin >> value;
        WaveletMatrix<int, 512> matrix(input);
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

    constexpr int n = 173;
    std::mt19937 rng(918273);
    std::vector<int> values(n);
    for(int& value: values) value = static_cast<int>(rng() % 401) - 200;
    WaveletMatrix<int, 200> matrix(values);

    for(int k = 0; k < n; k++) assert(matrix[k] == values[k]);
    for(int step = 0; step < 3000; step++){
        int l = static_cast<int>(rng() % (n + 1));
        int r = static_cast<int>(rng() % (n + 1));
        if(l > r) std::swap(l, r);
        int x = static_cast<int>(rng() % 501) - 250;
        int y = static_cast<int>(rng() % 501) - 250;
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

    for(int x = -220; x <= 220; x++){
        int occurrence = 0;
        for(int k = 0; k < n; k++){
            if(values[k] == x) assert(matrix.select(x, occurrence++) == k);
        }
        assert(matrix.select(x, occurrence) == n);
    }
}
