#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

template<class T>
std::vector<T> gcd_convolution(const std::vector<T>& a, const std::vector<T>& b){
    if(a.size() != b.size())[[unlikely]]{
        throw std::runtime_error("library assertion fault: size violation (gcd_convolution).");
    }
    const int n = static_cast<int>(a.size());
    std::vector<T> transformed_a(static_cast<std::size_t>(n + 1));
    std::vector<T> transformed_b(static_cast<std::size_t>(n + 1));
    for(int divisor = 1; divisor <= n; divisor++){
        for(int multiple = divisor; multiple <= n; multiple += divisor){
            transformed_a[static_cast<std::size_t>(divisor)] +=
                a[static_cast<std::size_t>(multiple - 1)];
            transformed_b[static_cast<std::size_t>(divisor)] +=
                b[static_cast<std::size_t>(multiple - 1)];
        }
        transformed_a[static_cast<std::size_t>(divisor)] *=
            transformed_b[static_cast<std::size_t>(divisor)];
    }
    for(int divisor = n; divisor >= 1; divisor--){
        for(int multiple = divisor + divisor; multiple <= n; multiple += divisor){
            transformed_a[static_cast<std::size_t>(divisor)] -=
                transformed_a[static_cast<std::size_t>(multiple)];
        }
    }
    std::vector<T> result(static_cast<std::size_t>(n));
    for(int value = 1; value <= n; value++){
        result[static_cast<std::size_t>(value - 1)] =
            transformed_a[static_cast<std::size_t>(value)];
    }
    return result;
}

template<class T>
std::vector<T> lcm_convolution(const std::vector<T>& a, const std::vector<T>& b){
    if(a.size() != b.size())[[unlikely]]{
        throw std::runtime_error("library assertion fault: size violation (lcm_convolution).");
    }
    const int n = static_cast<int>(a.size());
    std::vector<T> transformed_a(static_cast<std::size_t>(n + 1));
    std::vector<T> transformed_b(static_cast<std::size_t>(n + 1));
    for(int divisor = 1; divisor <= n; divisor++){
        const T& value_a = a[static_cast<std::size_t>(divisor - 1)];
        const T& value_b = b[static_cast<std::size_t>(divisor - 1)];
        for(int multiple = divisor; multiple <= n; multiple += divisor){
            transformed_a[static_cast<std::size_t>(multiple)] += value_a;
            transformed_b[static_cast<std::size_t>(multiple)] += value_b;
        }
    }
    for(int value = 1; value <= n; value++){
        transformed_a[static_cast<std::size_t>(value)] *=
            transformed_b[static_cast<std::size_t>(value)];
    }
    for(int divisor = 1; divisor <= n; divisor++){
        for(int multiple = divisor + divisor; multiple <= n; multiple += divisor){
            transformed_a[static_cast<std::size_t>(multiple)] -=
                transformed_a[static_cast<std::size_t>(divisor)];
        }
    }
    std::vector<T> result(static_cast<std::size_t>(n));
    for(int value = 1; value <= n; value++){
        result[static_cast<std::size_t>(value - 1)] =
            transformed_a[static_cast<std::size_t>(value)];
    }
    return result;
}
