#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <vector>

struct RollingHash{
    using u64 = std::uint64_t;
    using u128 = __uint128_t;

    static constexpr u64 mod = (1ULL << 61) - 1;
    static constexpr u64 default_base = 1000003;

private:
    int n = 0;
    u64 base = default_base;
    std::vector<u64> power;
    std::vector<u64> prefix;

    static u64 add_mod(u64 a, u64 b){
        a += b;
        if(a >= mod) a -= mod;
        return a;
    }
    static u64 sub_mod(u64 a, u64 b){
        return a >= b ? a - b : a + mod - b;
    }
    static u64 mul_mod(u64 a, u64 b){
        u128 x = static_cast<u128>(a) * b;
        x = (x >> 61) + (x & mod);
        if(x >= mod) x -= mod;
        return static_cast<u64>(x);
    }
    void check_range(int l, int r, const char* message) const{
        if(l < 0 || r < l || n < r)[[unlikely]] throw std::runtime_error(message);
    }

public:
    RollingHash() = default;
    explicit RollingHash(const std::string& s, u64 base_value = default_base){
        build(s, base_value);
    }

    void build(const std::string& s, u64 base_value = default_base){
        if(base_value == 0 || base_value >= mod)[[unlikely]]{
            throw std::runtime_error("library assertion fault: invalid argument (build).");
        }
        n = static_cast<int>(s.size());
        base = base_value;
        power.assign(static_cast<std::size_t>(n) + 1, 1);
        prefix.assign(static_cast<std::size_t>(n) + 1, 0);
        for(int i = 0; i < n; i++){
            power[static_cast<std::size_t>(i + 1)] = mul_mod(power[static_cast<std::size_t>(i)], base);
            prefix[static_cast<std::size_t>(i + 1)] = add_mod(
                mul_mod(prefix[static_cast<std::size_t>(i)], base),
                static_cast<unsigned char>(s[static_cast<std::size_t>(i)]) + 1
            );
        }
    }

    int size() const{ return n; }
    u64 base_value() const{ return base; }

    u64 get(int l, int r) const{
        check_range(l, r, "library assertion fault: range violation (get).");
        return sub_mod(
            prefix[static_cast<std::size_t>(r)],
            mul_mod(prefix[static_cast<std::size_t>(l)], power[static_cast<std::size_t>(r - l)])
        );
    }
    u64 all() const{
        return get(0, n);
    }

    int lcp(const RollingHash& other, int l1, int l2) const{
        if(base != other.base)[[unlikely]]{
            throw std::runtime_error("library assertion fault: base mismatch (lcp).");
        }
        check_range(l1, l1, "library assertion fault: range violation (lcp).");
        other.check_range(l2, l2, "library assertion fault: range violation (lcp).");
        int low = 0;
        int high = std::min(n - l1, other.n - l2) + 1;
        while(high - low > 1){
            int mid = (low + high) >> 1;
            if(get(l1, l1 + mid) == other.get(l2, l2 + mid)) low = mid;
            else high = mid;
        }
        return low;
    }
};
