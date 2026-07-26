#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DYNAMIC_DYNAMIC_WEIGHTED_WAVELET_MATRIX_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DYNAMIC_DYNAMIC_WEIGHTED_WAVELET_MATRIX_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "../detail/mutable_btree_bit_sequence.hpp"

template<
    class T,
    class W,
    int MAX_SIZE,
    int BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<T>>::digits
>
struct DynamicWeightedWaveletMatrix{
    static_assert(std::is_integral_v<T>);
    static_assert(MAX_SIZE >= 0);
    using U = std::make_unsigned_t<T>;
    static constexpr int digits = std::numeric_limits<U>::digits;
    static_assert(0 < BIT_WIDTH && BIT_WIDTH <= digits);
    static_assert(!std::is_signed_v<T> || BIT_WIDTH == digits);

private:
    using DynamicWeightedBitVector = wavelet_matrix_detail::MutableBTreeBitSequence<W, W>;

    int _n = 0;
    std::array<DynamicWeightedBitVector, static_cast<std::size_t>(BIT_WIDTH)> bit_vectors{};
    std::array<int, static_cast<std::size_t>(BIT_WIDTH)> zero_count{};

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

    U access_encoded(int k) const{
        U value = 0;
        int position = k;
        for(int level = 0; level < BIT_WIDTH; level++){
            auto [bit, ignored_weight] =
                bit_vectors[static_cast<std::size_t>(level)].access(position);
            (void)ignored_weight;
            value = static_cast<U>((value << 1) | (bit ? U{1} : U{0}));
            const int ones_before =
                bit_vectors[static_cast<std::size_t>(level)].rank(true, position);
            position = bit
                ? zero_count[static_cast<std::size_t>(level)] + ones_before
                : position - ones_before;
        }
        return value;
    }

    int count_less_encoded(int l, int r, U upper) const{
        int result = 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto stats = bits.range_stats(l, r);
            const int zeros = (r - l) - stats.ones;
            const bool bit =
                static_cast<bool>((upper >> (BIT_WIDTH - 1 - level)) & U{1});
            const int ones_l = bits.rank(true, l);
            const int ones_r = ones_l + stats.ones;
            if(bit){
                result += zeros;
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + ones_r;
            }else{
                l -= ones_l;
                r = l + zeros;
            }
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
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto stats = bits.range_stats(l, r);
            const int zeros = (r - l) - stats.ones;
            const bool bit =
                static_cast<bool>((upper >> (BIT_WIDTH - 1 - level)) & U{1});
            const int ones_l = bits.rank(true, l);
            const int ones_r = ones_l + stats.ones;
            if(bit){
                result += stats.zero_sum;
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + ones_r;
            }else{
                l -= ones_l;
                r = l + zeros;
            }
        }
        return result;
    }

    W sum_less_value(int l, int r, T upper) const{
        U key = encode(upper);
        if(!fits(key)) return sum(l, r);
        return sum_less_encoded(l, r, key);
    }

public:
    DynamicWeightedWaveletMatrix() = default;

    DynamicWeightedWaveletMatrix(
        const std::vector<T>& sequence,
        const std::vector<W>& weight_sequence
    ): _n(static_cast<int>(sequence.size())){
        if(sequence.size() != weight_sequence.size() ||
           sequence.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        std::vector<U> current_value(sequence.size());
        std::vector<W> current_weight = weight_sequence;
        for(std::size_t i = 0; i < sequence.size(); i++){
            current_value[i] = encode_checked(
                sequence[i], "library assertion fault: bit width violation (constructor)."
            );
        }
        for(int level = 0; level < BIT_WIDTH; level++){
            const int shift = BIT_WIDTH - 1 - level;
            std::vector<unsigned char> bits(current_value.size());
            std::vector<U> next_value;
            std::vector<W> next_weight;
            next_value.reserve(current_value.size());
            next_weight.reserve(current_weight.size());
            for(std::size_t i = 0; i < current_value.size(); i++){
                bits[i] =
                    static_cast<unsigned char>((current_value[i] >> shift) & U{1});
            }
            bit_vectors[static_cast<std::size_t>(level)].assign(bits, current_weight);
            for(int target = 0; target < 2; target++){
                for(std::size_t i = 0; i < current_value.size(); i++){
                    if(bits[i] == target){
                        next_value.push_back(current_value[i]);
                        next_weight.push_back(current_weight[i]);
                    }
                }
                if(target == 0){
                    zero_count[static_cast<std::size_t>(level)] =
                        static_cast<int>(next_value.size());
                }
            }
            current_value.swap(next_value);
            current_weight.swap(next_weight);
        }
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

    DynamicWeightedWaveletMatrix(
        const DynamicWeightedWaveletMatrix&
    ) = default;
    DynamicWeightedWaveletMatrix& operator=(
        const DynamicWeightedWaveletMatrix&
    ) = default;
    DynamicWeightedWaveletMatrix(
        DynamicWeightedWaveletMatrix&&
    ) noexcept = default;
    DynamicWeightedWaveletMatrix& operator=(
        DynamicWeightedWaveletMatrix&&
    ) noexcept = default;

    int size() const{ return _n; }

    T access(int k) const{
        check_index(k, "library assertion fault: range violation (access).");
        return decode(access_encoded(k));
    }
    T operator[](int k) const{ return access(k); }

    W weight(int k) const{
        check_index(k, "library assertion fault: range violation (weight).");
        return bit_vectors[0].access(k).payload;
    }

    void insert(int position, T value, const W& weight_value){
        if(position < 0 || _n < position || _n == MAX_SIZE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (insert).");
        }
        U encoded = encode_checked(
            value, "library assertion fault: bit width violation (insert)."
        );
        int current = position;
        for(int level = 0; level < BIT_WIDTH; level++){
            auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            const int ones_before = bits.rank(true, current);
            const int old_zeros = zero_count[static_cast<std::size_t>(level)];
            const bool bit =
                static_cast<bool>((encoded >> (BIT_WIDTH - 1 - level)) & U{1});
            bits.insert(current, bit, weight_value);
            if(bit){
                current = old_zeros + ones_before;
            }else{
                current -= ones_before;
                zero_count[static_cast<std::size_t>(level)]++;
            }
        }
        _n++;
    }

    std::pair<T, W> erase(int position){
        check_index(position, "library assertion fault: range violation (erase).");
        U encoded = 0;
        W erased_weight{};
        int current = position;
        for(int level = 0; level < BIT_WIDTH; level++){
            auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto [bit, weight_value] = bits.access(current);
            if(level == 0) erased_weight = weight_value;
            encoded = static_cast<U>((encoded << 1) | (bit ? U{1} : U{0}));
            const int ones_before = bits.rank(true, current);
            const int old_zeros = zero_count[static_cast<std::size_t>(level)];
            const int next =
                bit ? old_zeros + ones_before : current - ones_before;
            bits.erase(current);
            if(!bit) zero_count[static_cast<std::size_t>(level)]--;
            current = next;
        }
        _n--;
        return {decode(encoded), erased_weight};
    }

    void push_back(T value, const W& weight_value){
        insert(_n, value, weight_value);
    }

    std::pair<T, W> pop_back(){
        if(_n == 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (pop_back).");
        }
        return erase(_n - 1);
    }

    void set(int k, T value, const W& weight_value){
        check_index(k, "library assertion fault: range violation (set).");
        encode_checked(value, "library assertion fault: bit width violation (set).");
        erase(k);
        insert(k, value, weight_value);
    }

    void set_value(int k, T value){ set(k, value, weight(k)); }

    void set_weight(int k, const W& weight_value){
        check_index(k, "library assertion fault: range violation (set_weight).");
        int current = k;
        for(int level = 0; level < BIT_WIDTH; level++){
            auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            const int ones_before = bits.rank(true, current);
            const bool bit = bits.set_payload(current, weight_value);
            current = bit
                ? zero_count[static_cast<std::size_t>(level)] + ones_before
                : current - ones_before;
        }
    }

    int rank(T value, int r) const{ return rank(value, 0, r); }

    int rank(T value, int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (rank).");
        U encoded = encode(value);
        if(!fits(encoded)) return 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            const bool bit =
                static_cast<bool>((encoded >> (BIT_WIDTH - 1 - level)) & U{1});
            const int ones_l = bits.rank(true, l);
            const int ones_r = bits.rank(true, r);
            if(bit){
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + ones_r;
            }else{
                l -= ones_l;
                r -= ones_r;
            }
        }
        return r - l;
    }

    int select(T value, int k) const{
        if(k < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (select).");
        }
        U encoded = encode(value);
        if(!fits(encoded)) return _n;
        int l = 0, r = _n;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            const bool bit =
                static_cast<bool>((encoded >> (BIT_WIDTH - 1 - level)) & U{1});
            const int ones_l = bits.rank(true, l);
            const int ones_r = bits.rank(true, r);
            if(bit){
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = zero_count[static_cast<std::size_t>(level)] + ones_r;
            }else{
                l -= ones_l;
                r -= ones_r;
            }
        }
        if(r - l <= k) return _n;
        int position = l + k;
        for(int level = BIT_WIDTH - 1; level >= 0; level--){
            const bool bit =
                static_cast<bool>((encoded >> (BIT_WIDTH - 1 - level)) & U{1});
            position = bit_vectors[static_cast<std::size_t>(level)].select(
                bit,
                bit ? position - zero_count[static_cast<std::size_t>(level)] : position
            );
        }
        return position;
    }

    T kth_smallest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (kth_smallest).");
        if(k < 0 || r - l <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (kth_smallest)."
            );
        }
        U value = 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto stats = bits.range_stats(l, r);
            const int zeros = (r - l) - stats.ones;
            const int ones_l = bits.rank(true, l);
            if(k < zeros){
                value <<= 1;
                l -= ones_l;
                r = l + zeros;
            }else{
                value = static_cast<U>((value << 1) | U{1});
                k -= zeros;
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = l + stats.ones;
            }
        }
        return decode(value);
    }

    T kth_largest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (kth_largest).");
        if(k < 0 || r - l <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (kth_largest)."
            );
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
            throw std::runtime_error(
                "library assertion fault: range violation (range_freq)."
            );
        }
        return count_less_value(l, r, upper) - count_less_value(l, r, lower);
    }

    W sum(int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (sum).");
        return bit_vectors[0].range_stats(l, r).sum;
    }

    W range_sum(int l, int r, T upper) const{
        check_range(l, r, "library assertion fault: range violation (range_sum).");
        return sum_less_value(l, r, upper);
    }

    W range_sum(int l, int r, T lower, T upper) const{
        check_range(l, r, "library assertion fault: range violation (range_sum).");
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (range_sum)."
            );
        }
        return sum_less_value(l, r, upper) - sum_less_value(l, r, lower);
    }

    W sum_k_smallest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (sum_k_smallest).");
        if(k < 0 || r - l < k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (sum_k_smallest)."
            );
        }
        W result{};
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto& bits = bit_vectors[static_cast<std::size_t>(level)];
            auto stats = bits.range_stats(l, r);
            const int zeros = (r - l) - stats.ones;
            const int ones_l = bits.rank(true, l);
            if(level == BIT_WIDTH - 1){
                if(k <= zeros){
                    result += bits.sum_first(false, l, k);
                }else{
                    result += stats.zero_sum;
                    result += bits.sum_first(true, l, k - zeros);
                }
                return result;
            }
            if(k <= zeros){
                l -= ones_l;
                r = l + zeros;
            }else{
                result += stats.zero_sum;
                k -= zeros;
                l = zero_count[static_cast<std::size_t>(level)] + ones_l;
                r = l + stats.ones;
            }
        }
        return result;
    }

    W sum_k_largest(int l, int r, int k) const{
        check_range(l, r, "library assertion fault: range violation (sum_k_largest).");
        if(k < 0 || r - l < k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (sum_k_largest)."
            );
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

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DYNAMIC_DYNAMIC_WEIGHTED_WAVELET_MATRIX_HPP_INCLUDED
