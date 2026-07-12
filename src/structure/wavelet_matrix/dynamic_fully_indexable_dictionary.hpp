#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

template<int MAX_SIZE>
struct DynamicFullyIndexableDictionary{
    static_assert(MAX_SIZE >= 0);

private:
    static constexpr int word_count = (MAX_SIZE + 63) / 64;
    int _n = 0;
    std::array<std::uint64_t, word_count> bit{};
    std::array<int, word_count + 1> fenwick{};

    void add(int k, int delta){
        for(k++; k <= word_count; k += k & -k) fenwick[k] += delta;
    }
    int sum(int r) const{
        int result = 0;
        for(; r > 0; r -= r & -r) result += fenwick[r];
        return result;
    }

public:
    explicit DynamicFullyIndexableDictionary(int n = 0){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        _n = n;
    }
    explicit DynamicFullyIndexableDictionary(const std::vector<bool>& values):
        DynamicFullyIndexableDictionary(static_cast<int>(values.size())){
        for(int k = 0; k < _n; k++) if(values[static_cast<std::size_t>(k)]) set(k, true);
    }

    int size() const{ return _n; }
    bool operator[](int k) const{
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (operator[]).");
        }
        return (bit[k >> 6] >> (k & 63)) & 1;
    }
    void set(int k, bool value){
        if(k < 0 || _n <= k)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (set).");
        }
        bool old = (bit[k >> 6] >> (k & 63)) & 1;
        if(old == value) return;
        bit[k >> 6] ^= std::uint64_t{1} << (k & 63);
        add(k >> 6, value ? 1 : -1);
    }
    void flip(int k){ set(k, !(*this)[k]); }

    int rank(bool value, int r) const{
        if(r < 0 || _n < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (rank).");
        }
        int word = r >> 6;
        int ones = sum(word);
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
        if(rank(value, _n) <= k) return _n;
        int low = 0, high = _n;
        while(low < high){
            int mid = low + (high - low) / 2;
            if(rank(value, mid + 1) <= k) low = mid + 1;
            else high = mid;
        }
        return low;
    }
};
