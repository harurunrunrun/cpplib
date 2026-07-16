#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../structure/modint/modint.hpp"

namespace convolution_internal{

inline bool is_power_of_two(std::size_t n){
    return n > 0 && (n & (n - 1)) == 0;
}

inline long long pow_mod_long(long long x, long long n, int mod){
    long long res = 1 % mod;
    while(n > 0){
        if(n & 1){
            res = static_cast<long long>(
                static_cast<__int128>(res) * x % mod
            );
        }
        x = static_cast<long long>(
            static_cast<__int128>(x) * x % mod
        );
        n >>= 1;
    }
    return res;
}

inline int primitive_root(int mod){
    if(mod == 2){
        return 1;
    }
    if(mod == 167772161){
        return 3;
    }
    if(mod == 469762049){
        return 3;
    }
    if(mod == 754974721){
        return 11;
    }
    if(mod == 998244353){
        return 3;
    }

    int x = mod - 1;
    std::vector<int> factors;
    for(int p = 2; static_cast<long long>(p) * p <= x; ++p){
        if(x % p == 0){
            factors.push_back((int)p);
            while(x % p == 0){
                x /= p;
            }
        }
    }
    if(x > 1){
        factors.push_back(x);
    }

    for(int g = 2; ; g++){
        bool ok = true;
        for(int p: factors){
            if(pow_mod_long(g, (mod - 1) / p, mod) == 1){
                ok = false;
                break;
            }
        }
        if(ok){
            return g;
        }
    }
}

template<int MOD>
int primitive_root(){
    static const int g = primitive_root(MOD);
    return g;
}

template<class T>
void bit_reverse_permute(std::vector<T>& a){
    const int n = (int)a.size();
    for(int i = 1, j = 0; i < n; i++){
        int bit = n >> 1;
        for(; j & bit; bit >>= 1){
            j ^= bit;
        }
        j ^= bit;
        if(i < j){
            std::swap(a[i], a[j]);
        }
    }
}

} // namespace convolution_internal

template<int MOD>
void number_theoretic_transform(std::vector<Modint<MOD>>& a, bool inverse = false){
    static_assert(MOD >= 2);
    const int n = (int)a.size();
    if(!convolution_internal::is_power_of_two(a.size()))[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid size (number_theoretic_transform).");
    }
    if((MOD - 1) % n != 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: transform size is too large (number_theoretic_transform).");
    }

    using mint = Modint<MOD>;
    convolution_internal::bit_reverse_permute(a);
    const mint root = mint(convolution_internal::primitive_root<MOD>());
    for(int len = 2; len <= n; len <<= 1){
        mint wlen = root.pow((MOD - 1) / len);
        if(inverse){
            wlen = wlen.inv();
        }
        for(int l = 0; l < n; l += len){
            mint w = 1;
            const int half = len >> 1;
            for(int i = 0; i < half; i++){
                const mint u = a[l + i];
                const mint v = a[l + half + i] * w;
                a[l + i] = u + v;
                a[l + half + i] = u - v;
                w *= wlen;
            }
        }
    }

    if(inverse){
        const mint inv_n = mint(n).inv();
        for(mint& x: a){
            x *= inv_n;
        }
    }
}

template<int MOD>
std::vector<Modint<MOD>> convolution_ntt(std::vector<Modint<MOD>> a, std::vector<Modint<MOD>> b){
    static_assert(MOD >= 2);
    if(a.empty() || b.empty()){
        return {};
    }
    const int need = (int)a.size() + (int)b.size() - 1;
    int n = 1;
    while(n < need){
        n <<= 1;
    }
    if((MOD - 1) % n != 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: transform size is too large (convolution_ntt).");
    }

    a.resize(n);
    b.resize(n);
    number_theoretic_transform(a);
    number_theoretic_transform(b);
    for(int i = 0; i < n; i++){
        a[i] *= b[i];
    }
    number_theoretic_transform(a, true);
    a.resize(need);
    return a;
}

template<int MOD>
std::vector<Modint<MOD>> arbitrary_mod_convolution(const std::vector<Modint<MOD>>& a, const std::vector<Modint<MOD>>& b){
    if(a.empty() || b.empty()){
        return {};
    }

    if constexpr(MOD == 167772161 || MOD == 469762049 || MOD == 754974721 || MOD == 998244353){
        return convolution_ntt<MOD>(a, b);
    }else{
        constexpr int M1 = 754974721;
        constexpr int M2 = 167772161;
        constexpr int M3 = 469762049;
        using mint1 = Modint<M1>;
        using mint2 = Modint<M2>;
        using mint3 = Modint<M3>;
        using mint = Modint<MOD>;

        std::vector<mint1> a1(a.size()), b1(b.size());
        std::vector<mint2> a2(a.size()), b2(b.size());
        std::vector<mint3> a3(a.size()), b3(b.size());
        for(std::size_t i = 0; i < a.size(); i++){
            a1[i] = mint1(a[i].val());
            a2[i] = mint2(a[i].val());
            a3[i] = mint3(a[i].val());
        }
        for(std::size_t i = 0; i < b.size(); i++){
            b1[i] = mint1(b[i].val());
            b2[i] = mint2(b[i].val());
            b3[i] = mint3(b[i].val());
        }

        auto c1 = convolution_ntt(a1, b1);
        auto c2 = convolution_ntt(a2, b2);
        auto c3 = convolution_ntt(a3, b3);

        const long long inv_m1_mod_m2 = mint2(M1).inv().val();
        const long long inv_m1m2_mod_m3 = (mint3(M1) * mint3(M2)).inv().val();
        const long long m1_mod_m3 = M1 % M3;
        const long long m1_mod_target = MOD == 1 ? 0 : M1 % MOD;
        const long long m1m2_mod_target = MOD == 1 ? 0 : (long long)((__int128)M1 * M2 % MOD);

        std::vector<mint> res(c1.size());
        for(std::size_t i = 0; i < c1.size(); i++){
            const long long x1 = c1[i].val();
            long long t2 = (c2[i].val() - x1) % M2;
            if(t2 < 0){
                t2 += M2;
            }
            t2 = t2 * inv_m1_mod_m2 % M2;

            const long long x12_mod_m3 = static_cast<long long>(
                (x1 + static_cast<__int128>(m1_mod_m3) * t2) % M3
            );
            long long t3 = (c3[i].val() - x12_mod_m3) % M3;
            if(t3 < 0){
                t3 += M3;
            }
            t3 = t3 * inv_m1m2_mod_m3 % M3;

            res[i] = mint(x1) + mint(m1_mod_target) * mint(t2) + mint(m1m2_mod_target) * mint(t3);
        }
        return res;
    }
}
