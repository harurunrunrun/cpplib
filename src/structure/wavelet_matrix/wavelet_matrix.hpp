#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_WAVELET_MATRIX_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_WAVELET_MATRIX_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include "fully_indexable_dictionary.hpp"

template<
    class T,
    int MAX_SIZE,
    int BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<T>>::digits
>
struct WaveletMatrix{
    static_assert(std::is_integral_v<T>);
    static_assert(MAX_SIZE >= 0);
    using U = std::make_unsigned_t<T>;
    static constexpr int digits = std::numeric_limits<U>::digits;
    static_assert(0 < BIT_WIDTH && BIT_WIDTH <= digits);
    static_assert(!std::is_signed_v<T> || BIT_WIDTH == digits);

private:
    struct State{
        std::array<FullyIndexableDictionary<MAX_SIZE>, BIT_WIDTH> matrix;
        std::array<int, BIT_WIDTH> middle{};
        std::array<U, MAX_SIZE> current{};
        std::array<U, MAX_SIZE> next{};

        explicit State(int n){
            for(auto& dictionary: matrix){
                dictionary = FullyIndexableDictionary<MAX_SIZE>(n);
            }
        }
    };

    int _n = 0;
    std::unique_ptr<State> state;

    static constexpr U sign_mask = std::is_signed_v<T>
        ? U{1} << (digits - 1) : U{0};
    static U encode(T value){ return static_cast<U>(value) ^ sign_mask; }
    static T decode(U value){ return static_cast<T>(value ^ sign_mask); }

    void check_range(int l, int r, const char* message) const{
        if(l < 0 || r < l || _n < r)[[unlikely]] throw std::runtime_error(message);
    }

    int count_less_encoded(int l, int r, U upper) const{
        int result = 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            int bit_index = BIT_WIDTH - 1 - level;
            bool value = (upper >> bit_index) & 1;
            int left_l = state->matrix[level].rank(false, l);
            int left_r = state->matrix[level].rank(false, r);
            if(value){
                result += left_r - left_l;
                l = state->middle[level] + state->matrix[level].rank(true, l);
                r = state->middle[level] + state->matrix[level].rank(true, r);
            }else{
                l = left_l;
                r = left_r;
            }
        }
        return result;
    }

public:
    explicit WaveletMatrix(const std::vector<T>& values):
        _n(static_cast<int>(values.size())), state(nullptr){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        state = std::make_unique<State>(_n);
        for(int k = 0; k < _n; k++){
            U value = encode(values[static_cast<std::size_t>(k)]);
            if constexpr(BIT_WIDTH < digits){
                if((value >> BIT_WIDTH) != 0)[[unlikely]]{
                    throw std::runtime_error("library assertion fault: bit width violation (constructor).");
                }
            }
            state->current[k] = value;
        }
        for(int level = 0; level < BIT_WIDTH; level++){
            int bit_index = BIT_WIDTH - 1 - level;
            for(int k = 0; k < _n; k++){
                if((state->current[k] >> bit_index) & 1) state->matrix[level].set(k);
            }
            state->matrix[level].build();
            int zero = state->matrix[level].rank(false, _n);
            state->middle[level] = zero;
            int left = 0, right = zero;
            for(int k = 0; k < _n; k++){
                if((state->current[k] >> bit_index) & 1){
                    state->next[right++] = state->current[k];
                }else{
                    state->next[left++] = state->current[k];
                }
            }
            for(int k = 0; k < _n; k++) state->current[k] = state->next[k];
        }
    }

    template<std::size_t N>
    explicit WaveletMatrix(const std::array<T, N>& values):
        WaveletMatrix(std::vector<T>(values.begin(), values.end())){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
    }

    WaveletMatrix(const WaveletMatrix&) = delete;
    WaveletMatrix& operator=(const WaveletMatrix&) = delete;
    WaveletMatrix(WaveletMatrix&&) = default;
    WaveletMatrix& operator=(WaveletMatrix&&) = default;

    int size() const{ return _n; }

    T access(int k) const{
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (access).");
        }
        U value = 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            bool bit = state->matrix[level][k];
            if(bit){
                value |= U{1} << (BIT_WIDTH - 1 - level);
                k = state->middle[level] + state->matrix[level].rank(true, k);
            }else{
                k = state->matrix[level].rank(false, k);
            }
        }
        return decode(value);
    }
    T operator[](int k) const{ return access(k); }

    int rank(T value, int r) const{ return rank(value, 0, r); }
    int rank(T value, int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (rank).");
        U key = encode(value);
        for(int level = 0; level < BIT_WIDTH; level++){
            bool bit = (key >> (BIT_WIDTH - 1 - level)) & 1;
            if(bit){
                l = state->middle[level] + state->matrix[level].rank(true, l);
                r = state->middle[level] + state->matrix[level].rank(true, r);
            }else{
                l = state->matrix[level].rank(false, l);
                r = state->matrix[level].rank(false, r);
            }
        }
        return r - l;
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
        for(int level = 0; level < BIT_WIDTH; level++){
            int left_l = state->matrix[level].rank(false, l);
            int left_r = state->matrix[level].rank(false, r);
            int zero = left_r - left_l;
            if(k < zero){
                l = left_l;
                r = left_r;
            }else{
                value |= U{1} << (BIT_WIDTH - 1 - level);
                k -= zero;
                l = state->middle[level] + state->matrix[level].rank(true, l);
                r = state->middle[level] + state->matrix[level].rank(true, r);
            }
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
        return count_less_encoded(l, r, encode(upper));
    }
    int range_freq(int l, int r, T lower, T upper) const{
        check_range(l, r, "library assertion fault: range violation (range_freq).");
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (range_freq).");
        }
        return count_less_encoded(l, r, encode(upper)) -
            count_less_encoded(l, r, encode(lower));
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

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_WAVELET_MATRIX_HPP_INCLUDED
