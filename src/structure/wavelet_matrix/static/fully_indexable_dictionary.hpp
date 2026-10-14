#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_STATIC_FULLY_INDEXABLE_DICTIONARY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_STATIC_FULLY_INDEXABLE_DICTIONARY_HPP_INCLUDED

#include <algorithm>
#include <array>
#include "../../array/static_or_vector.hpp"
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

template<int MAX_SIZE = -1>
struct FullyIndexableDictionary{
    static_assert(MAX_SIZE == -1 || MAX_SIZE >= 0);

private:
    static constexpr int fixed_word_count = MAX_SIZE < 0 ? 0 : MAX_SIZE / 64 + (MAX_SIZE % 64 != 0);
    int word_count = 0;
    int _n = 0;
    cpplib_detail::StaticOrVector<std::uint64_t, (MAX_SIZE < 0 ? -1 : fixed_word_count)> bit{};
    cpplib_detail::StaticOrVector<int, (MAX_SIZE < 0 ? -1 : fixed_word_count + 1)> prefix{};

    void check_position(int k, const char* message) const{
        if(k < 0 || _n < k)[[unlikely]] throw std::runtime_error(message);
    }

public:
    explicit FullyIndexableDictionary(int n = 0){
        if(n < 0 || (MAX_SIZE >= 0 && MAX_SIZE < n))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        _n = n;
        word_count = MAX_SIZE < 0 ? n / 64 + (n % 64 != 0) : fixed_word_count;
        bit.resize(static_cast<std::size_t>(word_count));
        prefix.resize(static_cast<std::size_t>(word_count) + 1);
    }

    explicit FullyIndexableDictionary(const std::vector<bool>& values):
        FullyIndexableDictionary(cpplib_detail::checked_int_size(values.size())){
        for(int k = 0; k < _n; k++) if(values[static_cast<std::size_t>(k)]) set(k);
        build();
    }

    int size() const{ return _n; }

    void set(int k, bool value = true){
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (set).");
        }
        std::uint64_t mask = std::uint64_t{1} << (k & 63);
        if(value) bit[k >> 6] |= mask;
        else bit[k >> 6] &= ~mask;
    }

    void build(){
        prefix[0] = 0;
        for(int k = 0; k < word_count; k++){
            prefix[k + 1] = prefix[k] + std::popcount(bit[k]);
        }
    }

    bool operator[](int k) const{
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (operator[]).");
        }
        return (bit[k >> 6] >> (k & 63)) & 1;
    }

    int rank(bool value, int r) const{
        check_position(r, "library assertion fault: range violation (rank).");
        int word = r >> 6;
        int ones = prefix[word];
        if((r & 63) != 0 && word < word_count){
            ones += std::popcount(bit[word] & ((std::uint64_t{1} << (r & 63)) - 1));
        }
        return value ? ones : r - ones;
    }

    int rank(bool value, int l, int r) const{
        if(l < 0 || r < l || _n < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (rank).");
        }
        return rank(value, r) - rank(value, l);
    }

    int select(bool value, int k) const{
        if(k < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (select).");
        }
        int total = rank(value, _n);
        if(total <= k) return _n;

        const int words = _n / 64 + (_n % 64 != 0);
        auto count_before_word = [&](int word){
            const int ones = prefix[static_cast<std::size_t>(word)];
            return value ? ones : std::min(_n, word << 6) - ones;
        };
        int low = 0, high = words;
        while(low < high){
            const int mid = low + (high - low) / 2;
            if(count_before_word(mid + 1) <= k) low = mid + 1;
            else high = mid;
        }

        const int word = low;
        const int offset = k - count_before_word(word);
        const int length = std::min(64, _n - (word << 6));
        const std::uint64_t valid = length == 64
            ? ~std::uint64_t{0}
            : (std::uint64_t{1} << length) - 1;
        std::uint64_t candidates = value
            ? bit[static_cast<std::size_t>(word)] & valid
            : ~bit[static_cast<std::size_t>(word)] & valid;
        for(int order = 0; order < offset; ++order){
            candidates &= candidates - 1;
        }
        return (word << 6) + static_cast<int>(std::countr_zero(candidates));
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_STATIC_FULLY_INDEXABLE_DICTIONARY_HPP_INCLUDED
