// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/fft/bitwise_convolution.hpp"
#include "../../src/structure/modint/modint.hpp"

using mint = Modint<998244353>;

std::vector<long long> naive_or(const std::vector<long long>& a, const std::vector<long long>& b){
    std::vector<long long> res(a.size());
    for(std::size_t i = 0; i < a.size(); i++){
        for(std::size_t j = 0; j < b.size(); j++){
            res[i | j] += a[i] * b[j];
        }
    }
    return res;
}

std::vector<long long> naive_and(const std::vector<long long>& a, const std::vector<long long>& b){
    std::vector<long long> res(a.size());
    for(std::size_t i = 0; i < a.size(); i++){
        for(std::size_t j = 0; j < b.size(); j++){
            res[i & j] += a[i] * b[j];
        }
    }
    return res;
}

std::vector<long long> naive_xor(const std::vector<long long>& a, const std::vector<long long>& b){
    std::vector<long long> res(a.size());
    for(std::size_t i = 0; i < a.size(); i++){
        for(std::size_t j = 0; j < b.size(); j++){
            res[i ^ j] += a[i] * b[j];
        }
    }
    return res;
}

std::vector<long long> naive_xnor(const std::vector<long long>& a, const std::vector<long long>& b){
    const std::size_t mask = a.size() - 1;
    std::vector<long long> res(a.size());
    for(std::size_t i = 0; i < a.size(); i++){
        for(std::size_t j = 0; j < b.size(); j++){
            res[mask ^ (i ^ j)] += a[i] * b[j];
        }
    }
    return res;
}

void test_long_long(){
    for(int lg = 0; lg <= 4; lg++){
        const int n = 1 << lg;
        std::vector<long long> a(n), b(n);
        for(int i = 0; i < n; i++){
            a[i] = i * i - 3 * i + 5;
            b[i] = (i + 2) * (i % 3 - 1);
        }
        assert(or_convolution(a, b) == naive_or(a, b));
        assert(and_convolution(a, b) == naive_and(a, b));
        assert(xor_convolution(a, b) == naive_xor(a, b));
        assert(xnor_convolution(a, b) == naive_xnor(a, b));
    }
}

void test_modint(){
    std::vector<mint> a = {1, 2, 3, 4};
    std::vector<mint> b = {5, 6, 7, 8};
    auto res = xor_convolution(a, b);
    std::vector<long long> aa = {1, 2, 3, 4};
    std::vector<long long> bb = {5, 6, 7, 8};
    auto expected = naive_xor(aa, bb);
    for(int i = 0; i < 4; i++){
        assert(res[i].val() == expected[i]);
    }
}

void test_transform_inverse(){
    std::vector<long long> a = {1, 2, 3, 4, 5, 6, 7, 8};
    auto b = a;
    or_zeta_transform(b);
    or_mobius_transform(b);
    assert(a == b);

    b = a;
    and_zeta_transform(b);
    and_mobius_transform(b);
    assert(a == b);

    b = a;
    walsh_hadamard_transform(b);
    walsh_hadamard_transform(b, true);
    assert(a == b);
}

void test_exceptions(){
    bool thrown = false;
    try{
        (void)or_convolution(std::vector<long long>{1, 2, 3}, std::vector<long long>{1, 2, 3});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)xor_convolution(std::vector<long long>{1, 2}, std::vector<long long>{1});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int q;
    if(std::cin >> q){
        while(q--){
            std::string type;
            int lg;
            std::cin >> type >> lg;
            const int n = 1 << lg;
            std::vector<long long> a(static_cast<std::size_t>(n));
            std::vector<long long> b(static_cast<std::size_t>(n));
            for(auto& value: a) std::cin >> value;
            for(auto& value: b) std::cin >> value;
            std::vector<long long> result;
            if(type == "OR") result = or_convolution(a, b);
            if(type == "AND") result = and_convolution(a, b);
            if(type == "XOR") result = xor_convolution(a, b);
            if(type == "XNOR") result = xnor_convolution(a, b);
            for(int i = 0; i < n; i++){
                if(i != 0) std::cout << ' ';
                std::cout << result[static_cast<std::size_t>(i)];
            }
            std::cout << '\n';
        }
        return 0;
    }

    test_long_long();
    test_modint();
    test_transform_inverse();
    test_exceptions();
}
