#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>

template<
    class T,
    int MAX_SIZE,
    int BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<T>>::digits,
    int BLOCK_SIZE = 512
>
struct DynamicWaveletMatrix{
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
    std::array<std::array<U, BLOCK_SIZE>, block_count_max> sorted{};
    std::array<int, block_count_max> block_size{};

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

    static bool query_fits(U value){ return fits(value); }

    void check_index(int k, const char* message) const{
        if(k < 0 || _n <= k)[[unlikely]] throw std::runtime_error(message);
    }

    void check_range(int l, int r, const char* message) const{
        if(l < 0 || r < l || _n < r)[[unlikely]] throw std::runtime_error(message);
    }

    void rebuild_block(int block){
        int first = block * BLOCK_SIZE;
        int last = std::min(_n, first + BLOCK_SIZE);
        block_size[static_cast<std::size_t>(block)] = last - first;
        for(int i = first; i < last; i++){
            sorted[static_cast<std::size_t>(block)][static_cast<std::size_t>(i - first)] =
                values[static_cast<std::size_t>(i)];
        }
        auto begin = sorted[static_cast<std::size_t>(block)].begin();
        std::sort(begin, begin + block_size[static_cast<std::size_t>(block)]);
    }

    int count_less_encoded(int l, int r, U upper) const{
        int result = 0;
        while(l < r && l % BLOCK_SIZE != 0){
            if(values[static_cast<std::size_t>(l)] < upper) result++;
            l++;
        }
        while(l + BLOCK_SIZE <= r){
            int block = l / BLOCK_SIZE;
            auto begin = sorted[static_cast<std::size_t>(block)].begin();
            int size = block_size[static_cast<std::size_t>(block)];
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
        if(!query_fits(key)) return r - l;
        return count_less_encoded(l, r, key);
    }

    int count_equal_encoded(int l, int r, U key) const{
        int result = 0;
        while(l < r && l % BLOCK_SIZE != 0){
            if(values[static_cast<std::size_t>(l)] == key) result++;
            l++;
        }
        while(l + BLOCK_SIZE <= r){
            int block = l / BLOCK_SIZE;
            auto begin = sorted[static_cast<std::size_t>(block)].begin();
            int size = block_size[static_cast<std::size_t>(block)];
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
    DynamicWaveletMatrix() = default;

    explicit DynamicWaveletMatrix(const std::vector<T>& sequence):
        _n(static_cast<int>(sequence.size())){
        if(sequence.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        block_count = (_n + BLOCK_SIZE - 1) / BLOCK_SIZE;
        for(int i = 0; i < _n; i++){
            values[static_cast<std::size_t>(i)] = encode_checked(
                sequence[static_cast<std::size_t>(i)],
                "library assertion fault: bit width violation (constructor)."
            );
        }
        for(int block = 0; block < block_count; block++) rebuild_block(block);
    }

    template<std::size_t N>
    explicit DynamicWaveletMatrix(const std::array<T, N>& sequence):
        DynamicWaveletMatrix(std::vector<T>(sequence.begin(), sequence.end())){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
    }

    int size() const{ return _n; }

    T access(int k) const{
        check_index(k, "library assertion fault: range violation (access).");
        return decode(values[static_cast<std::size_t>(k)]);
    }
    T operator[](int k) const{ return access(k); }

    void set(int k, T value){
        check_index(k, "library assertion fault: range violation (set).");
        U encoded = encode_checked(
            value,
            "library assertion fault: bit width violation (set)."
        );
        values[static_cast<std::size_t>(k)] = encoded;
        rebuild_block(k / BLOCK_SIZE);
    }

    int rank(T value, int r) const{ return rank(value, 0, r); }
    int rank(T value, int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (rank).");
        U key = encode(value);
        if(!query_fits(key)) return 0;
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
