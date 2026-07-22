// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include "../../src/structure/wavelet_matrix/dynamic_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/dynamic_wavelet_matrix.hpp"

namespace{

using Matrix = DynamicWaveletMatrix<int, 4096, 32, long long, 2>;
using BitMatrix = DynamicWaveletMatrix<unsigned, 4096, 6, long long, 2>;

template<class Function>
bool throws_runtime_error(Function&& function){
    try{
        function();
    }catch(const std::runtime_error&){
        return true;
    }
    return false;
}

template<class T>
std::vector<T> sorted_unique(std::vector<T> values){
    std::sort(values.begin(), values.end());
    values.erase(std::unique(values.begin(), values.end()), values.end());
    return values;
}

template<class T>
std::optional<T> optional_min(const std::vector<T>& values){
    if(values.empty()) return std::nullopt;
    return *std::min_element(values.begin(), values.end());
}

template<class T>
std::optional<T> optional_max(const std::vector<T>& values){
    if(values.empty()) return std::nullopt;
    return *std::max_element(values.begin(), values.end());
}

int minimum_prefix_count(std::vector<int> values, long long target, bool largest){
    if(target <= 0) return 0;
    std::sort(values.begin(), values.end());
    if(largest) std::reverse(values.begin(), values.end());
    long long sum = 0;
    for(int i = 0; i < static_cast<int>(values.size()); i++){
        sum += values[static_cast<std::size_t>(i)];
        if(sum >= target) return i + 1;
    }
    return -1;
}

void check_core(const Matrix& matrix, const std::vector<int>& values, std::mt19937& rng){
    const int n = static_cast<int>(values.size());
    assert(matrix.size() == n);
    assert(matrix.empty() == values.empty());
    assert(matrix.bit_size() == (values.empty() ? 0 : 32));

    const auto unique = sorted_unique(values);
    assert(matrix.values() == unique);
    assert(matrix.distinct_size() == static_cast<int>(unique.size()));
    for(int value = -36; value <= 36; value++){
        const auto iterator = std::lower_bound(unique.begin(), unique.end(), value);
        const bool present = iterator != unique.end() && *iterator == value;
        assert(matrix.contains(value) == present);
        const int expected_index = present
            ? static_cast<int>(iterator - unique.begin()) : -1;
        assert(matrix.index_of(value) == expected_index);
    }
    for(int i = 0; i < n; i++){
        assert(matrix.access(i) == values[static_cast<std::size_t>(i)]);
        assert(matrix[i] == values[static_cast<std::size_t>(i)]);
    }

    int l = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
    int r = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
    if(l > r) std::swap(l, r);
    const int x = static_cast<int>(rng() % 81U) - 40;
    int y = static_cast<int>(rng() % 81U) - 40;
    if(y < x) y = x;

    const auto first = values.begin() + l;
    const auto last = values.begin() + r;
    const int equal = static_cast<int>(std::count(first, last, x));
    const int prefix_equal = static_cast<int>(
        std::count(values.begin(), values.begin() + r, x)
    );
    const int all_equal = static_cast<int>(
        std::count(values.begin(), values.end(), x)
    );
    assert(matrix.rank(x, r) == prefix_equal);
    assert(matrix.rank(x, l, r) == equal);
    assert(matrix.count(x) == all_equal);
    assert(matrix.count(x, l, r) == equal);

    std::vector<int> positions;
    for(int i = 0; i < n; i++){
        if(values[static_cast<std::size_t>(i)] == x) positions.push_back(i);
    }
    assert(matrix.select(x, -1) == -1);
    for(int k = 0; k <= static_cast<int>(positions.size()); k++){
        const int expected = k < static_cast<int>(positions.size())
            ? positions[static_cast<std::size_t>(k)] : -1;
        assert(matrix.select(x, k) == expected);
    }

    std::vector<int> ordered(first, last);
    std::sort(ordered.begin(), ordered.end());
    const auto count_if = [&](auto predicate){
        return static_cast<int>(std::count_if(first, last, predicate));
    };
    const auto sum_if = [&](auto predicate){
        long long result = 0;
        for(auto iterator = first; iterator != last; ++iterator){
            if(predicate(*iterator)) result += *iterator;
        }
        return result;
    };

    const int less = count_if([&](int value){ return value < x; });
    const int less_equal = count_if([&](int value){ return value <= x; });
    const int greater = count_if([&](int value){ return value > x; });
    const int greater_equal = count_if([&](int value){ return value >= x; });
    const int in_range = count_if([&](int value){
        return x <= value && value < y;
    });
    assert(matrix.range_freq(l, r, x) == less);
    assert(matrix.range_freq(l, r, x, y) == in_range);
    assert(matrix.count_less(l, r, x) == less);
    assert(matrix.count_less_equal(l, r, x) == less_equal);
    assert(matrix.count_greater(l, r, x) == greater);
    assert(matrix.count_greater_equal(l, r, x) == greater_equal);

    std::vector<int> ge;
    std::vector<int> gt;
    std::vector<int> lt;
    std::vector<int> le;
    for(int value : ordered){
        if(value >= x) ge.push_back(value);
        if(value > x) gt.push_back(value);
        if(value < x) lt.push_back(value);
        if(value <= x) le.push_back(value);
    }
    assert(matrix.next_value(l, r, x) == optional_min(ge));
    assert(matrix.next_value_ge(l, r, x) == optional_min(ge));
    assert(matrix.next_value_gt(l, r, x) == optional_min(gt));
    assert(matrix.prev_value(l, r, x) == optional_max(lt));
    assert(matrix.prev_value_lt(l, r, x) == optional_max(lt));
    assert(matrix.prev_value_le(l, r, x) == optional_max(le));

    assert(matrix.min_value(l, r) == optional_min(ordered));
    assert(matrix.max_value(l, r) == optional_max(ordered));
    if(ordered.empty()){
        assert(!matrix.median_lower(l, r));
        assert(!matrix.median_upper(l, r));
        assert(!matrix.mode(l, r));
    }else{
        const int k = static_cast<int>(
            rng() % static_cast<unsigned>(ordered.size())
        );
        assert(matrix.kth_smallest(l, r, k) == ordered[static_cast<std::size_t>(k)]);
        assert(matrix.kth_largest(l, r, k)
            == ordered[ordered.size() - 1U - static_cast<std::size_t>(k)]);
        assert(matrix.quantile(l, r, k) == ordered[static_cast<std::size_t>(k)]);
        assert(matrix.median_lower(l, r)
            == std::optional<int>(ordered[(ordered.size() - 1U) / 2U]));
        assert(matrix.median_upper(l, r)
            == std::optional<int>(ordered[ordered.size() / 2U]));
    }

    const long long sum_all = std::accumulate(first, last, 0LL);
    const auto sum_less = sum_if([&](int value){ return value < x; });
    const auto sum_less_equal = sum_if([&](int value){ return value <= x; });
    const auto sum_greater = sum_if([&](int value){ return value > x; });
    const auto sum_greater_equal = sum_if([&](int value){ return value >= x; });
    assert(matrix.sum_all(l, r) == sum_all);
    assert(matrix.count_and_sum_less(l, r, x)
        == std::make_pair(less, sum_less));
    assert(matrix.count_and_sum_less_equal(l, r, x)
        == std::make_pair(less_equal, sum_less_equal));
    assert(matrix.count_and_sum_greater(l, r, x)
        == std::make_pair(greater, sum_greater));
    assert(matrix.count_and_sum_greater_equal(l, r, x)
        == std::make_pair(greater_equal, sum_greater_equal));
    assert(matrix.sum_less(l, r, x) == sum_less);
    assert(matrix.sum_less_equal(l, r, x) == sum_less_equal);
    assert(matrix.sum_range(l, r, x, y)
        == sum_if([&](int value){ return x <= value && value < y; }));
    assert(matrix.sum_equal(l, r, x)
        == sum_if([&](int value){ return value == x; }));
    assert(matrix.sum_greater(l, r, x) == sum_greater);
    assert(matrix.sum_greater_equal(l, r, x) == sum_greater_equal);
    const int take = ordered.empty() ? 0 : static_cast<int>(
        rng() % static_cast<unsigned>(ordered.size() + 1U)
    );
    assert(matrix.sum_k_smallest(l, r, take)
        == std::accumulate(ordered.begin(), ordered.begin() + take, 0LL));
    assert(matrix.sum_k_largest(l, r, take)
        == std::accumulate(ordered.end() - take, ordered.end(), 0LL));

    const long long target = static_cast<long long>(
        static_cast<int>(rng() % 161U) - 40
    );
    assert(matrix.min_count_for_sum_smallest_ge(l, r, target)
        == minimum_prefix_count(ordered, target, false));
    assert(matrix.min_count_for_sum_largest_ge(l, r, target)
        == minimum_prefix_count(ordered, target, true));

    std::map<int, int> frequency_map;
    for(int value : ordered) frequency_map[value]++;
    const std::vector<std::pair<int, int>> frequencies(
        frequency_map.begin(), frequency_map.end()
    );
    assert(matrix.list_frequencies(l, r) == frequencies);
    std::vector<std::pair<int, int>> ranged_frequencies;
    for(const auto& entry : frequencies){
        if(x <= entry.first && entry.first < y) ranged_frequencies.push_back(entry);
    }
    assert(matrix.list_frequencies(l, r, x, y) == ranged_frequencies);
    std::vector<int> distinct;
    for(const auto& entry : frequencies) distinct.push_back(entry.first);
    assert(matrix.distinct_values(l, r) == distinct);

    auto frequent = frequencies;
    std::sort(frequent.begin(), frequent.end(), [](const auto& left, const auto& right){
        if(left.second != right.second) return left.second > right.second;
        return left.first < right.first;
    });
    const int top_count = static_cast<int>(rng() % 8U);
    auto expected_top = frequent;
    if(static_cast<int>(expected_top.size()) > top_count){
        expected_top.resize(static_cast<std::size_t>(top_count));
    }
    assert(matrix.top_k_frequent(l, r, top_count) == expected_top);
    assert(matrix.top_k_frequent(l, r, 0).empty());
    if(frequent.empty()) assert(!matrix.mode(l, r));
    else{
        assert(matrix.mode(l, r)
            == std::make_optional(frequent.front()));
    }

    int l2 = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
    int r2 = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
    if(l2 > r2) std::swap(l2, r2);
    std::map<int, int> other;
    for(int i = l2; i < r2; i++){
        other[values[static_cast<std::size_t>(i)]]++;
    }
    std::vector<std::tuple<int, int, int>> intersection;
    for(const auto& [value, frequency] : frequency_map){
        auto iterator = other.find(value);
        if(iterator != other.end()){
            intersection.emplace_back(value, frequency, iterator->second);
        }
    }
    assert(matrix.intersect(l, r, l2, r2) == intersection);
}

unsigned long long transform_value(
    unsigned value,
    unsigned long long mask,
    BitMatrix::BitwiseOperation operation
){
    mask &= 63ULL;
    const unsigned long long raw = static_cast<unsigned long long>(value);
    switch(operation){
        case BitMatrix::BitwiseOperation::Xor: return (raw ^ mask) & 63ULL;
        case BitMatrix::BitwiseOperation::Or: return (raw | mask) & 63ULL;
        case BitMatrix::BitwiseOperation::And: return (raw & mask) & 63ULL;
    }
    return 0;
}

#define CHECK_BITWISE_WRAPPER(name, operation) do { \
    assert(matrix.count_less_##name(l, r, mask, upper) == matrix.count_less_bitwise(l, r, mask, upper, operation)); \
    assert(matrix.range_freq_##name(l, r, mask, lower, upper) == matrix.range_freq_bitwise(l, r, mask, lower, upper, operation)); \
    assert(matrix.count_less_equal_##name(l, r, mask, upper) == matrix.count_less_equal_bitwise(l, r, mask, upper, operation)); \
    assert(matrix.count_equal_##name(l, r, mask, probe) == matrix.count_equal_bitwise(l, r, mask, probe, operation)); \
    assert(matrix.count_greater_##name(l, r, mask, lower) == matrix.count_greater_bitwise(l, r, mask, lower, operation)); \
    assert(matrix.count_greater_equal_##name(l, r, mask, lower) == matrix.count_greater_equal_bitwise(l, r, mask, lower, operation)); \
    assert(matrix.count_and_sum_less_##name(l, r, mask, upper) == matrix.count_and_sum_less_bitwise(l, r, mask, upper, operation)); \
    assert(matrix.count_and_sum_less_equal_##name(l, r, mask, upper) == matrix.count_and_sum_less_equal_bitwise(l, r, mask, upper, operation)); \
    assert(matrix.count_and_sum_range_##name(l, r, mask, lower, upper) == matrix.count_and_sum_range_bitwise(l, r, mask, lower, upper, operation)); \
    assert(matrix.count_and_sum_greater_##name(l, r, mask, lower) == matrix.count_and_sum_greater_bitwise(l, r, mask, lower, operation)); \
    assert(matrix.count_and_sum_greater_equal_##name(l, r, mask, lower) == matrix.count_and_sum_greater_equal_bitwise(l, r, mask, lower, operation)); \
    assert(matrix.sum_less_##name(l, r, mask, upper) == matrix.sum_less_bitwise(l, r, mask, upper, operation)); \
    assert(matrix.sum_less_equal_##name(l, r, mask, upper) == matrix.sum_less_equal_bitwise(l, r, mask, upper, operation)); \
    assert(matrix.sum_equal_##name(l, r, mask, probe) == matrix.sum_equal_bitwise(l, r, mask, probe, operation)); \
    assert(matrix.sum_range_##name(l, r, mask, lower, upper) == matrix.sum_range_bitwise(l, r, mask, lower, upper, operation)); \
    assert(matrix.sum_greater_##name(l, r, mask, lower) == matrix.sum_greater_bitwise(l, r, mask, lower, operation)); \
    assert(matrix.sum_greater_equal_##name(l, r, mask, lower) == matrix.sum_greater_equal_bitwise(l, r, mask, lower, operation)); \
    assert(matrix.rank_##name(probe, r, mask) == matrix.rank_bitwise(probe, r, mask, operation)); \
    assert(matrix.rank_##name(probe, l, r, mask) == matrix.rank_bitwise(probe, l, r, mask, operation)); \
    assert(matrix.count_##name(probe, mask) == matrix.count_bitwise(probe, mask, operation)); \
    assert(matrix.count_##name(probe, l, r, mask) == matrix.count_bitwise(probe, l, r, mask, operation)); \
    assert(matrix.contains_##name(probe, mask) == matrix.contains_bitwise(probe, mask, operation)); \
    assert(matrix.select_##name(probe, occurrence, mask) == matrix.select_bitwise(probe, occurrence, mask, operation)); \
    assert(matrix.values_##name(mask) == matrix.values_bitwise(mask, operation)); \
    assert(matrix.distinct_size_##name(mask) == matrix.distinct_size_bitwise(mask, operation)); \
    assert(matrix.index_of_##name(probe, mask) == matrix.index_of_bitwise(probe, mask, operation)); \
    assert(matrix.list_frequencies_##name(l, r, mask) == matrix.list_frequencies_bitwise(l, r, mask, operation)); \
    assert(matrix.list_frequencies_##name(l, r, lower, upper, mask) == matrix.list_frequencies_bitwise(l, r, lower, upper, mask, operation)); \
    assert(matrix.distinct_values_##name(l, r, mask) == matrix.distinct_values_bitwise(l, r, mask, operation)); \
    assert(matrix.top_k_frequent_##name(l, r, 4, mask) == matrix.top_k_frequent_bitwise(l, r, 4, mask, operation)); \
    assert(matrix.mode_##name(l, r, mask) == matrix.mode_bitwise(l, r, mask, operation)); \
    assert(matrix.intersect_##name(l, r, l2, r2, mask) == matrix.intersect_bitwise(l, r, l2, r2, mask, operation)); \
    if(l < r){ \
        const int index = l; \
        const int kth = (r - l) / 2; \
        assert(matrix.access_##name(index, mask) == matrix.access_bitwise(index, mask, operation)); \
        assert(matrix.kth_smallest_##name(l, r, kth, mask) == matrix.kth_smallest_bitwise(l, r, kth, mask, operation)); \
        assert(matrix.kth_largest_##name(l, r, kth, mask) == matrix.kth_largest_bitwise(l, r, kth, mask, operation)); \
        assert(matrix.quantile_##name(l, r, kth, mask) == matrix.quantile_bitwise(l, r, kth, mask, operation)); \
        assert(matrix.min_value_##name(l, r, mask) == matrix.min_value_bitwise(l, r, mask, operation)); \
        assert(matrix.max_value_##name(l, r, mask) == matrix.max_value_bitwise(l, r, mask, operation)); \
        assert(matrix.median_lower_##name(l, r, mask) == matrix.median_lower_bitwise(l, r, mask, operation)); \
        assert(matrix.median_upper_##name(l, r, mask) == matrix.median_upper_bitwise(l, r, mask, operation)); \
        assert(matrix.next_value_ge_##name(l, r, lower, mask) == matrix.next_value_ge_bitwise(l, r, lower, mask, operation)); \
        assert(matrix.next_value_gt_##name(l, r, lower, mask) == matrix.next_value_gt_bitwise(l, r, lower, mask, operation)); \
        assert(matrix.prev_value_lt_##name(l, r, upper, mask) == matrix.prev_value_lt_bitwise(l, r, upper, mask, operation)); \
        assert(matrix.prev_value_le_##name(l, r, upper, mask) == matrix.prev_value_le_bitwise(l, r, upper, mask, operation)); \
        assert(matrix.next_value_##name(l, r, lower, mask) == matrix.next_value_bitwise(l, r, lower, mask, operation)); \
        assert(matrix.prev_value_##name(l, r, upper, mask) == matrix.prev_value_bitwise(l, r, upper, mask, operation)); \
    } \
} while(false)

void check_bitwise(
    const BitMatrix& matrix,
    const std::vector<unsigned>& values,
    std::mt19937& rng,
    BitMatrix::BitwiseOperation operation
){
    const int n = static_cast<int>(values.size());
    int l = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
    int r = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
    if(l > r) std::swap(l, r);
    int l2 = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
    int r2 = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
    if(l2 > r2) std::swap(l2, r2);
    const unsigned long long mask = static_cast<unsigned long long>(rng() % 128U);
    unsigned long long lower = static_cast<unsigned long long>(rng() % 72U);
    unsigned long long upper = static_cast<unsigned long long>(rng() % 72U);
    if(lower > upper) std::swap(lower, upper);
    const unsigned long long probe = static_cast<unsigned long long>(rng() % 72U);
    const int occurrence = static_cast<int>(rng() % 6U);

    std::vector<std::pair<unsigned long long, unsigned>> transformed;
    for(int i = l; i < r; i++){
        const unsigned source = values[static_cast<std::size_t>(i)];
        transformed.emplace_back(transform_value(source, mask, operation), source);
    }
    const auto count_sum = [&](auto predicate){
        std::pair<int, long long> result{0, 0};
        for(const auto& [value, source] : transformed){
            if(predicate(value)){
                result.first++;
                result.second += source;
            }
        }
        return result;
    };
    const auto less = count_sum([&](auto value){ return value < upper; });
    const auto less_equal = count_sum([&](auto value){ return value <= upper; });
    const auto ranged = count_sum([&](auto value){
        return lower <= value && value < upper;
    });
    const auto greater = count_sum([&](auto value){ return value > lower; });
    const auto greater_equal = count_sum([&](auto value){ return value >= lower; });
    const auto equal = count_sum([&](auto value){ return value == probe; });

    assert(matrix.count_less_bitwise(l, r, mask, upper, operation) == less.first);
    assert(matrix.range_freq_bitwise(l, r, mask, lower, upper, operation)
        == ranged.first);
    assert(matrix.count_less_equal_bitwise(l, r, mask, upper, operation)
        == less_equal.first);
    assert(matrix.count_equal_bitwise(l, r, mask, probe, operation) == equal.first);
    assert(matrix.count_greater_bitwise(l, r, mask, lower, operation)
        == greater.first);
    assert(matrix.count_greater_equal_bitwise(l, r, mask, lower, operation)
        == greater_equal.first);
    assert(matrix.count_and_sum_less_bitwise(l, r, mask, upper, operation) == less);
    assert(matrix.count_and_sum_less_equal_bitwise(l, r, mask, upper, operation)
        == less_equal);
    assert(matrix.count_and_sum_range_bitwise(
        l, r, mask, lower, upper, operation
    ) == ranged);
    assert(matrix.count_and_sum_greater_bitwise(l, r, mask, lower, operation)
        == greater);
    assert(matrix.count_and_sum_greater_equal_bitwise(
        l, r, mask, lower, operation
    ) == greater_equal);
    assert(matrix.sum_less_bitwise(l, r, mask, upper, operation) == less.second);
    assert(matrix.sum_less_equal_bitwise(l, r, mask, upper, operation)
        == less_equal.second);
    assert(matrix.sum_equal_bitwise(l, r, mask, probe, operation) == equal.second);
    assert(matrix.sum_range_bitwise(l, r, mask, lower, upper, operation)
        == ranged.second);
    assert(matrix.sum_greater_bitwise(l, r, mask, lower, operation)
        == greater.second);
    assert(matrix.sum_greater_equal_bitwise(l, r, mask, lower, operation)
        == greater_equal.second);

    std::vector<unsigned long long> ordered;
    std::map<unsigned long long, int> frequencies;
    for(const auto& [value, source] : transformed){
        static_cast<void>(source);
        ordered.push_back(value);
        frequencies[value]++;
    }
    std::sort(ordered.begin(), ordered.end());
    const std::vector<std::pair<unsigned long long, int>> listed(
        frequencies.begin(), frequencies.end()
    );
    assert(matrix.list_frequencies_bitwise(l, r, mask, operation) == listed);
    std::vector<std::pair<unsigned long long, int>> listed_range;
    for(const auto& entry : listed){
        if(lower <= entry.first && entry.first < upper){
            listed_range.push_back(entry);
        }
    }
    assert(matrix.list_frequencies_bitwise(
        l, r, lower, upper, mask, operation
    ) == listed_range);
    assert(matrix.distinct_values_bitwise(l, r, mask, operation)
        == sorted_unique(ordered));

    std::vector<int> positions;
    for(int i = 0; i < n; i++){
        if(transform_value(values[static_cast<std::size_t>(i)], mask, operation)
            == probe){
            positions.push_back(i);
        }
    }
    const int prefix_equal = static_cast<int>(std::count_if(
        values.begin(), values.begin() + r, [&](unsigned value){
            return transform_value(value, mask, operation) == probe;
        }
    ));
    assert(matrix.rank_bitwise(probe, r, mask, operation) == prefix_equal);
    assert(matrix.rank_bitwise(probe, l, r, mask, operation) == equal.first);
    assert(matrix.count_bitwise(probe, mask, operation)
        == static_cast<int>(positions.size()));
    assert(matrix.count_bitwise(probe, l, r, mask, operation) == equal.first);
    assert(matrix.contains_bitwise(probe, mask, operation) == !positions.empty());
    const int selected = occurrence < static_cast<int>(positions.size())
        ? positions[static_cast<std::size_t>(occurrence)] : -1;
    assert(matrix.select_bitwise(probe, occurrence, mask, operation) == selected);
    assert(matrix.select_bitwise(probe, -1, mask, operation) == -1);

    const auto global = matrix.values_bitwise(mask, operation);
    assert(matrix.distinct_size_bitwise(mask, operation)
        == static_cast<int>(global.size()));
    const auto global_iterator = std::lower_bound(
        global.begin(), global.end(), probe
    );
    const int expected_index =
        global_iterator != global.end() && *global_iterator == probe
            ? static_cast<int>(global_iterator - global.begin()) : -1;
    assert(matrix.index_of_bitwise(probe, mask, operation) == expected_index);

    if(ordered.empty()){
        assert(!matrix.min_value_bitwise(l, r, mask, operation));
        assert(!matrix.max_value_bitwise(l, r, mask, operation));
        assert(!matrix.median_lower_bitwise(l, r, mask, operation));
        assert(!matrix.median_upper_bitwise(l, r, mask, operation));
        assert(!matrix.mode_bitwise(l, r, mask, operation));
    }else{
        const int k = static_cast<int>(
            rng() % static_cast<unsigned>(ordered.size())
        );
        assert(matrix.kth_smallest_bitwise(l, r, k, mask, operation)
            == ordered[static_cast<std::size_t>(k)]);
        assert(matrix.kth_largest_bitwise(l, r, k, mask, operation)
            == ordered[ordered.size() - 1U - static_cast<std::size_t>(k)]);
        assert(matrix.quantile_bitwise(l, r, k, mask, operation)
            == ordered[static_cast<std::size_t>(k)]);
        assert(matrix.min_value_bitwise(l, r, mask, operation)
            == std::optional<unsigned long long>(ordered.front()));
        assert(matrix.max_value_bitwise(l, r, mask, operation)
            == std::optional<unsigned long long>(ordered.back()));
        assert(matrix.median_lower_bitwise(l, r, mask, operation)
            == std::optional<unsigned long long>(
                ordered[(ordered.size() - 1U) / 2U]
            ));
        assert(matrix.median_upper_bitwise(l, r, mask, operation)
            == std::optional<unsigned long long>(
                ordered[ordered.size() / 2U]
            ));
        assert(matrix.access_bitwise(l, mask, operation)
            == transform_value(values[static_cast<std::size_t>(l)], mask, operation));
    }

    std::vector<unsigned long long> ge;
    std::vector<unsigned long long> gt;
    std::vector<unsigned long long> lt;
    std::vector<unsigned long long> le;
    for(auto value : ordered){
        if(value >= lower) ge.push_back(value);
        if(value > lower) gt.push_back(value);
        if(value < upper) lt.push_back(value);
        if(value <= upper) le.push_back(value);
    }
    assert(matrix.next_value_ge_bitwise(l, r, lower, mask, operation)
        == optional_min(ge));
    assert(matrix.next_value_gt_bitwise(l, r, lower, mask, operation)
        == optional_min(gt));
    assert(matrix.prev_value_lt_bitwise(l, r, upper, mask, operation)
        == optional_max(lt));
    assert(matrix.prev_value_le_bitwise(l, r, upper, mask, operation)
        == optional_max(le));
    assert(matrix.next_value_bitwise(l, r, lower, mask, operation)
        == optional_min(ge));
    assert(matrix.prev_value_bitwise(l, r, upper, mask, operation)
        == optional_max(lt));

    auto top = listed;
    std::sort(top.begin(), top.end(), [](const auto& left, const auto& right){
        if(left.second != right.second) return left.second > right.second;
        return left.first < right.first;
    });
    if(top.size() > 4U) top.resize(4);
    assert(matrix.top_k_frequent_bitwise(l, r, 4, mask, operation) == top);
    if(top.empty()) assert(!matrix.mode_bitwise(l, r, mask, operation));
    else{
        assert(matrix.mode_bitwise(l, r, mask, operation)
            == std::make_optional(top.front()));
    }

    std::map<unsigned long long, int> second_frequencies;
    for(int i = l2; i < r2; i++){
        second_frequencies[
            transform_value(values[static_cast<std::size_t>(i)], mask, operation)
        ]++;
    }
    std::vector<std::tuple<unsigned long long, int, int>> intersection;
    for(const auto& [value, frequency] : frequencies){
        auto iterator = second_frequencies.find(value);
        if(iterator != second_frequencies.end()){
            intersection.emplace_back(value, frequency, iterator->second);
        }
    }
    assert(matrix.intersect_bitwise(l, r, l2, r2, mask, operation)
        == intersection);

    if(operation == BitMatrix::BitwiseOperation::Xor){
        CHECK_BITWISE_WRAPPER(xor, operation);
    }else if(operation == BitMatrix::BitwiseOperation::Or){
        CHECK_BITWISE_WRAPPER(or, operation);
    }else{
        CHECK_BITWISE_WRAPPER(and, operation);
    }
}

#undef CHECK_BITWISE_WRAPPER

void run_failure_tests(){
    DynamicWaveletMatrix<unsigned, 3, 5, long long, 1> matrix(
        std::vector<unsigned>{1, 2, 3}
    );
    assert(throws_runtime_error([&]{ matrix.insert(0, 4); }));
    assert(throws_runtime_error([&]{ matrix.insert(-1, 4); }));
    assert(throws_runtime_error([&]{ matrix.insert(4, 4); }));
    assert(throws_runtime_error([&]{ static_cast<void>(matrix.erase(-1)); }));
    assert(throws_runtime_error([&]{ static_cast<void>(matrix.erase(3)); }));
    assert(throws_runtime_error([&]{ static_cast<void>(matrix.access(3)); }));
    assert(throws_runtime_error([&]{ matrix.set(3, 0); }));
    assert(throws_runtime_error([&]{ matrix.set(0, 32); }));
    assert(throws_runtime_error([&]{
        static_cast<void>(matrix.rank(1, -1, 2));
    }));
    assert(throws_runtime_error([&]{
        static_cast<void>(matrix.rank(1, 0, 4));
    }));
    assert(throws_runtime_error([&]{
        static_cast<void>(matrix.kth_smallest(0, 3, 3));
    }));
    assert(throws_runtime_error([&]{
        static_cast<void>(matrix.kth_largest(0, 3, -1));
    }));
    assert(throws_runtime_error([&]{
        static_cast<void>(matrix.sum_k_smallest(0, 3, 4));
    }));
    assert(throws_runtime_error([&]{
        static_cast<void>(matrix.range_freq(0, 3, 3U, 2U));
    }));
    assert(throws_runtime_error([&]{
        static_cast<void>(matrix.sum_range(0, 3, 3U, 2U));
    }));
    assert(throws_runtime_error([&]{
        static_cast<void>(matrix.list_frequencies(0, 3, 3U, 2U));
    }));
    assert(throws_runtime_error([&]{
        static_cast<void>(matrix.range_freq_bitwise(
            0, 3, 0, 3, 2, decltype(matrix)::BitwiseOperation::Xor
        ));
    }));
    assert(matrix.select(99, 0) == -1);
    assert(matrix.select(1, -1) == -1);

    assert(throws_runtime_error([]{
        DynamicWaveletMatrix<unsigned, 2, 5, long long, 1> too_large(
            std::vector<unsigned>{1, 2, 3}
        );
        static_cast<void>(too_large);
    }));
    assert(throws_runtime_error([]{
        DynamicWaveletMatrix<unsigned, 3, 5, long long, 1> invalid(
            std::vector<unsigned>{32}
        );
        static_cast<void>(invalid);
    }));

    DynamicWaveletMatrix<unsigned, 4, 5, long long, 1> empty;
    assert(empty.empty());
    empty.insert(0, 7);
    assert(empty.erase(0) == 7);
    assert(empty.empty());
    assert(!empty.min_value(0, 0));
    assert(!empty.max_value(0, 0));
    assert(!empty.next_value(0, 0, 0));
    assert(!empty.prev_value(0, 0, 0));
}

void run_performance_test(){
    constexpr int initial_size = 80000;
    std::vector<unsigned> values(static_cast<std::size_t>(initial_size));
    for(int i = 0; i < initial_size; i++){
        values[static_cast<std::size_t>(i)] = static_cast<unsigned>(
            (static_cast<std::uint64_t>(i) * 48271ULL)
            & ((1ULL << 20) - 1ULL)
        );
    }
    DynamicWaveletMatrix<unsigned, 90000, 20, long long, 2> matrix(values);
    std::uint64_t checksum = 0;
    for(int step = 0; step < 5000; step++){
        const int position = static_cast<int>(
            (static_cast<std::uint64_t>(step) * 7919ULL)
            % static_cast<std::uint64_t>(matrix.size() + 1)
        );
        const unsigned value = static_cast<unsigned>(
            (static_cast<std::uint64_t>(step) * 104729ULL)
            & ((1ULL << 20) - 1ULL)
        );
        matrix.insert(position, value);
        checksum ^= static_cast<std::uint64_t>(matrix.access(position));
        if(step % 2 == 0){
            const int erase_position = static_cast<int>(
                (static_cast<std::uint64_t>(step) * 3571ULL)
                % static_cast<std::uint64_t>(matrix.size())
            );
            checksum ^= static_cast<std::uint64_t>(
                matrix.erase(erase_position)
            );
        }
        const int l = matrix.size() / 4;
        const int r = matrix.size() * 3 / 4;
        checksum ^= static_cast<std::uint64_t>(
            matrix.range_freq(l, r, value)
        );
    }
    assert(matrix.size() == initial_size + 2500);
    assert(checksum != std::numeric_limits<std::uint64_t>::max());
}


} // namespace

int main(){
    int input_n = 0;
    int query_count = 0;
    if(std::cin >> input_n >> query_count){
        std::vector<int> input(static_cast<std::size_t>(input_n));
        for(int& value : input) std::cin >> value;
        Matrix matrix(input);
        const auto print_optional = [](const std::optional<int>& value){
            if(value) std::cout << *value << '\n';
            else std::cout << "NONE\n";
        };
        while(query_count-- > 0){
            std::string type;
            std::cin >> type;
            if(type == "INSERT"){
                int k = 0;
                int value = 0;
                std::cin >> k >> value;
                matrix.insert(k, value);
            }else if(type == "ERASE"){
                int k = 0;
                std::cin >> k;
                std::cout << matrix.erase(k) << '\n';
            }else if(type == "SET"){
                int k = 0;
                int value = 0;
                std::cin >> k >> value;
                matrix.set(k, value);
            }else if(type == "ACCESS"){
                int k = 0;
                std::cin >> k;
                std::cout << matrix[k] << '\n';
            }else if(type == "RANK"){
                int value = 0;
                int l = 0;
                int r = 0;
                std::cin >> value >> l >> r;
                std::cout << matrix.rank(value, l, r) << '\n';
            }else if(type == "SELECT"){
                int value = 0;
                int k = 0;
                std::cin >> value >> k;
                std::cout << matrix.select(value, k) << '\n';
            }else if(type == "KTH" || type == "KLARG"){
                int l = 0;
                int r = 0;
                int k = 0;
                std::cin >> l >> r >> k;
                std::cout << (
                    type == "KTH"
                        ? matrix.kth_smallest(l, r, k)
                        : matrix.kth_largest(l, r, k)
                ) << '\n';
            }else if(type == "FREQ"){
                int l = 0;
                int r = 0;
                int upper = 0;
                std::cin >> l >> r >> upper;
                std::cout << matrix.range_freq(l, r, upper) << '\n';
            }else if(type == "RANGE"){
                int l = 0;
                int r = 0;
                int lower = 0;
                int upper = 0;
                std::cin >> l >> r >> lower >> upper;
                std::cout << matrix.range_freq(l, r, lower, upper) << '\n';
            }else if(type == "PREV" || type == "NEXT"){
                int l = 0;
                int r = 0;
                int value = 0;
                std::cin >> l >> r >> value;
                print_optional(
                    type == "PREV"
                        ? matrix.prev_value(l, r, value)
                        : matrix.next_value(l, r, value)
                );
            }
        }
        return 0;
    }

    std::mt19937 rng(20260722);
    std::vector<int> values;
    Matrix matrix;
    for(int step = 0; step < 2200; step++){
        const unsigned action = static_cast<unsigned>(rng() % 10U);
        if(values.empty() || (action < 3U && values.size() < 700U)){
            const int position = static_cast<int>(
                rng() % static_cast<unsigned>(values.size() + 1U)
            );
            const int value = static_cast<int>(rng() % 65U) - 32;
            matrix.insert(position, value);
            values.insert(values.begin() + position, value);
        }else if(action < 5U){
            const int position = static_cast<int>(
                rng() % static_cast<unsigned>(values.size())
            );
            const int expected = values[static_cast<std::size_t>(position)];
            assert(matrix.erase(position) == expected);
            values.erase(values.begin() + position);
        }else if(action < 7U){
            const int position = static_cast<int>(
                rng() % static_cast<unsigned>(values.size())
            );
            const int value = static_cast<int>(rng() % 65U) - 32;
            matrix.set(position, value);
            values[static_cast<std::size_t>(position)] = value;
        }
        check_core(matrix, values, rng);
    }

    const std::array<int, 5> source = {3, -1, 4, -1, 5};
    Matrix from_array(source);
    Matrix copied(from_array);
    assert(copied.values() == from_array.values());
    Matrix assigned;
    assigned = copied;
    assert(assigned.access(2) == 4);
    Matrix moved(std::move(assigned));
    assert(moved.size() == 5);
    Matrix move_assigned;
    move_assigned = std::move(moved);
    assert(move_assigned.size() == 5);
    move_assigned.build(std::vector<int>{9, 2, 6, 5});
    assert(move_assigned.size() == 4);
    assert(move_assigned.access(0) == 9);

    std::vector<unsigned> bit_values;
    BitMatrix bit_matrix;
    for(int step = 0; step < 700; step++){
        const unsigned action = rng() % 4U;
        if(bit_values.empty()
            || (action == 0U && bit_values.size() < 300U)){
            const int position = static_cast<int>(
                rng() % static_cast<unsigned>(bit_values.size() + 1U)
            );
            const unsigned value = rng() & 63U;
            bit_matrix.insert(position, value);
            bit_values.insert(bit_values.begin() + position, value);
        }else if(action == 1U){
            const int position = static_cast<int>(
                rng() % static_cast<unsigned>(bit_values.size())
            );
            assert(bit_matrix.erase(position)
                == bit_values[static_cast<std::size_t>(position)]);
            bit_values.erase(bit_values.begin() + position);
        }else if(action == 2U){
            const int position = static_cast<int>(
                rng() % static_cast<unsigned>(bit_values.size())
            );
            const unsigned value = rng() & 63U;
            bit_matrix.set(position, value);
            bit_values[static_cast<std::size_t>(position)] = value;
        }
        check_bitwise(
            bit_matrix, bit_values, rng, BitMatrix::BitwiseOperation::Xor
        );
        check_bitwise(
            bit_matrix, bit_values, rng, BitMatrix::BitwiseOperation::Or
        );
        check_bitwise(
            bit_matrix, bit_values, rng, BitMatrix::BitwiseOperation::And
        );
    }

    run_failure_tests();
    run_performance_test();
}
