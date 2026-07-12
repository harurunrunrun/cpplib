// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <optional>
#include <random>
#include <stdexcept>
#include <tuple>
#include <vector>
#include "../../src/structure/wavelet_matrix/dynamic_rectangle_sum.hpp"
#include "../../src/structure/wavelet_matrix/dynamic_wavelet_matrix_2d.hpp"
#include "../../src/structure/wavelet_matrix/dynamic_wavelet_matrix_2d_weighted.hpp"

int main(){
    constexpr int n = 151;
    std::mt19937 rng(11235813);
    std::vector<int> xs(n), ys(n);
    std::vector<long long> weights(n);
    std::vector<std::pair<int, int>> points;
    std::vector<std::tuple<int, int, long long>> weighted_points;
    points.reserve(n);
    weighted_points.reserve(n);
    for(int k = 0; k < n; k++){
        xs[k] = static_cast<int>(rng() % 251) - 125;
        ys[k] = static_cast<int>(rng() % 251) - 125;
        weights[k] = static_cast<int>(rng() % 301) - 150;
        points.emplace_back(xs[k], ys[k]);
        weighted_points.emplace_back(xs[k], ys[k], weights[k]);
    }

    DynamicWaveletMatrix2D<int, int, 200, 32, 24> plain(xs, ys);
    DynamicWaveletMatrix2D<int, int, 200, 32, 24> plain_pairs(points);
    DynamicWaveletMatrix2DWeighted<int, int, long long, 200, 32, 24> weighted(
        xs, ys, weights);
    DynamicRectangleSum<int, int, long long, 200, 32, 24> rectangle_sum(weighted_points);

    for(int k = 0; k < n; k++){
        assert(plain.x(k) == xs[k]);
        assert(plain.y(k) == ys[k]);
        assert(weighted.x(k) == xs[k]);
        assert(weighted.y(k) == ys[k]);
        assert(weighted.weight(k) == weights[k]);
    }

    for(int step = 0; step < 2500; step++){
        if(rng() % 3 == 0){
            int k = static_cast<int>(rng() % n);
            int y = static_cast<int>(rng() % 251) - 125;
            long long weight = static_cast<int>(rng() % 301) - 150;
            ys[k] = y;
            weights[k] = weight;
            plain.set_y(k, y);
            plain_pairs.set_y(k, y);
            weighted.set(k, y, weight);
            rectangle_sum.set(k, y, weight);
            assert(plain.y(k) == y);
            assert(weighted.y(k) == y);
            assert(weighted.weight(k) == weight);
        }else if(rng() % 2 == 0){
            int k = static_cast<int>(rng() % n);
            long long weight = static_cast<int>(rng() % 301) - 150;
            weights[k] = weight;
            weighted.set_weight(k, weight);
            rectangle_sum.set_weight(k, weight);
            assert(weighted.weight(k) == weight);
        }

        int xl = static_cast<int>(rng() % 321) - 160;
        int xr = static_cast<int>(rng() % 321) - 160;
        int yl = static_cast<int>(rng() % 321) - 160;
        int yr = static_cast<int>(rng() % 321) - 160;
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

    bool thrown = false;
    try{
        plain.set_y(n, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}
