// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
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
        for(int k = 0; k < input_n; ++k){
            std::cin >> input_x[static_cast<std::size_t>(k)]
                >> input_y[static_cast<std::size_t>(k)]
                >> input_weight[static_cast<std::size_t>(k)];
        }
        DynamicWaveletMatrix2D<int, int, 8192, 32> plain(input_x, input_y);
        DynamicWaveletMatrix2DWeighted<int, int, long long, 8192, 32> weighted(
            input_x, input_y, input_weight
        );
        auto print_optional = [](const std::optional<int>& value){
            if(value) std::cout << *value << '\n';
            else std::cout << "NONE\n";
        };
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "INSERT"){
                int k, x, y;
                long long weight;
                std::cin >> k >> x >> y >> weight;
                plain.insert(k, x, y);
                weighted.insert(k, x, y, weight);
            }else if(type == "ERASE"){
                int k;
                std::cin >> k;
                const auto plain_value = plain.erase(k);
                const auto weighted_value = weighted.erase(k);
                assert(plain_value.first == std::get<0>(weighted_value));
                assert(plain_value.second == std::get<1>(weighted_value));
            }else if(type == "SET"){
                int k, y;
                long long weight;
                std::cin >> k >> y >> weight;
                plain.set_y(k, y);
                weighted.set(k, y, weight);
            }else if(type == "SETX"){
                int k, x;
                std::cin >> k >> x;
                plain.set_x(k, x);
                weighted.set_x(k, x);
            }else if(type == "SETP"){
                int k, x, y;
                long long weight;
                std::cin >> k >> x >> y >> weight;
                plain.set(k, x, y);
                weighted.set_point(k, x, y, weight);
            }else if(type == "SETW"){
                int k;
                long long weight;
                std::cin >> k >> weight;
                weighted.set_weight(k, weight);
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

    constexpr int maximum_size = 512;
    std::mt19937 rng(11235813);
    std::vector<int> xs;
    std::vector<int> ys;
    std::vector<long long> weights;
    for(int k = 0; k < 151; ++k){
        xs.push_back(static_cast<int>(rng() % 251) - 125);
        ys.push_back(static_cast<int>(rng() % 251) - 125);
        weights.push_back(static_cast<int>(rng() % 301) - 150);
    }
    std::vector<std::pair<int, int>> points;
    std::vector<std::tuple<int, int, long long>> weighted_points;
    for(std::size_t k = 0; k < xs.size(); ++k){
        points.emplace_back(xs[k], ys[k]);
        weighted_points.emplace_back(xs[k], ys[k], weights[k]);
    }

    DynamicWaveletMatrix2D<int, int, maximum_size, 32> plain(xs, ys);
    DynamicWaveletMatrix2D<int, int, maximum_size, 32> plain_pairs(points);
    DynamicWaveletMatrix2DWeighted<int, int, long long, maximum_size, 32> weighted(
        xs, ys, weights);
    DynamicRectangleSum<int, int, long long, maximum_size, 32> rectangle_sum(
        weighted_points);

    auto verify_access = [&]{
        assert(plain.size() == static_cast<int>(xs.size()));
        assert(weighted.size() == static_cast<int>(xs.size()));
        for(int repeat = 0; repeat < 4 && !xs.empty(); ++repeat){
            const int k = static_cast<int>(rng() % xs.size());
            assert(plain.x(k) == xs[static_cast<std::size_t>(k)]);
            assert(plain.y(k) == ys[static_cast<std::size_t>(k)]);
            assert(weighted.x(k) == xs[static_cast<std::size_t>(k)]);
            assert(weighted.y(k) == ys[static_cast<std::size_t>(k)]);
            assert(weighted.weight(k) == weights[static_cast<std::size_t>(k)]);
        }
    };

    for(int step = 0; step < 5000; ++step){
        const int operation = static_cast<int>(rng() % 8);
        if((operation == 0 && xs.size() < maximum_size) || xs.empty()){
            const int k = static_cast<int>(rng() % (xs.size() + 1));
            const int x = static_cast<int>(rng() % 251) - 125;
            const int y = static_cast<int>(rng() % 251) - 125;
            const long long weight = static_cast<int>(rng() % 301) - 150;
            xs.insert(xs.begin() + k, x);
            ys.insert(ys.begin() + k, y);
            weights.insert(weights.begin() + k, weight);
            plain.insert(k, x, y);
            plain_pairs.insert(k, x, y);
            weighted.insert(k, x, y, weight);
            rectangle_sum.insert(k, x, y, weight);
        }else if(operation == 1 && xs.size() > 1){
            const int k = static_cast<int>(rng() % xs.size());
            const auto expected = std::tuple{xs[static_cast<std::size_t>(k)],
                                             ys[static_cast<std::size_t>(k)],
                                             weights[static_cast<std::size_t>(k)]};
            assert(plain.erase(k) == std::pair(std::get<0>(expected), std::get<1>(expected)));
            plain_pairs.erase(k);
            assert(weighted.erase(k) == expected);
            rectangle_sum.erase(k);
            xs.erase(xs.begin() + k);
            ys.erase(ys.begin() + k);
            weights.erase(weights.begin() + k);
        }else if(operation == 2){
            const int k = static_cast<int>(rng() % xs.size());
            const int x = static_cast<int>(rng() % 251) - 125;
            xs[static_cast<std::size_t>(k)] = x;
            plain.set_x(k, x);
            plain_pairs.set_x(k, x);
            weighted.set_x(k, x);
            rectangle_sum.set_x(k, x);
        }else if(operation == 3){
            const int k = static_cast<int>(rng() % xs.size());
            const int x = static_cast<int>(rng() % 251) - 125;
            const int y = static_cast<int>(rng() % 251) - 125;
            const long long weight = static_cast<int>(rng() % 301) - 150;
            xs[static_cast<std::size_t>(k)] = x;
            ys[static_cast<std::size_t>(k)] = y;
            weights[static_cast<std::size_t>(k)] = weight;
            plain.set(k, x, y);
            plain_pairs.set(k, x, y);
            weighted.set_point(k, x, y, weight);
            rectangle_sum.set_point(k, x, y, weight);
        }else if(operation == 4){
            const int k = static_cast<int>(rng() % xs.size());
            const int y = static_cast<int>(rng() % 251) - 125;
            const long long weight = static_cast<int>(rng() % 301) - 150;
            ys[static_cast<std::size_t>(k)] = y;
            weights[static_cast<std::size_t>(k)] = weight;
            plain.set_y(k, y);
            plain_pairs.set_y(k, y);
            weighted.set(k, y, weight);
            rectangle_sum.set(k, y, weight);
        }else if(operation == 5){
            const int k = static_cast<int>(rng() % xs.size());
            const long long weight = static_cast<int>(rng() % 301) - 150;
            weights[static_cast<std::size_t>(k)] = weight;
            weighted.set_weight(k, weight);
            rectangle_sum.set_weight(k, weight);
        }

        verify_access();
        int xl = static_cast<int>(rng() % 321) - 160;
        int xr = static_cast<int>(rng() % 321) - 160;
        int yl = static_cast<int>(rng() % 321) - 160;
        int yr = static_cast<int>(rng() % 321) - 160;
        if(xl > xr) std::swap(xl, xr);
        if(yl > yr) std::swap(yl, yr);

        int count = 0;
        long long sum = 0;
        std::vector<int> y_candidates;
        for(std::size_t k = 0; k < xs.size(); ++k){
            if(xl <= xs[k] && xs[k] < xr){
                y_candidates.push_back(ys[k]);
                if(yl <= ys[k] && ys[k] < yr){
                    ++count;
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
            const int k = static_cast<int>(rng() % y_candidates.size());
            assert(plain.kth_smallest_y(xl, xr, k) == y_candidates[static_cast<std::size_t>(k)]);
            const auto lower = std::lower_bound(y_candidates.begin(), y_candidates.end(), yl);
            const std::optional<int> previous = lower == y_candidates.begin()
                ? std::nullopt : std::optional<int>(*std::prev(lower));
            const std::optional<int> next = lower == y_candidates.end()
                ? std::nullopt : std::optional<int>(*lower);
            assert(plain.prev_y(xl, xr, yl) == previous);
            assert(plain.next_y(xl, xr, yl) == next);
        }
    }

    DynamicWaveletMatrix2D<int, int, 4> empty;
    assert(empty.empty());
    empty.push_back(1, 2);
    assert(empty.pop_back() == std::pair(1, 2));
    bool thrown = false;
    try{ empty.pop_back(); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}
