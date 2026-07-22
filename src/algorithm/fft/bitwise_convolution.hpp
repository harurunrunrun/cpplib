#ifndef CPPLIB_SRC_ALGORITHM_FFT_BITWISE_CONVOLUTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_FFT_BITWISE_CONVOLUTION_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace bitwise_convolution_internal{

inline bool is_power_of_two(std::size_t n){
    return n > 0 && (n & (n - 1)) == 0;
}

inline void assert_same_power_of_two(std::size_t n, std::size_t m, const char* name){
    if(n != m || !is_power_of_two(n))[[unlikely]]{
        throw std::runtime_error(name);
    }
}

} // namespace bitwise_convolution_internal

template<class T>
void or_zeta_transform(std::vector<T>& a){
    const std::size_t n = a.size();
    if(!bitwise_convolution_internal::is_power_of_two(n))[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid size (or_zeta_transform).");
    }
    for(std::size_t bit = 1; bit < n; bit <<= 1){
        for(std::size_t mask = 0; mask < n; mask++){
            if(mask & bit){
                a[mask] += a[mask ^ bit];
            }
        }
    }
}

template<class T>
void or_mobius_transform(std::vector<T>& a){
    const std::size_t n = a.size();
    if(!bitwise_convolution_internal::is_power_of_two(n))[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid size (or_mobius_transform).");
    }
    for(std::size_t bit = 1; bit < n; bit <<= 1){
        for(std::size_t mask = 0; mask < n; mask++){
            if(mask & bit){
                a[mask] -= a[mask ^ bit];
            }
        }
    }
}

template<class T>
void and_zeta_transform(std::vector<T>& a){
    const std::size_t n = a.size();
    if(!bitwise_convolution_internal::is_power_of_two(n))[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid size (and_zeta_transform).");
    }
    for(std::size_t bit = 1; bit < n; bit <<= 1){
        for(std::size_t mask = 0; mask < n; mask++){
            if((mask & bit) == 0){
                a[mask] += a[mask | bit];
            }
        }
    }
}

template<class T>
void and_mobius_transform(std::vector<T>& a){
    const std::size_t n = a.size();
    if(!bitwise_convolution_internal::is_power_of_two(n))[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid size (and_mobius_transform).");
    }
    for(std::size_t bit = 1; bit < n; bit <<= 1){
        for(std::size_t mask = 0; mask < n; mask++){
            if((mask & bit) == 0){
                a[mask] -= a[mask | bit];
            }
        }
    }
}

template<class T>
void walsh_hadamard_transform(std::vector<T>& a, bool inverse = false){
    const std::size_t n = a.size();
    if(!bitwise_convolution_internal::is_power_of_two(n))[[unlikely]]{
        throw std::runtime_error("library assertion fault: invalid size (walsh_hadamard_transform).");
    }
    for(std::size_t bit = 1; bit < n; bit <<= 1){
        for(std::size_t block = 0; block < n; block += bit << 1){
            for(std::size_t i = 0; i < bit; i++){
                const T x = a[block + i];
                const T y = a[block + bit + i];
                a[block + i] = x + y;
                a[block + bit + i] = x - y;
            }
        }
    }
    if(inverse){
        for(T& x: a){
            x /= T(n);
        }
    }
}

template<class T>
std::vector<T> or_convolution(std::vector<T> a, std::vector<T> b){
    bitwise_convolution_internal::assert_same_power_of_two(
        a.size(), b.size(), "library assertion fault: invalid size (or_convolution).");
    or_zeta_transform(a);
    or_zeta_transform(b);
    for(std::size_t i = 0; i < a.size(); i++){
        a[i] *= b[i];
    }
    or_mobius_transform(a);
    return a;
}

template<class T>
std::vector<T> and_convolution(std::vector<T> a, std::vector<T> b){
    bitwise_convolution_internal::assert_same_power_of_two(
        a.size(), b.size(), "library assertion fault: invalid size (and_convolution).");
    and_zeta_transform(a);
    and_zeta_transform(b);
    for(std::size_t i = 0; i < a.size(); i++){
        a[i] *= b[i];
    }
    and_mobius_transform(a);
    return a;
}

template<class T>
std::vector<T> xor_convolution(std::vector<T> a, std::vector<T> b){
    bitwise_convolution_internal::assert_same_power_of_two(
        a.size(), b.size(), "library assertion fault: invalid size (xor_convolution).");
    walsh_hadamard_transform(a);
    walsh_hadamard_transform(b);
    for(std::size_t i = 0; i < a.size(); i++){
        a[i] *= b[i];
    }
    walsh_hadamard_transform(a, true);
    return a;
}

template<class T>
std::vector<T> xnor_convolution(const std::vector<T>& a, const std::vector<T>& b){
    std::vector<T> x = xor_convolution(a, b);
    const std::size_t mask = x.size() - 1;
    std::vector<T> res(x.size());
    for(std::size_t i = 0; i < x.size(); i++){
        res[i] = x[mask ^ i];
    }
    return res;
}

#endif  // CPPLIB_SRC_ALGORITHM_FFT_BITWISE_CONVOLUTION_HPP_INCLUDED
