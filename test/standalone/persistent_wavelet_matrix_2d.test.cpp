// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <optional>
#include <random>
#include <stdexcept>
#include <tuple>
#include <vector>
#include "../../src/structure/wavelet_matrix/partially_persistent_rectangle_sum.hpp"
#include "../../src/structure/wavelet_matrix/partially_persistent_wavelet_matrix_2d.hpp"
#include "../../src/structure/wavelet_matrix/partially_persistent_wavelet_matrix_2d_weighted.hpp"
#include "../../src/structure/wavelet_matrix/persistent_rectangle_sum.hpp"
#include "../../src/structure/wavelet_matrix/persistent_wavelet_matrix_2d.hpp"
#include "../../src/structure/wavelet_matrix/persistent_wavelet_matrix_2d_weighted.hpp"

int main(){
    constexpr int n = 113;
    constexpr int max_version = 500;
    std::mt19937 rng(20260713);
    std::vector<int> xs(n), ys(n);
    std::vector<long long> weights(n);
    std::vector<std::pair<int, int>> points;
    std::vector<std::tuple<int, int, long long>> weighted_points;
    points.reserve(n);
    weighted_points.reserve(n);
    for(int i = 0; i < n; i++){
        xs[i] = static_cast<int>(rng() % 241) - 120;
        ys[i] = static_cast<int>(rng() % 241) - 120;
        weights[i] = static_cast<int>(rng() % 401) - 200;
        points.emplace_back(xs[i], ys[i]);
        weighted_points.emplace_back(xs[i], ys[i], weights[i]);
    }

    PersistentWaveletMatrix2D<int, int, 160, max_version, 32, 20> plain(points);
    PersistentWaveletMatrix2DWeighted<int, int, long long, 160, max_version, 32, 20> weighted(xs, ys, weights);
    PersistentRectangleSum<int, int, long long, 160, max_version, 32, 20> rectangle_sum(weighted_points);
    std::vector<std::vector<int>> y_versions(1, ys);
    std::vector<std::vector<long long>> weight_versions(1, weights);

    for(int q = 0; q < max_version; q++){
        int base = static_cast<int>(rng() % y_versions.size());
        int v_plain, v_weighted, v_rect;
        if(rng() % 5 == 0){
            v_plain = plain.fork(base);
            v_weighted = weighted.fork(base);
            v_rect = rectangle_sum.fork(base);
            y_versions.push_back(y_versions[static_cast<std::size_t>(base)]);
            weight_versions.push_back(weight_versions[static_cast<std::size_t>(base)]);
        }else{
            int k = static_cast<int>(rng() % n);
            int y = static_cast<int>(rng() % 241) - 120;
            long long weight = static_cast<int>(rng() % 401) - 200;
            auto next_y = y_versions[static_cast<std::size_t>(base)];
            auto next_weight = weight_versions[static_cast<std::size_t>(base)];
            next_y[k] = y;
            next_weight[k] = weight;
            v_plain = plain.set_y(base, k, y);
            v_weighted = weighted.set(base, k, y, weight);
            v_rect = rectangle_sum.set(base, k, y, weight);
            y_versions.push_back(std::move(next_y));
            weight_versions.push_back(std::move(next_weight));
        }
        assert(v_plain == static_cast<int>(y_versions.size()) - 1);
        assert(v_weighted == v_plain);
        assert(v_rect == v_plain);

        for(int rep = 0; rep < 8; rep++){
            int version = static_cast<int>(rng() % y_versions.size());
            int xl = static_cast<int>(rng() % 301) - 150;
            int xr = static_cast<int>(rng() % 301) - 150;
            int yl = static_cast<int>(rng() % 301) - 150;
            int yr = static_cast<int>(rng() % 301) - 150;
            if(xl > xr) std::swap(xl, xr);
            if(yl > yr) std::swap(yl, yr);

            int count = 0;
            long long sum = 0;
            std::vector<int> y_candidates;
            for(int i = 0; i < n; i++){
                if(xl <= xs[i] && xs[i] < xr){
                    y_candidates.push_back(y_versions[static_cast<std::size_t>(version)][i]);
                    if(yl <= y_versions[static_cast<std::size_t>(version)][i] &&
                        y_versions[static_cast<std::size_t>(version)][i] < yr
                    ){
                        count++;
                        sum += weight_versions[static_cast<std::size_t>(version)][i];
                    }
                }
            }
            assert(plain.rectangle_count(version, xl, xr, yl, yr) == count);
            assert(weighted.rectangle_count(version, xl, xr, yl, yr) == count);
            assert(weighted.rectangle_sum(version, xl, xr, yl, yr) == sum);
            assert(rectangle_sum.range_sum(version, xl, xr, yl, yr) == sum);

            if(!y_candidates.empty()){
                std::sort(y_candidates.begin(), y_candidates.end());
                int k = static_cast<int>(rng() % y_candidates.size());
                assert(plain.kth_smallest_y(version, xl, xr, k) == y_candidates[k]);
                auto lower = std::lower_bound(y_candidates.begin(), y_candidates.end(), yl);
                std::optional<int> previous = lower == y_candidates.begin()
                    ? std::nullopt : std::optional<int>(*std::prev(lower));
                assert(plain.prev_y(version, xl, xr, yl) == previous);
                std::optional<int> next = lower == y_candidates.end()
                    ? std::nullopt : std::optional<int>(*lower);
                assert(plain.next_y(version, xl, xr, yl) == next);
            }
        }
    }

    PartiallyPersistentWaveletMatrix2D<int, int, 160, max_version, 32, 20> partial_plain(xs, ys);
    PartiallyPersistentWaveletMatrix2DWeighted<int, int, long long, 160, max_version, 32, 20> partial_weighted(xs, ys, weights);
    PartiallyPersistentRectangleSum<int, int, long long, 160, max_version, 32, 20> partial_sum(weighted_points);
    std::vector<std::vector<int>> history_y(1, ys);
    std::vector<std::vector<long long>> history_weight(1, weights);

    for(int q = 0; q < max_version; q++){
        int k = static_cast<int>(rng() % n);
        int y = static_cast<int>(rng() % 241) - 120;
        long long weight = static_cast<int>(rng() % 401) - 200;
        auto next_y = history_y.back();
        auto next_weight = history_weight.back();
        next_y[k] = y;
        next_weight[k] = weight;
        int version = partial_plain.set_y(k, y);
        int weighted_version = partial_weighted.set(k, y, weight);
        int sum_version = partial_sum.set(k, y, weight);
        history_y.push_back(std::move(next_y));
        history_weight.push_back(std::move(next_weight));
        assert(version == static_cast<int>(history_y.size()) - 1);
        assert(weighted_version == version);
        assert(sum_version == version);

        for(int rep = 0; rep < 6; rep++){
            int query_version = static_cast<int>(rng() % history_y.size());
            int xl = static_cast<int>(rng() % 301) - 150;
            int xr = static_cast<int>(rng() % 301) - 150;
            int yl = static_cast<int>(rng() % 301) - 150;
            int yr = static_cast<int>(rng() % 301) - 150;
            if(xl > xr) std::swap(xl, xr);
            if(yl > yr) std::swap(yl, yr);
            int count = 0;
            long long sum = 0;
            for(int i = 0; i < n; i++){
                if(xl <= xs[i] && xs[i] < xr &&
                    yl <= history_y[static_cast<std::size_t>(query_version)][i] &&
                    history_y[static_cast<std::size_t>(query_version)][i] < yr
                ){
                    count++;
                    sum += history_weight[static_cast<std::size_t>(query_version)][i];
                }
            }
            assert(partial_plain.range_freq(query_version, xl, xr, yl, yr) == count);
            assert(partial_weighted.range_sum(query_version, xl, xr, yl, yr) == sum);
            assert(partial_sum.rectangle_sum(query_version, xl, xr, yl, yr) == sum);
        }
    }

    bool thrown = false;
    try{
        plain.set_y(0, n, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}
