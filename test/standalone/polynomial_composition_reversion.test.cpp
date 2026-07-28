// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/math/polynomial/polynomial_composition.hpp"
#include "../../src/algorithm/math/polynomial/polynomial_reversion.hpp"

using mint = Modint998244353;

void print(const std::vector<mint>& values){
    for(std::size_t index = 0; index < values.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << values[index];
    }
    std::cout << '\n';
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int kind;
        std::size_t coefficient_count;
        std::size_t size;
        std::cin >> kind >> coefficient_count >> size;
        std::vector<mint> function(coefficient_count);
        for(mint& coefficient: function) std::cin >> coefficient;
        if(kind == 0){
            std::size_t inner_count;
            std::cin >> inner_count;
            std::vector<mint> inner(inner_count);
            for(mint& coefficient: inner) std::cin >> coefficient;
            print(math::polynomial_compose<998244353>(
                function, inner, size
            ));
        }else{
            print(math::polynomial_reversion<998244353>(
                function, size
            ));
        }
    }
}
