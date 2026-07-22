#ifndef CPPLIB_SRC_STRUCTURE_MODINT_FAST_MODINT_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_MODINT_FAST_MODINT_HPP_INCLUDED

#include <cstdint>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <utility>

template<int mod>
struct FastModint{
    static_assert(1 <= mod);

private:
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    u32 value = 0;

    static constexpr u32 umod(){
        return static_cast<u32>(mod);
    }

    static constexpr u64 im(){
        return static_cast<u64>(-1) / umod() + 1;
    }

    static u32 reduce(u64 z){
        u64 x = static_cast<u64>((static_cast<__uint128_t>(z) * im()) >> 64);
        u32 v = static_cast<u32>(z - x * umod());
        if(v >= umod()) v += umod();
        return v;
    }

    static constexpr bool is_prime_constexpr(int n){
        if(n <= 1) return false;
        if(n == 2) return true;
        if(n % 2 == 0) return false;
        for(long long i = 3; i * i <= n; i += 2){
            if(n % i == 0) return false;
        }
        return true;
    }

    static long long inv_gcd(long long a, long long m){
        a %= m;
        if(a < 0) a += m;
        long long b = m;
        long long u = 1, v = 0;
        while(b){
            long long t = a / b;
            a -= t * b;
            std::swap(a, b);
            u -= t * v;
            std::swap(u, v);
        }
        if(a != 1)[[unlikely]]{
            throw std::runtime_error("library assertion fault: gcd is not 1 (FastModint::inv).");
        }
        u %= m;
        if(u < 0) u += m;
        return u;
    }

public:
    FastModint() = default;

    FastModint(long long v){
        v %= mod;
        if(v < 0) v += mod;
        value = static_cast<u32>(v);
    }

    static constexpr int get_mod(){
        return mod;
    }

    static FastModint raw(int v){
        FastModint x;
        x.value = static_cast<u32>(v);
        return x;
    }

    int val() const{
        return static_cast<int>(value);
    }

    FastModint pow(long long n) const{
        if(n < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: negative exponent (FastModint::pow).");
        }
        FastModint result = 1;
        FastModint a = *this;
        while(n){
            if(n & 1) result *= a;
            a *= a;
            n >>= 1;
        }
        return result;
    }

    FastModint inv() const{
        if constexpr(is_prime_constexpr(mod)){
            if(value == 0)[[unlikely]]{
                throw std::runtime_error("library assertion fault: existence violation (FastModint::inv).");
            }
            return pow(mod - 2);
        }else{
            return FastModint(inv_gcd(value, mod));
        }
    }

    FastModint& operator+=(const FastModint& rhs){
        value += rhs.value;
        if(value >= umod()) value -= umod();
        return *this;
    }

    FastModint& operator-=(const FastModint& rhs){
        if(value < rhs.value) value += umod();
        value -= rhs.value;
        return *this;
    }

    FastModint& operator*=(const FastModint& rhs){
        value = reduce(static_cast<u64>(value) * rhs.value);
        return *this;
    }

    FastModint& operator/=(const FastModint& rhs){
        return *this *= rhs.inv();
    }

    FastModint operator+() const{
        return *this;
    }

    FastModint operator-() const{
        return FastModint(0) - *this;
    }

    FastModint& operator++(){
        value++;
        if(value == umod()) value = 0;
        return *this;
    }

    FastModint& operator--(){
        if(value == 0) value = umod();
        value--;
        return *this;
    }

    FastModint operator++(int){
        FastModint result = *this;
        ++*this;
        return result;
    }

    FastModint operator--(int){
        FastModint result = *this;
        --*this;
        return result;
    }

    friend FastModint operator+(FastModint lhs, const FastModint& rhs){
        return lhs += rhs;
    }

    friend FastModint operator-(FastModint lhs, const FastModint& rhs){
        return lhs -= rhs;
    }

    friend FastModint operator*(FastModint lhs, const FastModint& rhs){
        return lhs *= rhs;
    }

    friend FastModint operator/(FastModint lhs, const FastModint& rhs){
        return lhs /= rhs;
    }

    friend bool operator==(const FastModint& lhs, const FastModint& rhs){
        return lhs.value == rhs.value;
    }

    friend bool operator!=(const FastModint& lhs, const FastModint& rhs){
        return !(lhs == rhs);
    }

    friend std::istream& operator>>(std::istream& is, FastModint& x){
        long long v;
        if(is >> v){
            x = FastModint(v);
        }else{
            throw std::runtime_error("library assertion fault: input fails (FastModint::istream).");
        }
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const FastModint& x){
        return os << x.val();
    }
};

using FastModint998244353 = FastModint<998244353>;
using FastModint1000000007 = FastModint<1000000007>;

#endif  // CPPLIB_SRC_STRUCTURE_MODINT_FAST_MODINT_HPP_INCLUDED
