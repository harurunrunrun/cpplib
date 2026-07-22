#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_WEIGHTED_WAVELET_MATRIX_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_WEIGHTED_WAVELET_MATRIX_HPP_INCLUDED

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
    class W,
    int MAX_SIZE,
    int BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<T>>::digits
>
struct WeightedWaveletMatrix{
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
        std::array<std::array<W, MAX_SIZE + 1>, BIT_WIDTH + 1> prefix;
        std::array<U, MAX_SIZE> current{};
        std::array<U, MAX_SIZE> next{};
        std::array<W, MAX_SIZE> current_weight{};
        std::array<W, MAX_SIZE> next_weight{};

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
                l = state->middle[level] + (l - left_l);
                r = state->middle[level] + (r - left_r);
            }else{
                l = left_l;
                r = left_r;
            }
        }
        return result;
    }

    W sum_less_encoded(int l, int r, U upper) const{
        W result{};
        for(int level = 0; level < BIT_WIDTH; level++){
            int bit_index = BIT_WIDTH - 1 - level;
            bool value = (upper >> bit_index) & 1;
            int left_l = state->matrix[level].rank(false, l);
            int left_r = state->matrix[level].rank(false, r);
            if(value){
                result += state->prefix[level + 1][left_r] -
                    state->prefix[level + 1][left_l];
                l = state->middle[level] + (l - left_l);
                r = state->middle[level] + (r - left_r);
            }else{
                l = left_l;
                r = left_r;
            }
        }
        return result;
    }

public:
    WeightedWaveletMatrix(const std::vector<T>& values, const std::vector<W>& weights):
        _n(static_cast<int>(values.size())), state(nullptr){
        if(values.size() != weights.size())[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        state = std::make_unique<State>(_n);
        state->prefix[0][0] = W{};
        for(int k = 0; k < _n; k++){
            U value = encode(values[static_cast<std::size_t>(k)]);
            if constexpr(BIT_WIDTH < digits){
                if((value >> BIT_WIDTH) != 0)[[unlikely]]{
                    throw std::runtime_error("library assertion fault: bit width violation (constructor).");
                }
            }
            state->current[k] = value;
            state->current_weight[k] = weights[static_cast<std::size_t>(k)];
            state->prefix[0][k + 1] = state->prefix[0][k] + state->current_weight[k];
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
                    state->next[right] = state->current[k];
                    state->next_weight[right] = state->current_weight[k];
                    right++;
                }else{
                    state->next[left] = state->current[k];
                    state->next_weight[left] = state->current_weight[k];
                    left++;
                }
            }
            state->prefix[level + 1][0] = W{};
            for(int k = 0; k < _n; k++){
                state->current[k] = state->next[k];
                state->current_weight[k] = state->next_weight[k];
                state->prefix[level + 1][k + 1] =
                    state->prefix[level + 1][k] + state->current_weight[k];
            }
        }
    }

    template<std::size_t N>
    WeightedWaveletMatrix(
        const std::array<T, N>& values,
        const std::array<W, N>& weights
    ): WeightedWaveletMatrix(
        std::vector<T>(values.begin(), values.end()),
        std::vector<W>(weights.begin(), weights.end())
    ){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
    }

    WeightedWaveletMatrix(const WeightedWaveletMatrix&) = delete;
    WeightedWaveletMatrix& operator=(const WeightedWaveletMatrix&) = delete;
    WeightedWaveletMatrix(WeightedWaveletMatrix&&) = default;
    WeightedWaveletMatrix& operator=(WeightedWaveletMatrix&&) = default;

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
            int left_l = state->matrix[level].rank(false, l);
            int left_r = state->matrix[level].rank(false, r);
            if(bit){
                l = state->middle[level] + (l - left_l);
                r = state->middle[level] + (r - left_r);
            }else{
                l = left_l;
                r = left_r;
            }
        }
        return r - l;
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
                l = state->middle[level] + (l - left_l);
                r = state->middle[level] + (r - left_r);
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

    W sum(int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (sum).");
        return state->prefix[0][r] - state->prefix[0][l];
    }
    W range_sum(int l, int r, T upper) const{
        check_range(l, r, "library assertion fault: range violation (range_sum).");
        return sum_less_encoded(l, r, encode(upper));
    }
    W range_sum(int l, int r, T lower, T upper) const{
        check_range(l, r, "library assertion fault: range violation (range_sum).");
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (range_sum).");
        }
        return sum_less_encoded(l, r, encode(upper)) -
            sum_less_encoded(l, r, encode(lower));
    }
    W sum_k_smallest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (sum_k_smallest).");
        if(k < 0 || r - l < k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (sum_k_smallest).");
        }
        W result{};
        for(int level = 0; level < BIT_WIDTH; level++){
            int left_l = state->matrix[level].rank(false, l);
            int left_r = state->matrix[level].rank(false, r);
            int zero = left_r - left_l;
            if(k <= zero){
                l = left_l;
                r = left_r;
            }else{
                result += state->prefix[level + 1][left_r] -
                    state->prefix[level + 1][left_l];
                k -= zero;
                l = state->middle[level] + (l - left_l);
                r = state->middle[level] + (r - left_r);
            }
        }
        return result + state->prefix[BIT_WIDTH][l + k] - state->prefix[BIT_WIDTH][l];
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

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_WEIGHTED_WAVELET_MATRIX_HPP_INCLUDED
