#include <algorithm>
#include <cassert>
#include <cstdint>
#include <optional>
#include <random>
#include <vector>
#include "../../../src/structure/wavelet_matrix/wavelet_matrix.hpp"

inline void run_wavelet_matrix_standalone_check(){
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
