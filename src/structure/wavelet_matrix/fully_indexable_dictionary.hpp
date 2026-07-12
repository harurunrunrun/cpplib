#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

template<int MAX_SIZE>
struct FullyIndexableDictionary{
    static_assert(MAX_SIZE >= 0);

private:
    static constexpr int word_count = (MAX_SIZE + 63) / 64;
    int _n = 0;
    std::array<std::uint64_t, word_count> bit{};
    std::array<int, word_count + 1> prefix{};

    void check_position(int k, const char* message) const{
        if(k < 0 || _n < k)[[unlikely]] throw std::runtime_error(message);
    }

public:
    explicit FullyIndexableDictionary(int n = 0){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        _n = n;
    }

    explicit FullyIndexableDictionary(const std::vector<bool>& values):
        FullyIndexableDictionary(static_cast<int>(values.size())){
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
        int low = 0, high = _n;
        while(low < high){
            int mid = low + (high - low) / 2;
            if(rank(value, mid + 1) <= k) low = mid + 1;
            else high = mid;
        }
        return low;
    }
};
