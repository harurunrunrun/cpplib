#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<
    class T,
    class W,
    int MAX_SIZE,
    int BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<T>>::digits,
    int BLOCK_SIZE = 512
>
struct DynamicWeightedWaveletMatrix{
    static_assert(std::is_integral_v<T>);
    static_assert(MAX_SIZE >= 0);
    static_assert(BLOCK_SIZE > 0);
    using U = std::make_unsigned_t<T>;
    static constexpr int digits = std::numeric_limits<U>::digits;
    static_assert(0 < BIT_WIDTH && BIT_WIDTH <= digits);
    static_assert(!std::is_signed_v<T> || BIT_WIDTH == digits);

private:
    static constexpr int block_count_max = (MAX_SIZE + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int _n = 0;
    int block_count = 0;
    std::array<U, MAX_SIZE> values{};
    std::array<W, MAX_SIZE> weights{};
    std::array<std::array<U, BLOCK_SIZE>, block_count_max> sorted_value{};
    std::array<std::array<W, BLOCK_SIZE + 1>, block_count_max> sorted_prefix{};
    std::array<W, block_count_max> block_sum{};

    static constexpr U sign_mask = std::is_signed_v<T>
        ? U{1} << (digits - 1) : U{0};
    static U encode(T value){ return static_cast<U>(value) ^ sign_mask; }
    static T decode(U value){ return static_cast<T>(value ^ sign_mask); }

    static bool fits(U value){
        if constexpr(BIT_WIDTH < digits) return (value >> BIT_WIDTH) == 0;
        else return true;
    }

    static U encode_checked(T value, const char* message){
        U result = encode(value);
        if(!fits(result))[[unlikely]] throw std::runtime_error(message);
        return result;
    }

    void check_index(int k, const char* message) const{
        if(k < 0 || _n <= k)[[unlikely]] throw std::runtime_error(message);
    }

    void check_range(int l, int r, const char* message) const{
        if(l < 0 || r < l || _n < r)[[unlikely]] throw std::runtime_error(message);
    }

    void rebuild_block(int block){
        int first = block * BLOCK_SIZE;
        int last = std::min(_n, first + BLOCK_SIZE);
        int size = last - first;
        std::array<std::pair<U, W>, BLOCK_SIZE> buffer{};
        W total{};
        for(int i = 0; i < size; i++){
            int index = first + i;
            buffer[static_cast<std::size_t>(i)] = {
                values[static_cast<std::size_t>(index)],
                weights[static_cast<std::size_t>(index)]
            };
            total += weights[static_cast<std::size_t>(index)];
        }
        for(int i = 1; i < size; i++){
            auto value = buffer[static_cast<std::size_t>(i)];
            int j = i;
            while(j > 0 && value.first < buffer[static_cast<std::size_t>(j - 1)].first){
                buffer[static_cast<std::size_t>(j)] = buffer[static_cast<std::size_t>(j - 1)];
                j--;
            }
            buffer[static_cast<std::size_t>(j)] = value;
        }
        sorted_prefix[static_cast<std::size_t>(block)][0] = W{};
        for(int i = 0; i < size; i++){
            sorted_value[static_cast<std::size_t>(block)][static_cast<std::size_t>(i)] =
                buffer[static_cast<std::size_t>(i)].first;
            sorted_prefix[static_cast<std::size_t>(block)][static_cast<std::size_t>(i + 1)] =
                sorted_prefix[static_cast<std::size_t>(block)][static_cast<std::size_t>(i)] +
                buffer[static_cast<std::size_t>(i)].second;
        }
        block_sum[static_cast<std::size_t>(block)] = total;
    }

    int block_size(int block) const{
        int first = block * BLOCK_SIZE;
        return std::min(_n - first, BLOCK_SIZE);
    }

    int count_less_encoded(int l, int r, U upper) const{
        int result = 0;
        while(l < r && l % BLOCK_SIZE != 0){
            if(values[static_cast<std::size_t>(l)] < upper) result++;
            l++;
        }
        while(l + BLOCK_SIZE <= r){
            int block = l / BLOCK_SIZE;
            auto begin = sorted_value[static_cast<std::size_t>(block)].begin();
            int size = block_size(block);
            result += static_cast<int>(std::lower_bound(begin, begin + size, upper) - begin);
            l += BLOCK_SIZE;
        }
        while(l < r){
            if(values[static_cast<std::size_t>(l)] < upper) result++;
            l++;
        }
        return result;
    }

    int count_less_value(int l, int r, T upper) const{
        U key = encode(upper);
        if(!fits(key)) return r - l;
        return count_less_encoded(l, r, key);
    }

    W sum_less_encoded(int l, int r, U upper) const{
        W result{};
        while(l < r && l % BLOCK_SIZE != 0){
            if(values[static_cast<std::size_t>(l)] < upper){
                result += weights[static_cast<std::size_t>(l)];
            }
            l++;
        }
        while(l + BLOCK_SIZE <= r){
            int block = l / BLOCK_SIZE;
            auto begin = sorted_value[static_cast<std::size_t>(block)].begin();
            int size = block_size(block);
            int count = static_cast<int>(std::lower_bound(begin, begin + size, upper) - begin);
            result += sorted_prefix[static_cast<std::size_t>(block)][static_cast<std::size_t>(count)];
            l += BLOCK_SIZE;
        }
        while(l < r){
            if(values[static_cast<std::size_t>(l)] < upper){
                result += weights[static_cast<std::size_t>(l)];
            }
            l++;
        }
        return result;
    }

    W sum_less_value(int l, int r, T upper) const{
        U key = encode(upper);
        if(!fits(key)) return sum(l, r);
        return sum_less_encoded(l, r, key);
    }

    int count_equal_encoded(int l, int r, U key) const{
        int result = 0;
        while(l < r && l % BLOCK_SIZE != 0){
            if(values[static_cast<std::size_t>(l)] == key) result++;
            l++;
        }
        while(l + BLOCK_SIZE <= r){
            int block = l / BLOCK_SIZE;
            auto begin = sorted_value[static_cast<std::size_t>(block)].begin();
            int size = block_size(block);
            auto range = std::equal_range(begin, begin + size, key);
            result += static_cast<int>(range.second - range.first);
            l += BLOCK_SIZE;
        }
        while(l < r){
            if(values[static_cast<std::size_t>(l)] == key) result++;
            l++;
        }
        return result;
    }

public:
    DynamicWeightedWaveletMatrix() = default;

    DynamicWeightedWaveletMatrix(
        const std::vector<T>& sequence,
        const std::vector<W>& weight_sequence
    ): _n(static_cast<int>(sequence.size())){
        if(sequence.size() != weight_sequence.size())[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        if(sequence.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        block_count = (_n + BLOCK_SIZE - 1) / BLOCK_SIZE;
        for(int i = 0; i < _n; i++){
            values[static_cast<std::size_t>(i)] = encode_checked(
                sequence[static_cast<std::size_t>(i)],
                "library assertion fault: bit width violation (constructor)."
            );
            weights[static_cast<std::size_t>(i)] = weight_sequence[static_cast<std::size_t>(i)];
        }
        for(int block = 0; block < block_count; block++) rebuild_block(block);
    }

    template<std::size_t N>
    DynamicWeightedWaveletMatrix(
        const std::array<T, N>& sequence,
        const std::array<W, N>& weight_sequence
    ): DynamicWeightedWaveletMatrix(
        std::vector<T>(sequence.begin(), sequence.end()),
        std::vector<W>(weight_sequence.begin(), weight_sequence.end())
    ){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
    }

    int size() const{ return _n; }

    T access(int k) const{
        check_index(k, "library assertion fault: range violation (access).");
        return decode(values[static_cast<std::size_t>(k)]);
    }
    T operator[](int k) const{ return access(k); }

    W weight(int k) const{
        check_index(k, "library assertion fault: range violation (weight).");
        return weights[static_cast<std::size_t>(k)];
    }

    void set(int k, T value, W weight_value){
        check_index(k, "library assertion fault: range violation (set).");
        values[static_cast<std::size_t>(k)] = encode_checked(
            value,
            "library assertion fault: bit width violation (set)."
        );
        weights[static_cast<std::size_t>(k)] = weight_value;
        rebuild_block(k / BLOCK_SIZE);
    }

    void set_value(int k, T value){ set(k, value, weight(k)); }
    void set_weight(int k, W weight_value){ set(k, access(k), weight_value); }

    int rank(T value, int r) const{ return rank(value, 0, r); }
    int rank(T value, int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (rank).");
        U key = encode(value);
        if(!fits(key)) return 0;
        return count_equal_encoded(l, r, key);
    }

    int select(T value, int k) const{
        if(k < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (select).");
        }
        if(rank(value, _n) <= k) return _n;
        int low = 0, high = _n;
        while(low < high){
            int mid = low + (high - low) / 2;
            if(rank(value, mid + 1) <= k) low = mid + 1;
            else high = mid;
        }
        return low;
    }

    T kth_smallest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (kth_smallest).");
        if(k < 0 || r - l <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (kth_smallest).");
        }
        U value = 0;
        for(int bit = BIT_WIDTH - 1; bit >= 0; bit--){
            U candidate = value | (U{1} << bit);
            if(count_less_encoded(l, r, candidate) <= k) value = candidate;
        }
        return decode(value);
    }

    T kth_largest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (kth_largest).");
        if(k < 0 || r - l <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (kth_largest).");
        }
        return kth_smallest(l, r, r - l - 1 - k);
    }

    int range_freq(int l, int r, T upper) const{
        check_range(l, r, "library assertion fault: range violation (range_freq).");
        return count_less_value(l, r, upper);
    }

    int range_freq(int l, int r, T lower, T upper) const{
        check_range(l, r, "library assertion fault: range violation (range_freq).");
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (range_freq).");
        }
        return count_less_value(l, r, upper) - count_less_value(l, r, lower);
    }

    W sum(int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (sum).");
        W result{};
        while(l < r && l % BLOCK_SIZE != 0){
            result += weights[static_cast<std::size_t>(l)];
            l++;
        }
        while(l + BLOCK_SIZE <= r){
            result += block_sum[static_cast<std::size_t>(l / BLOCK_SIZE)];
            l += BLOCK_SIZE;
        }
        while(l < r){
            result += weights[static_cast<std::size_t>(l)];
            l++;
        }
        return result;
    }

    W range_sum(int l, int r, T upper) const{
        check_range(l, r, "library assertion fault: range violation (range_sum).");
        return sum_less_value(l, r, upper);
    }

    W range_sum(int l, int r, T lower, T upper) const{
        check_range(l, r, "library assertion fault: range violation (range_sum).");
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (range_sum).");
        }
        return sum_less_value(l, r, upper) - sum_less_value(l, r, lower);
    }

    W sum_k_smallest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (sum_k_smallest).");
        if(k < 0 || r - l < k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (sum_k_smallest).");
        }
        if(k == 0) return W{};
        if(k == r - l) return sum(l, r);
        T border = kth_smallest(l, r, k - 1);
        W result = range_sum(l, r, border);
        int need = k - range_freq(l, r, border);
        U key = encode(border);
        for(int i = l; i < r && need > 0; i++){
            if(values[static_cast<std::size_t>(i)] == key){
                result += weights[static_cast<std::size_t>(i)];
                need--;
            }
        }
        return result;
    }

    W sum_k_largest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (sum_k_largest).");
        if(k < 0 || r - l < k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (sum_k_largest).");
        }
        return sum(l, r) - sum_k_smallest(l, r, r - l - k);
    }

    std::optional<T> prev_value(int l, int r, T upper) const{
        int count = range_freq(l, r, upper);
        if(count == 0) return std::nullopt;
        return kth_smallest(l, r, count - 1);
    }

    std::optional<T> next_value(int l, int r, T lower) const{
        int count = range_freq(l, r, lower);
        if(count == r - l) return std::nullopt;
        return kth_smallest(l, r, count);
    }
};
