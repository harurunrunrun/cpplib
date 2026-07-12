#pragma once

#include <cmath>
#include <complex>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

namespace fft_internal{

inline bool is_power_of_two(std::size_t n){
    return n > 0 && (n & (n - 1)) == 0;
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

} // namespace fft_internal

void fast_fourier_transform(std::vector<std::complex<double>>& a, bool inverse = false){
    const int n = (int)a.size();
    if(!fft_internal::is_power_of_two(a.size()))[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid size (fast_fourier_transform).");
    }

    fft_internal::bit_reverse_permute(a);
    const double pi = std::acos(-1.0);
    for(int len = 2; len <= n; len <<= 1){
        const double angle = (inverse ? -2.0 : 2.0) * pi / len;
        const std::complex<double> wlen(std::cos(angle), std::sin(angle));
        for(int l = 0; l < n; l += len){
            std::complex<double> w(1.0, 0.0);
            const int half = len >> 1;
            for(int i = 0; i < half; i++){
                const auto u = a[l + i];
                const auto v = a[l + half + i] * w;
                a[l + i] = u + v;
                a[l + half + i] = u - v;
                w *= wlen;
            }
        }
    }

    if(inverse){
        for(auto& x: a){
            x /= n;
        }
    }
}

std::vector<double> convolution_fft(const std::vector<double>& a, const std::vector<double>& b){
    if(a.empty() || b.empty()){
        return {};
    }

    const int need = (int)a.size() + (int)b.size() - 1;
    int n = 1;
    while(n < need){
        n <<= 1;
    }

    std::vector<std::complex<double>> fa(n), fb(n);
    for(std::size_t i = 0; i < a.size(); i++){
        fa[i] = a[i];
    }
    for(std::size_t i = 0; i < b.size(); i++){
        fb[i] = b[i];
    }
    fast_fourier_transform(fa);
    fast_fourier_transform(fb);
    for(int i = 0; i < n; i++){
        fa[i] *= fb[i];
    }
    fast_fourier_transform(fa, true);

    std::vector<double> res(need);
    for(int i = 0; i < need; i++){
        res[i] = fa[i].real();
    }
    return res;
}

std::vector<long long> convolution_ll(const std::vector<long long>& a, const std::vector<long long>& b){
    std::vector<double> da(a.begin(), a.end());
    std::vector<double> db(b.begin(), b.end());
    auto c = convolution_fft(da, db);
    std::vector<long long> res(c.size());
    for(std::size_t i = 0; i < c.size(); i++){
        res[i] = std::llround(c[i]);
    }
    return res;
}
