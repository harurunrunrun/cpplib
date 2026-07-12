// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>
#include "../../src/structure/wavelet_matrix/dynamic_rectangle_sum.hpp"
#include "../../src/structure/wavelet_matrix/dynamic_wavelet_matrix_2d.hpp"
#include "../../src/structure/wavelet_matrix/dynamic_wavelet_matrix_2d_weighted.hpp"

int main(){
    int input_n, q;
    if(std::cin >> input_n >> q){
        std::vector<int> input_x(static_cast<std::size_t>(input_n));
        std::vector<int> input_y(static_cast<std::size_t>(input_n));
        std::vector<long long> input_weight(static_cast<std::size_t>(input_n));
        for(int k = 0; k < input_n; k++){
            std::cin >> input_x[static_cast<std::size_t>(k)]
                >> input_y[static_cast<std::size_t>(k)]
                >> input_weight[static_cast<std::size_t>(k)];
        }
        DynamicWaveletMatrix2D<int, int, 256, 32, 24> plain(input_x, input_y);
        DynamicWaveletMatrix2DWeighted<int, int, long long, 256, 32, 24> weighted(
            input_x, input_y, input_weight
        );
        auto print_optional = [](const std::optional<int>& value){
            if(value) std::cout << *value << '\n';
            else std::cout << "NONE\n";
        };
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "SET"){
                int k, y;
                long long weight_value;
                std::cin >> k >> y >> weight_value;
                plain.set_y(k, y);
                weighted.set(k, y, weight_value);
            }else if(type == "SETW"){
                int k;
                long long weight_value;
                std::cin >> k >> weight_value;
                weighted.set_weight(k, weight_value);
            }else if(type == "GET"){
                int k;
                std::cin >> k;
                std::cout << plain.x(k) << ' ' << plain.y(k) << ' '
                    << weighted.weight(k) << '\n';
            }else if(type == "COUNT" || type == "SUM"){
                int xl, xr, yl, yr;
                std::cin >> xl >> xr >> yl >> yr;
                if(type == "COUNT") std::cout << plain.rectangle_count(xl, xr, yl, yr) << '\n';
                else std::cout << weighted.rectangle_sum(xl, xr, yl, yr) << '\n';
            }else if(type == "KTH"){
                int xl, xr, k;
                std::cin >> xl >> xr >> k;
                std::cout << plain.kth_smallest_y(xl, xr, k) << '\n';
            }else if(type == "PREV" || type == "NEXT"){
                int xl, xr, value;
                std::cin >> xl >> xr >> value;
                if(type == "PREV") print_optional(plain.prev_y(xl, xr, value));
                else print_optional(plain.next_y(xl, xr, value));
            }
        }
        return 0;
    }

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
