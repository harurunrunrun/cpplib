#include <algorithm>
#include <cassert>
#include <optional>
#include <random>
#include <tuple>
#include <vector>
#include "../../../src/structure/wavelet_matrix/wavelet_matrix_2d.hpp"
#include "../../../src/structure/wavelet_matrix/wavelet_matrix_2d_weighted.hpp"
#include "../../../src/structure/wavelet_matrix/rectangle_sum.hpp"

inline void run_wavelet_matrix_2d_standalone_check(){
    constexpr int n = 137;
    std::mt19937 rng(271828);
    std::vector<int> xs(n), ys(n);
    std::vector<long long> weights(n);
    std::vector<std::pair<int, int>> points;
    std::vector<std::tuple<int, int, long long>> weighted_points;
    points.reserve(n);
    weighted_points.reserve(n);
    for(int k = 0; k < n; k++){
        xs[k] = static_cast<int>(rng() % 241) - 120;
        ys[k] = static_cast<int>(rng() % 241) - 120;
        weights[k] = static_cast<int>(rng() % 101) - 30;
        points.emplace_back(xs[k], ys[k]);
        weighted_points.emplace_back(xs[k], ys[k], weights[k]);
    }

    WaveletMatrix2D<int, int, 200> plain(xs, ys);
    WaveletMatrix2D<int, int, 200> plain_pairs(points);
    WaveletMatrix2DWeighted<int, int, long long, 200> weighted(xs, ys, weights);
    RectangleSum<int, int, long long, 200> rectangle_sum(weighted_points);

    for(int step = 0; step < 3000; step++){
        int xl = static_cast<int>(rng() % 301) - 150;
        int xr = static_cast<int>(rng() % 301) - 150;
        int yl = static_cast<int>(rng() % 301) - 150;
        int yr = static_cast<int>(rng() % 301) - 150;
        if(xl > xr) std::swap(xl, xr);
        if(yl > yr) std::swap(yl, yr);

        int count = 0;
        long long sum = 0;
        std::vector<int> y_candidates;
        for(int k = 0; k < n; k++){
            if(xl <= xs[k] && xs[k] < xr){
                y_candidates.push_back(ys[k]);
                if(yl <= ys[k] && ys[k] < yr){
                    count++;
                    sum += weights[k];
                }
            }
        }
        assert(plain.rectangle_count(xl, xr, yl, yr) == count);
        assert(plain_pairs.range_freq(xl, xr, yl, yr) == count);
        assert(weighted.rectangle_count(xl, xr, yl, yr) == count);
        assert(weighted.rectangle_sum(xl, xr, yl, yr) == sum);
        assert(rectangle_sum.range_sum(xl, xr, yl, yr) == sum);

        if(!y_candidates.empty()){
            std::sort(y_candidates.begin(), y_candidates.end());
            int k = static_cast<int>(rng() % y_candidates.size());
            assert(plain.kth_smallest_y(xl, xr, k) == y_candidates[k]);
            auto lower = std::lower_bound(y_candidates.begin(), y_candidates.end(), yl);
            std::optional<int> previous = lower == y_candidates.begin()
                ? std::nullopt : std::optional<int>(*std::prev(lower));
            assert(plain.prev_y(xl, xr, yl) == previous);
            std::optional<int> next = lower == y_candidates.end()
                ? std::nullopt : std::optional<int>(*lower);
            assert(plain.next_y(xl, xr, yl) == next);
        }
    }
}
