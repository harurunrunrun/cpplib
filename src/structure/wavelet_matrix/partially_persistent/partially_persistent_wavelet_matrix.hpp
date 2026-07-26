#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_PARTIALLY_PERSISTENT_PARTIALLY_PERSISTENT_WAVELET_MATRIX_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_PARTIALLY_PERSISTENT_PARTIALLY_PERSISTENT_WAVELET_MATRIX_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <deque>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "../detail/persistent_btree_bit_sequence.hpp"

template<
    class T,
    int MAX_SIZE,
    int MAX_VERSION,
    int BIT_WIDTH = std::numeric_limits<std::make_unsigned_t<T>>::digits
>
struct PartiallyPersistentWaveletMatrix{
    static_assert(std::is_integral_v<T>);
    static_assert(!std::is_same_v<std::remove_cv_t<T>, bool>);
    static_assert(MAX_SIZE >= 0);
    static_assert(MAX_VERSION >= 0);

    using U = std::make_unsigned_t<T>;
    static constexpr int digits = std::numeric_limits<U>::digits;
    static_assert(0 < BIT_WIDTH && BIT_WIDTH <= digits);
    static_assert(!std::is_signed_v<T> || BIT_WIDTH == digits);

private:
    using BitVector = wavelet_matrix_detail::PersistentBTreeBitVector<>;
    using Root = typename BitVector::Root;
    using Snapshot = typename BitVector::Snapshot;

    struct VersionState{
        std::array<Root, static_cast<std::size_t>(BIT_WIDTH)> root{};
        std::array<int, static_cast<std::size_t>(BIT_WIDTH)> zero{};
    };

    int _n = 0;
    int _versions = 0;
    std::array<BitVector, static_cast<std::size_t>(BIT_WIDTH)> bit_vector{};
    std::deque<VersionState> version_state;

    static constexpr U sign_mask = std::is_signed_v<T> ? U{1} << (digits - 1) : U{0};
    static U encode(T value){ return static_cast<U>(value) ^ sign_mask; }
    static T decode(U value){ return static_cast<T>(value ^ sign_mask); }
    static bool fits(U value){
        if constexpr(BIT_WIDTH < digits) return (value >> BIT_WIDTH) == 0;
        else return true;
    }
    static U encode_checked(T value, const char* message){
        const U result = encode(value);
        if(!fits(result))[[unlikely]] throw std::runtime_error(message);
        return result;
    }
    static bool bit_at(U value, int level){
        return static_cast<bool>((value >> (BIT_WIDTH - 1 - level)) & U{1});
    }

    void check_version(int version, const char* message) const{
        if(version < 0 || _versions <= version)[[unlikely]] throw std::runtime_error(message);
    }
    void check_index(int k, const char* message) const{
        if(k < 0 || _n <= k)[[unlikely]] throw std::runtime_error(message);
    }
    void check_range(int l, int r, const char* message) const{
        if(l < 0 || r < l || _n < r)[[unlikely]] throw std::runtime_error(message);
    }
    void check_version_capacity() const{
        if(_versions > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity violation (version).");
        }
    }

    std::pair<int, int> descend(int version, int level, int left, int right, bool bit) const{
        const Root root = version_state[static_cast<std::size_t>(version)].root[static_cast<std::size_t>(level)];
        const auto rank = bit_vector[static_cast<std::size_t>(level)].rank_pair(root, left, right);
        if(bit){
            const int zeros = version_state[static_cast<std::size_t>(version)].zero[static_cast<std::size_t>(level)];
            return {zeros + rank.ones_l, zeros + rank.ones_r};
        }
        return {left - rank.ones_l, right - rank.ones_r};
    }

    int count_less_encoded(int version, int left, int right, U upper) const{
        int result = 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            const Root root = version_state[static_cast<std::size_t>(version)].root[static_cast<std::size_t>(level)];
            const auto rank = bit_vector[static_cast<std::size_t>(level)].rank_pair(root, left, right);
            const int zero_left = left - rank.ones_l;
            const int zero_right = right - rank.ones_r;
            if(bit_at(upper, level)){
                result += zero_right - zero_left;
                const int zeros = version_state[static_cast<std::size_t>(version)].zero[static_cast<std::size_t>(level)];
                left = zeros + rank.ones_l;
                right = zeros + rank.ones_r;
            }else{
                left = zero_left;
                right = zero_right;
            }
        }
        return result;
    }

    int count_less_value(int version, int left, int right, T upper) const{
        const U key = encode(upper);
        if(!fits(key)) return right - left;
        return count_less_encoded(version, left, right, key);
    }

    void build(const std::vector<T>& sequence){
        _n = static_cast<int>(sequence.size());
        if(sequence.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        std::vector<U> current(sequence.size());
        for(std::size_t index = 0; index < sequence.size(); index++){
            current[index] = encode_checked(sequence[index], "library assertion fault: bit width violation (constructor).");
        }
        version_state.emplace_back();
        for(int level = 0; level < BIT_WIDTH; level++){
            std::vector<unsigned char> bits(current.size());
            std::vector<U> next;
            next.reserve(current.size());
            int zeros = 0;
            for(std::size_t index = 0; index < current.size(); index++){
                const bool bit = bit_at(current[index], level);
                bits[index] = static_cast<unsigned char>(bit);
                if(!bit){
                    next.push_back(current[index]);
                    zeros++;
                }
            }
            for(U value : current){
                if(bit_at(value, level)) next.push_back(value);
            }
            version_state[0].root[static_cast<std::size_t>(level)] = bit_vector[static_cast<std::size_t>(level)].build(bits);
            version_state[0].zero[static_cast<std::size_t>(level)] = zeros;
            current = std::move(next);
        }
        _versions = 1;
    }

public:
    PartiallyPersistentWaveletMatrix(): PartiallyPersistentWaveletMatrix(std::vector<T>{}){}
    explicit PartiallyPersistentWaveletMatrix(const std::vector<T>& sequence){ build(sequence); }
    template<std::size_t N>
    explicit PartiallyPersistentWaveletMatrix(const std::array<T, N>& sequence):
        PartiallyPersistentWaveletMatrix(std::vector<T>(sequence.begin(), sequence.end())){
        static_assert(N <= static_cast<std::size_t>(MAX_SIZE));
    }

    int size() const{ return _n; }
    int versions() const{ return _versions; }
    int latest_version() const{ return _versions - 1; }

    T access(int version, int k) const{
        check_version(version, "library assertion fault: range violation (access).");
        check_index(k, "library assertion fault: range violation (access).");
        U value = 0;
        int position = k;
        for(int level = 0; level < BIT_WIDTH; level++){
            const std::size_t index = static_cast<std::size_t>(level);
            const Root root = version_state[static_cast<std::size_t>(version)].root[index];
            const bool bit = bit_vector[index].access(root, position).bit;
            if(bit) value |= U{1} << (BIT_WIDTH - 1 - level);
            const int ones_before = bit_vector[index].rank(root, true, position);
            position = bit ? version_state[static_cast<std::size_t>(version)].zero[index] + ones_before : position - ones_before;
        }
        return decode(value);
    }

    int set(int k, T value){
        check_index(k, "library assertion fault: range violation (set).");
        const U encoded = encode_checked(value, "library assertion fault: bit width violation (set).");
        check_version_capacity();

        const int next_version = _versions;
        const VersionState base = version_state.back();
        version_state.push_back(base);
        std::array<Snapshot, static_cast<std::size_t>(BIT_WIDTH)> snapshot{};
        for(int level = 0; level < BIT_WIDTH; level++){
            snapshot[static_cast<std::size_t>(level)] = bit_vector[static_cast<std::size_t>(level)].snapshot();
        }
        auto next_root = base.root;
        auto next_zero = base.zero;
        try{
            int old_position = k;
            for(int level = 0; level < BIT_WIDTH; level++){
                const std::size_t index = static_cast<std::size_t>(level);
                const Root root = next_root[index];
                const bool old_bit = bit_vector[index].access(root, old_position).bit;
                const int ones_before = bit_vector[index].rank(root, true, old_position);
                const int next_position = old_bit ? next_zero[index] + ones_before : old_position - ones_before;
                next_root[index] = bit_vector[index].erase(root, old_position).root;
                if(!old_bit) next_zero[index]--;
                old_position = next_position;
            }
            int new_position = k;
            for(int level = 0; level < BIT_WIDTH; level++){
                const std::size_t index = static_cast<std::size_t>(level);
                const Root root = next_root[index];
                const bool new_bit = bit_at(encoded, level);
                const int ones_before = bit_vector[index].rank(root, true, new_position);
                const int next_position = new_bit ? next_zero[index] + ones_before : new_position - ones_before;
                next_root[index] = bit_vector[index].insert(root, new_position, new_bit);
                if(!new_bit) next_zero[index]++;
                new_position = next_position;
            }
        }catch(...){
            for(int level = 0; level < BIT_WIDTH; level++){
                bit_vector[static_cast<std::size_t>(level)].rollback(snapshot[static_cast<std::size_t>(level)]);
            }
            version_state.pop_back();
            throw;
        }
        version_state.back().root = next_root;
        version_state.back().zero = next_zero;
        _versions++;
        return next_version;
    }
    int rank(int version, T value, int r) const{ return rank(version, value, 0, r); }
    int rank(int version, T value, int l, int r) const{
        check_version(version, "library assertion fault: range violation (rank).");
        check_range(l, r, "library assertion fault: range violation (rank).");
        const U key = encode(value);
        if(!fits(key)) return 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto next = descend(version, level, l, r, bit_at(key, level));
            l = next.first;
            r = next.second;
        }
        return r - l;
    }

    int select(int version, T value, int k) const{
        check_version(version, "library assertion fault: range violation (select).");
        if(k < 0)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (select).");
        const U key = encode(value);
        if(!fits(key)) return _n;
        int left = 0;
        int right = _n;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto next = descend(version, level, left, right, bit_at(key, level));
            left = next.first;
            right = next.second;
        }
        if(right - left <= k) return _n;
        int position = left + k;
        for(int level = BIT_WIDTH - 1; level >= 0; level--){
            const bool bit = bit_at(key, level);
            const int occurrence = bit ? position - version_state[static_cast<std::size_t>(version)].zero[static_cast<std::size_t>(level)] : position;
            position = bit_vector[static_cast<std::size_t>(level)].select(
                version_state[static_cast<std::size_t>(version)].root[static_cast<std::size_t>(level)],
                bit,
                occurrence
            );
        }
        return position;
    }

    T kth_smallest(int version, int l, int r, int k) const{
        check_version(version, "library assertion fault: range violation (kth_smallest).");
        check_range(l, r, "library assertion fault: range violation (kth_smallest).");
        if(k < 0 || r - l <= k)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (kth_smallest).");
        U value = 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            const std::size_t index = static_cast<std::size_t>(level);
            const Root root = version_state[static_cast<std::size_t>(version)].root[index];
            const auto rank = bit_vector[index].rank_pair(root, l, r);
            const int zero_left = l - rank.ones_l;
            const int zero_right = r - rank.ones_r;
            const int zero_size = zero_right - zero_left;
            if(k < zero_size){
                l = zero_left;
                r = zero_right;
            }else{
                value |= U{1} << (BIT_WIDTH - 1 - level);
                k -= zero_size;
                const int zeros = version_state[static_cast<std::size_t>(version)].zero[index];
                l = zeros + rank.ones_l;
                r = zeros + rank.ones_r;
            }
        }
        return decode(value);
    }

    T kth_largest(int version, int l, int r, int k) const{
        check_version(version, "library assertion fault: range violation (kth_largest).");
        check_range(l, r, "library assertion fault: range violation (kth_largest).");
        if(k < 0 || r - l <= k)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (kth_largest).");
        return kth_smallest(version, l, r, r - l - 1 - k);
    }

    int range_freq(int version, int l, int r, T upper) const{
        check_version(version, "library assertion fault: range violation (range_freq).");
        check_range(l, r, "library assertion fault: range violation (range_freq).");
        return count_less_value(version, l, r, upper);
    }

    int range_freq(int version, int l, int r, T lower, T upper) const{
        check_version(version, "library assertion fault: range violation (range_freq).");
        check_range(l, r, "library assertion fault: range violation (range_freq).");
        if(upper < lower)[[unlikely]] throw std::runtime_error("library assertion fault: range violation (range_freq).");
        return count_less_value(version, l, r, upper) - count_less_value(version, l, r, lower);
    }

    std::optional<T> prev_value(int version, int l, int r, T upper) const{
        const int count = range_freq(version, l, r, upper);
        if(count == 0) return std::nullopt;
        return kth_smallest(version, l, r, count - 1);
    }

    std::optional<T> next_value(int version, int l, int r, T lower) const{
        const int count = range_freq(version, l, r, lower);
        if(count == r - l) return std::nullopt;
        return kth_smallest(version, l, r, count);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_PARTIALLY_PERSISTENT_PARTIALLY_PERSISTENT_WAVELET_MATRIX_HPP_INCLUDED
