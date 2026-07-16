// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/fft/polya_exponential.hpp"

namespace{

using mint = Modint<998244353>;

void self_test(){
    const std::vector<mint> atom{0, 1, 0, 0, 0, 0};
    const std::vector<mint> partitions = polya_exponential<998244353>(atom);
    const int expected[] = {1, 1, 1, 1, 1, 1};
    for(std::size_t index = 0; index < partitions.size(); ++index){
        assert(partitions[index] == mint(expected[index]));
    }
    assert(polya_logarithm<998244353>(partitions) == atom);
    assert(polya_exponential<998244353>({}, 0).empty());

    bool threw = false;
    try{
        (void)polya_exponential<998244353>({mint(1)}, 1);
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
}

void print(const std::vector<mint>& values){
    for(std::size_t index = 0; index < values.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << values[index];
    }
    std::cout << '\n';
}

} // namespace

int main(){
    self_test();
    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- > 0){
        std::size_t size;
        std::cin >> size;
        std::vector<mint> values(size);
        for(mint& value: values) std::cin >> value;
        const std::vector<mint> exponential =
            polya_exponential<998244353>(values, size);
        print(exponential);
        print(polya_logarithm<998244353>(exponential, size));
    }
}
