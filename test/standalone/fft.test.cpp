// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <complex>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/fft/fft.hpp"

std::vector<long long> naive_ll(const std::vector<long long>& a, const std::vector<long long>& b){
    if(a.empty() || b.empty()){
        return {};
    }
    std::vector<long long> res(a.size() + b.size() - 1);
    for(std::size_t i = 0; i < a.size(); i++){
        for(std::size_t j = 0; j < b.size(); j++){
            res[i + j] += a[i] * b[j];
        }
    }
    return res;
}

std::vector<double> naive_double(const std::vector<double>& a, const std::vector<double>& b){
    if(a.empty() || b.empty()){
        return {};
    }
    std::vector<double> res(a.size() + b.size() - 1);
    for(std::size_t i = 0; i < a.size(); i++){
        for(std::size_t j = 0; j < b.size(); j++){
            res[i + j] += a[i] * b[j];
        }
    }
    return res;
}

void test_transform(){
    std::vector<std::complex<double>> a = {
        {1.0, 2.0}, {3.0, -4.0}, {-5.0, 6.0}, {7.0, 8.0},
        {-9.0, 1.0}, {2.0, -3.0}, {4.0, 5.0}, {-6.0, -7.0},
    };
    auto b = a;
    fast_fourier_transform(b);
    fast_fourier_transform(b, true);
    for(std::size_t i = 0; i < a.size(); i++){
        assert(std::abs(a[i] - b[i]) < 1e-8);
    }
}

void test_long_long(){
    for(int n = 1; n <= 20; n++){
        for(int m = 1; m <= 20; m++){
            std::vector<long long> a(n), b(m);
            for(int i = 0; i < n; i++){
                a[i] = i * i - 7 * i + 3;
            }
            for(int i = 0; i < m; i++){
                b[i] = i * 5 - 11;
            }
            assert(convolution_ll(a, b) == naive_ll(a, b));
        }
    }
}

void test_double(){
    std::vector<double> a = {0.5, -1.25, 3.0, 2.75};
    std::vector<double> b = {-2.0, 4.5, 1.25};
    auto res = convolution_fft(a, b);
    auto expected = naive_double(a, b);
    assert(res.size() == expected.size());
    for(std::size_t i = 0; i < res.size(); i++){
        assert(std::abs(res[i] - expected[i]) < 1e-8);
    }
}

void test_empty(){
    assert(convolution_ll({}, {1, 2, 3}).empty());
    assert(convolution_fft({}, {1.0, 2.0}).empty());
}

void test_exceptions(){
    bool thrown = false;
    try{
        std::vector<std::complex<double>> a(3);
        fast_fourier_transform(a);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    test_transform();
    test_long_long();
    test_double();
    test_empty();
    test_exceptions();
}
