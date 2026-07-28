// competitive-verifier: STANDALONE

#include <cmath>
#include <complex>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/fft/transform/bluestein_fft.hpp"
#include "../../src/algorithm/fft/transform/chirp_z_transform.hpp"
#include "../../src/algorithm/math/polynomial/polynomial_middle_product.hpp"

using mint = Modint998244353;

void check_bluestein(){
    std::mt19937_64 random(20260728);
    const double pi = std::acos(-1.0);
    for(std::size_t size = 1; size <= 25; ++size){
        std::vector<std::complex<double>> values(size);
        for(auto& value: values){
            value = {
                static_cast<double>(static_cast<int>(random() % 21) - 10),
                static_cast<double>(static_cast<int>(random() % 21) - 10)
            };
        }
        for(const bool inverse: {false, true}){
            std::vector<std::complex<double>> expected(size);
            const double sign = inverse ? -1.0 : 1.0;
            for(std::size_t frequency = 0; frequency < size; ++frequency){
                for(std::size_t index = 0; index < size; ++index){
                    const double angle = sign * 2.0 * pi
                        * static_cast<double>(frequency * index)
                        / static_cast<double>(size);
                    expected[frequency] += values[index]
                        * std::complex<double>(std::cos(angle), std::sin(angle));
                }
                if(inverse) expected[frequency] /= static_cast<double>(size);
            }
            std::vector<std::complex<double>> actual = values;
            bluestein_fourier_transform(actual, inverse);
            for(std::size_t index = 0; index < size; ++index){
                if(std::abs(actual[index] - expected[index]) > 1e-7 * size){
                    throw std::runtime_error("Bluestein FFT mismatch");
                }
            }
        }
    }
}

void print(const std::vector<mint>& values){
    for(std::size_t index = 0; index < values.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << values[index];
    }
    std::cout << '\n';
}

int main(){
    check_bluestein();
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int kind;
        std::size_t first_size;
        std::cin >> kind >> first_size;
        std::vector<mint> first(first_size);
        for(mint& value: first) std::cin >> value;
        if(kind == 0){
            std::size_t second_size;
            std::cin >> second_size;
            std::vector<mint> second(second_size);
            for(mint& value: second) std::cin >> value;
            print(math::polynomial_middle_product<998244353>(first, second));
        }else{
            std::size_t count;
            mint initial;
            mint ratio;
            std::cin >> count >> initial >> ratio;
            print(chirp_z_transform<998244353>(
                first, initial, ratio, count
            ));
        }
    }
}
