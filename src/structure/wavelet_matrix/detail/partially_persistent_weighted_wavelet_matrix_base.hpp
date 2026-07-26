#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DETAIL_PARTIALLY_PERSISTENT_WEIGHTED_WAVELET_MATRIX_BASE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DETAIL_PARTIALLY_PERSISTENT_WEIGHTED_WAVELET_MATRIX_BASE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <deque>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "immutable_btree_bit_sequence.hpp"

namespace wavelet_matrix_detail{

template<class T, class W, int MAX_SIZE, int MAX_VERSION, int BIT_WIDTH>
struct PartiallyPersistentWeightedWaveletMatrixBase{
    static_assert(std::is_integral_v<T>);
    static_assert(!std::is_same_v<std::remove_cv_t<T>, bool>);
    static_assert(MAX_SIZE >= 0);
    static_assert(MAX_VERSION >= 0);
    using U = std::make_unsigned_t<T>;
    static constexpr int digits = std::numeric_limits<U>::digits;
    static_assert(0 < BIT_WIDTH && BIT_WIDTH <= digits);
    static_assert(!std::is_signed_v<T> || BIT_WIDTH == digits);

private:
    using BitSequence = ImmutableBTreeBitSequence<W, W>;
    using Root = typename BitSequence::Root;
    using Snapshot = typename BitSequence::Snapshot;
    struct VersionState{
        std::array<Root, static_cast<std::size_t>(BIT_WIDTH)> root{};
        std::array<int, static_cast<std::size_t>(BIT_WIDTH)> zero{};
    };

    int _n = 0;
    int _versions = 0;
    std::array<BitSequence, static_cast<std::size_t>(BIT_WIDTH)> bit_sequence{};
    std::deque<VersionState> version_state;

    static constexpr U sign_mask = std::is_signed_v<T>
        ? U{1} << (digits - 1) : U{0};
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
        if(version < 0 || _versions <= version)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }
    void check_index(int position, const char* message) const{
        if(position < 0 || _n <= position)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }
    void check_range(int left, int right, const char* message) const{
        if(left < 0 || right < left || _n < right)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }
    void check_version_capacity() const{
        if(_versions > MAX_VERSION)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation (version)."
            );
        }
    }
    std::pair<int, int> descend(
        int version, int level, int left, int right, bool bit
    ) const{
        const std::size_t index = static_cast<std::size_t>(level);
        const auto& state = version_state[static_cast<std::size_t>(version)];
        const auto rank = bit_sequence[index].rank_pair(
            state.root[index], left, right
        );
        if(bit) return {
            state.zero[index] + rank.ones_l,
            state.zero[index] + rank.ones_r
        };
        return {left - rank.ones_l, right - rank.ones_r};
    }
    int count_less_encoded(
        int version, int left, int right, U upper
    ) const{
        int result = 0;
        const auto& state = version_state[static_cast<std::size_t>(version)];
        for(int level = 0; level < BIT_WIDTH; level++){
            const std::size_t index = static_cast<std::size_t>(level);
            const auto rank = bit_sequence[index].rank_pair(
                state.root[index], left, right
            );
            const int zero_left = left - rank.ones_l;
            const int zero_right = right - rank.ones_r;
            if(bit_at(upper, level)){
                result += zero_right - zero_left;
                left = state.zero[index] + rank.ones_l;
                right = state.zero[index] + rank.ones_r;
            }else{
                left = zero_left;
                right = zero_right;
            }
        }
        return result;
    }
    int count_less_value(
        int version, int left, int right, T upper
    ) const{
        const U key = encode(upper);
        return fits(key)
            ? count_less_encoded(version, left, right, key)
            : right - left;
    }
    W sum_less_encoded(
        int version, int left, int right, U upper
    ) const{
        W result{};
        const auto& state = version_state[static_cast<std::size_t>(version)];
        for(int level = 0; level < BIT_WIDTH; level++){
            const std::size_t index = static_cast<std::size_t>(level);
            const auto stats = bit_sequence[index].range_stats(
                state.root[index], left, right
            );
            const int zero_left = left - stats.ones_l;
            const int zero_right = right - stats.ones_r;
            if(bit_at(upper, level)){
                result = result + stats.zero_sum;
                left = state.zero[index] + stats.ones_l;
                right = state.zero[index] + stats.ones_r;
            }else{
                left = zero_left;
                right = zero_right;
            }
        }
        return result;
    }
    W sum_less_value(
        int version, int left, int right, T upper
    ) const{
        const U key = encode(upper);
        return fits(key)
            ? sum_less_encoded(version, left, right, key)
            : sum(version, left, right);
    }
    void build(
        const std::vector<T>& sequence,
        const std::vector<W>& weights
    ){
        if(sequence.size() != weights.size()
            || sequence.size() > static_cast<std::size_t>(MAX_SIZE)
        )[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        _n = static_cast<int>(sequence.size());
        std::vector<U> current_value(sequence.size());
        std::vector<W> current_weight = weights;
        for(std::size_t i = 0; i < sequence.size(); i++){
            current_value[i] = encode_checked(
                sequence[i],
                "library assertion fault: bit width violation (constructor)."
            );
        }
        version_state.emplace_back();
        for(int level = 0; level < BIT_WIDTH; level++){
            std::vector<unsigned char> bits(current_value.size());
            std::vector<U> next_value;
            std::vector<W> next_weight;
            next_value.reserve(current_value.size());
            next_weight.reserve(current_weight.size());
            int zeros = 0;
            for(std::size_t i = 0; i < current_value.size(); i++){
                const bool bit = bit_at(current_value[i], level);
                bits[i] = static_cast<unsigned char>(bit);
                if(!bit){
                    next_value.push_back(current_value[i]);
                    next_weight.push_back(current_weight[i]);
                    zeros++;
                }
            }
            for(std::size_t i = 0; i < current_value.size(); i++){
                if(bit_at(current_value[i], level)){
                    next_value.push_back(current_value[i]);
                    next_weight.push_back(current_weight[i]);
                }
            }
            const std::size_t index = static_cast<std::size_t>(level);
            version_state[0].root[index] =
                bit_sequence[index].build(bits, current_weight);
            version_state[0].zero[index] = zeros;
            current_value = std::move(next_value);
            current_weight = std::move(next_weight);
        }
        _versions = 1;
    }

protected:
    int set_latest(
        int position, T value, const W& weight_value
    ){
        check_index(position, "library assertion fault: range violation (set).");
        const U encoded = encode_checked(
            value, "library assertion fault: bit width violation (set)."
        );
        check_version_capacity();
        const int next_version = _versions;
        const VersionState base = version_state.back();
        version_state.push_back(base);
        std::array<Snapshot, static_cast<std::size_t>(BIT_WIDTH)> snapshot{};
        for(int level = 0; level < BIT_WIDTH; level++){
            snapshot[static_cast<std::size_t>(level)] =
                bit_sequence[static_cast<std::size_t>(level)].snapshot();
        }
        auto next_root = base.root;
        auto next_zero = base.zero;
        try{
            int old_position = position;
            for(int level = 0; level < BIT_WIDTH; level++){
                const std::size_t index = static_cast<std::size_t>(level);
                const Root root = next_root[index];
                const bool bit =
                    bit_sequence[index].access(root, old_position).bit;
                const int ones_before =
                    bit_sequence[index].rank(root, true, old_position);
                const int following = bit
                    ? next_zero[index] + ones_before
                    : old_position - ones_before;
                next_root[index] =
                    bit_sequence[index].erase(root, old_position).root;
                if(!bit) next_zero[index]--;
                old_position = following;
            }
            int new_position = position;
            for(int level = 0; level < BIT_WIDTH; level++){
                const std::size_t index = static_cast<std::size_t>(level);
                const Root root = next_root[index];
                const bool bit = bit_at(encoded, level);
                const int ones_before =
                    bit_sequence[index].rank(root, true, new_position);
                const int following = bit
                    ? next_zero[index] + ones_before
                    : new_position - ones_before;
                next_root[index] = bit_sequence[index].insert(
                    root, new_position, bit, weight_value
                );
                if(!bit) next_zero[index]++;
                new_position = following;
            }
        }catch(...){
            for(int level = 0; level < BIT_WIDTH; level++){
                const std::size_t index = static_cast<std::size_t>(level);
                bit_sequence[index].rollback(snapshot[index]);
            }
            version_state.pop_back();
            throw;
        }
        version_state.back().root = next_root;
        version_state.back().zero = next_zero;
        _versions++;
        return next_version;
    }
    int set_weight_latest(
        int position, const W& weight_value
    ){
        check_index(
            position, "library assertion fault: range violation (set_weight)."
        );
        check_version_capacity();
        const int next_version = _versions;
        const VersionState base = version_state.back();
        version_state.push_back(base);
        std::array<Snapshot, static_cast<std::size_t>(BIT_WIDTH)> snapshot{};
        for(int level = 0; level < BIT_WIDTH; level++){
            snapshot[static_cast<std::size_t>(level)] =
                bit_sequence[static_cast<std::size_t>(level)].snapshot();
        }
        try{
            int current = position;
            for(int level = 0; level < BIT_WIDTH; level++){
                const std::size_t index = static_cast<std::size_t>(level);
                const Root root = base.root[index];
                const bool bit = bit_sequence[index].access(root, current).bit;
                const int ones_before =
                    bit_sequence[index].rank(root, true, current);
                version_state.back().root[index] =
                    bit_sequence[index].set_payload(
                        root, current, weight_value
                    ).root;
                current = bit
                    ? base.zero[index] + ones_before
                    : current - ones_before;
            }
        }catch(...){
            for(int level = 0; level < BIT_WIDTH; level++){
                const std::size_t index = static_cast<std::size_t>(level);
                bit_sequence[index].rollback(snapshot[index]);
            }
            version_state.pop_back();
            throw;
        }
        _versions++;
        return next_version;
    }
public:
    PartiallyPersistentWeightedWaveletMatrixBase():
        PartiallyPersistentWeightedWaveletMatrixBase(
            std::vector<T>{}, std::vector<W>{}
        ){}
    PartiallyPersistentWeightedWaveletMatrixBase(
        const std::vector<T>& sequence,
        const std::vector<W>& weights
    ){
        build(sequence, weights);
    }

    int size() const{ return _n; }
    int versions() const{ return _versions; }
    int latest_version() const{ return _versions - 1; }

    T access(int version, int position) const{
        check_version(
            version, "library assertion fault: range violation (access)."
        );
        check_index(
            position, "library assertion fault: range violation (access)."
        );
        const auto& state = version_state[static_cast<std::size_t>(version)];
        U value = 0;
        int current = position;
        for(int level = 0; level < BIT_WIDTH; level++){
            const std::size_t index = static_cast<std::size_t>(level);
            const bool bit =
                bit_sequence[index].access(state.root[index], current).bit;
            if(bit) value |= U{1} << (BIT_WIDTH - 1 - level);
            const int ones_before =
                bit_sequence[index].rank(state.root[index], true, current);
            current = bit
                ? state.zero[index] + ones_before
                : current - ones_before;
        }
        return decode(value);
    }
    W weight(int version, int position) const{
        check_version(
            version, "library assertion fault: range violation (weight)."
        );
        check_index(
            position, "library assertion fault: range violation (weight)."
        );
        const Root root =
            version_state[static_cast<std::size_t>(version)].root[0];
        return bit_sequence[0].access(root, position).payload;
    }
    int rank(int version, T value, int right) const{
        return rank(version, value, 0, right);
    }
    int rank(int version, T value, int left, int right) const{
        check_version(version, "library assertion fault: range violation (rank).");
        check_range(left, right, "library assertion fault: range violation (rank).");
        const U key = encode(value);
        if(!fits(key)) return 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto next = descend(
                version, level, left, right, bit_at(key, level)
            );
            left = next.first;
            right = next.second;
        }
        return right - left;
    }
    int select(int version, T value, int occurrence) const{
        check_version(
            version, "library assertion fault: range violation (select)."
        );
        if(occurrence < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (select)."
            );
        }
        const U key = encode(value);
        if(!fits(key)) return _n;
        int left = 0;
        int right = _n;
        for(int level = 0; level < BIT_WIDTH; level++){
            const auto next = descend(
                version, level, left, right, bit_at(key, level)
            );
            left = next.first;
            right = next.second;
        }
        if(right - left <= occurrence) return _n;
        int position = left + occurrence;
        const auto& state = version_state[static_cast<std::size_t>(version)];
        for(int level = BIT_WIDTH - 1; level >= 0; level--){
            const std::size_t index = static_cast<std::size_t>(level);
            const bool bit = bit_at(key, level);
            position = bit_sequence[index].select(
                state.root[index],
                bit,
                bit ? position - state.zero[index] : position
            );
        }
        return position;
    }
    T kth_smallest(int version, int left, int right, int k) const{
        check_version(
            version, "library assertion fault: range violation (kth_smallest)."
        );
        check_range(
            left, right, "library assertion fault: range violation (kth_smallest)."
        );
        if(k < 0 || right - left <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (kth_smallest)."
            );
        }
        const auto& state = version_state[static_cast<std::size_t>(version)];
        U value = 0;
        for(int level = 0; level < BIT_WIDTH; level++){
            const std::size_t index = static_cast<std::size_t>(level);
            const auto rank = bit_sequence[index].rank_pair(
                state.root[index], left, right
            );
            const int zero_left = left - rank.ones_l;
            const int zero_right = right - rank.ones_r;
            const int zero_count = zero_right - zero_left;
            if(k < zero_count){
                left = zero_left;
                right = zero_right;
            }else{
                value |= U{1} << (BIT_WIDTH - 1 - level);
                k -= zero_count;
                left = state.zero[index] + rank.ones_l;
                right = state.zero[index] + rank.ones_r;
            }
        }
        return decode(value);
    }
    T kth_largest(int version, int left, int right, int k) const{
        check_version(
            version, "library assertion fault: range violation (kth_largest)."
        );
        check_range(
            left, right, "library assertion fault: range violation (kth_largest)."
        );
        if(k < 0 || right - left <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (kth_largest)."
            );
        }
        return kth_smallest(version, left, right, right - left - 1 - k);
    }
    int range_freq(int version, int left, int right, T upper) const{
        check_version(
            version, "library assertion fault: range violation (range_freq)."
        );
        check_range(
            left, right, "library assertion fault: range violation (range_freq)."
        );
        return count_less_value(version, left, right, upper);
    }
    int range_freq(
        int version, int left, int right, T lower, T upper
    ) const{
        check_version(
            version, "library assertion fault: range violation (range_freq)."
        );
        check_range(
            left, right, "library assertion fault: range violation (range_freq)."
        );
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (range_freq)."
            );
        }
        return count_less_value(version, left, right, upper)
            - count_less_value(version, left, right, lower);
    }
    W sum(int version, int left, int right) const{
        check_version(version, "library assertion fault: range violation (sum).");
        check_range(left, right, "library assertion fault: range violation (sum).");
        const Root root =
            version_state[static_cast<std::size_t>(version)].root[0];
        return bit_sequence[0].sum_all(root, left, right);
    }
    W range_sum(int version, int left, int right, T upper) const{
        check_version(
            version, "library assertion fault: range violation (range_sum)."
        );
        check_range(
            left, right, "library assertion fault: range violation (range_sum)."
        );
        return sum_less_value(version, left, right, upper);
    }
    W range_sum(
        int version, int left, int right, T lower, T upper
    ) const{
        check_version(
            version, "library assertion fault: range violation (range_sum)."
        );
        check_range(
            left, right, "library assertion fault: range violation (range_sum)."
        );
        if(upper < lower)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (range_sum)."
            );
        }
        return sum_less_value(version, left, right, upper)
            - sum_less_value(version, left, right, lower);
    }
    W sum_k_smallest(int version, int left, int right, int k) const{
        check_version(
            version, "library assertion fault: range violation (sum_k_smallest)."
        );
        check_range(
            left, right, "library assertion fault: range violation (sum_k_smallest)."
        );
        if(k < 0 || right - left < k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (sum_k_smallest)."
            );
        }
        if(k == 0) return W{};
        if(k == right - left) return sum(version, left, right);
        const auto& state = version_state[static_cast<std::size_t>(version)];
        W result{};
        for(int level = 0; level < BIT_WIDTH; level++){
            const std::size_t index = static_cast<std::size_t>(level);
            const auto stats = bit_sequence[index].range_stats(
                state.root[index], left, right
            );
            const int zero_left = left - stats.ones_l;
            const int zero_right = right - stats.ones_r;
            const int zero_count = zero_right - zero_left;
            if(k < zero_count){
                if(level + 1 == BIT_WIDTH){
                    return result + bit_sequence[index].weight_of_first(
                        state.root[index], false, left, k
                    );
                }
                left = zero_left;
                right = zero_right;
            }else{
                result = result + stats.zero_sum;
                k -= zero_count;
                if(k == 0) return result;
                if(level + 1 == BIT_WIDTH){
                    return result + bit_sequence[index].weight_of_first(
                        state.root[index], true, left, k
                    );
                }
                left = state.zero[index] + stats.ones_l;
                right = state.zero[index] + stats.ones_r;
            }
        }
        return result;
    }
    W sum_k_largest(int version, int left, int right, int k) const{
        check_version(
            version, "library assertion fault: range violation (sum_k_largest)."
        );
        check_range(
            left, right, "library assertion fault: range violation (sum_k_largest)."
        );
        if(k < 0 || right - left < k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (sum_k_largest)."
            );
        }
        return sum(version, left, right)
            - sum_k_smallest(version, left, right, right - left - k);
    }
    std::optional<T> prev_value(
        int version, int left, int right, T upper
    ) const{
        const int count = range_freq(version, left, right, upper);
        if(count == 0) return std::nullopt;
        return kth_smallest(version, left, right, count - 1);
    }
    std::optional<T> next_value(
        int version, int left, int right, T lower
    ) const{
        const int count = range_freq(version, left, right, lower);
        if(count == right - left) return std::nullopt;
        return kth_smallest(version, left, right, count);
    }
};

} // namespace wavelet_matrix_detail

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DETAIL_PARTIALLY_PERSISTENT_WEIGHTED_WAVELET_MATRIX_BASE_HPP_INCLUDED
