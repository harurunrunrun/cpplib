#include <algorithm>
#include <cassert>
#include <optional>
#include <random>
#include <string>
#include <vector>
#include "../../../src/structure/wavelet_matrix/compressed_wavelet_matrix.hpp"

inline void run_compressed_wavelet_matrix_standalone_check(){
    constexpr int n = 149;
    std::mt19937 rng(161803);
    std::vector<int> values(n);
    for(int& value: values){
        value = (static_cast<int>(rng() % 401) - 200) * (static_cast<int>(rng() % 5) + 1);
    }
    CompressedWaveletMatrix<int, 200> matrix(values);

    for(int k = 0; k < n; k++) assert(matrix[k] == values[k]);
    for(int step = 0; step < 3000; step++){
        int l = static_cast<int>(rng() % (n + 1));
        int r = static_cast<int>(rng() % (n + 1));
        if(l > r) std::swap(l, r);
        int x = (static_cast<int>(rng() % 501) - 250) * (static_cast<int>(rng() % 5) + 1);
        int y = (static_cast<int>(rng() % 501) - 250) * (static_cast<int>(rng() % 5) + 1);
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
            assert(matrix.quantile(l, r, k) == sorted[k]);
            assert(matrix.kth_largest(l, r, k) == sorted[sorted.size() - 1 - k]);
            assert(matrix.range_min(l, r) == sorted.front());
            assert(matrix.range_max(l, r) == sorted.back());
            assert(matrix.median_floor(l, r) == sorted[(sorted.size() - 1) / 2]);
            assert(matrix.median_ceil(l, r) == sorted[sorted.size() / 2]);

            auto lower = std::lower_bound(sorted.begin(), sorted.end(), x);
            std::optional<int> previous = lower == sorted.begin()
                ? std::nullopt : std::optional<int>(*std::prev(lower));
            assert(matrix.prev_value(l, r, x) == previous);
            std::optional<int> next = lower == sorted.end()
                ? std::nullopt : std::optional<int>(*lower);
            assert(matrix.next_value(l, r, x) == next);
        }
    }

    for(int x = -1000; x <= 1000; x++){
        int occurrence = 0;
        for(int k = 0; k < n; k++){
            if(values[k] == x) assert(matrix.select(x, occurrence++) == k);
        }
        assert(matrix.select(x, occurrence) == n);
    }

    std::vector<std::string> words = {"bbb", "a", "ccc", "a", "bb", "ccc"};
    CompressedWaveletMatrix<std::string, 10> strings(words);
    assert(strings.rank(std::string("a"), 0, 6) == 2);
    assert(strings.rank(std::string("z"), 0, 6) == 0);
    assert(strings.kth_smallest(0, 6, 2) == "bb");
    assert(strings.range_freq(0, 6, std::string("b"), std::string("d")) == 4);
}
