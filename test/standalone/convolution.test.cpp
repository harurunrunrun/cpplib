// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/fft/convolution.hpp"

template<int MOD>
std::vector<Modint<MOD>> naive_convolution(const std::vector<Modint<MOD>>& a, const std::vector<Modint<MOD>>& b){
    if(a.empty() || b.empty()){
        return {};
    }
    std::vector<Modint<MOD>> res(a.size() + b.size() - 1);
    for(std::size_t i = 0; i < a.size(); i++){
        for(std::size_t j = 0; j < b.size(); j++){
            res[i + j] += a[i] * b[j];
        }
    }
    return res;
}

template<int MOD>
void assert_equal(const std::vector<Modint<MOD>>& a, const std::vector<Modint<MOD>>& b){
    assert(a.size() == b.size());
    for(std::size_t i = 0; i < a.size(); i++){
        assert(a[i] == b[i]);
    }
}

void test_ntt(){
    using mint = Modint<998244353>;
    std::vector<mint> a = {1, 2, 3, 4, 5};
    std::vector<mint> b = {6, 7, 8};
    assert_equal(convolution_ntt(a, b), naive_convolution(a, b));

    for(int n = 1; n <= 16; n++){
        for(int m = 1; m <= 16; m++){
            a.assign(n, 0);
            b.assign(m, 0);
            for(int i = 0; i < n; i++){
                a[i] = i * i - 3 * i + 7;
            }
            for(int i = 0; i < m; i++){
                b[i] = i * 5 - 4;
            }
            assert_equal(convolution_ntt(a, b), naive_convolution(a, b));
        }
    }
}

void test_arbitrary_mod(){
    using mint = Modint<1000000007>;
    for(int n = 1; n <= 18; n++){
        for(int m = 1; m <= 18; m++){
            std::vector<mint> a(n), b(m);
            for(int i = 0; i < n; i++){
                a[i] = (long long)i * i * i - 9 * i + 11;
            }
            for(int i = 0; i < m; i++){
                b[i] = (long long)i * i - 8 * i - 5;
            }
            assert_equal(arbitrary_mod_convolution(a, b), naive_convolution(a, b));
        }
    }
}

void test_empty(){
    using mint = Modint<998244353>;
    assert(convolution_ntt(std::vector<mint>{}, std::vector<mint>{1}).empty());
    assert(arbitrary_mod_convolution(std::vector<Modint<1000000007>>{}, std::vector<Modint<1000000007>>{1}).empty());
}

void test_transform(){
    using mint = Modint<998244353>;
    std::vector<mint> a = {1, 2, 3, 4, 5, 6, 7, 8};
    auto b = a;
    number_theoretic_transform(b);
    number_theoretic_transform(b, true);
    assert_equal(a, b);
}

void test_exceptions(){
    using mint = Modint<998244353>;
    bool thrown = false;
    try{
        std::vector<mint> a = {1, 2, 3};
        number_theoretic_transform(a);
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
            int n, m;
            std::cin >> type >> n >> m;
            if(type == "NTT"){
                using mint = Modint<998244353>;
                std::vector<mint> a(static_cast<std::size_t>(n));
                std::vector<mint> b(static_cast<std::size_t>(m));
                for(auto& value: a) std::cin >> value;
                for(auto& value: b) std::cin >> value;
                const auto result = convolution_ntt(a, b);
                std::cout << result.size();
                for(const auto& value: result) std::cout << ' ' << value;
                std::cout << '\n';
            }else{
                using mint = Modint<1000000007>;
                std::vector<mint> a(static_cast<std::size_t>(n));
                std::vector<mint> b(static_cast<std::size_t>(m));
                for(auto& value: a) std::cin >> value;
                for(auto& value: b) std::cin >> value;
                const auto result = arbitrary_mod_convolution(a, b);
                std::cout << result.size();
                for(const auto& value: result) std::cout << ' ' << value;
                std::cout << '\n';
            }
        }
        return 0;
    }

    test_ntt();
    test_arbitrary_mod();
    test_empty();
    test_transform();
    test_exceptions();
}
