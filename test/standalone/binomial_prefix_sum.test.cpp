// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <string>
#include "../../src/algorithm/math/combinatorics/binomial_prefix_sum.hpp"
#include "../../src/structure/modint/modint.hpp"

using mint = Modint<998244353>;

long long comb_naive(int n, int k){
    if(k < 0 || n < k) return 0;
    long long result = 1;
    for(int i = 1; i <= k; i++){
        result = result * (n - i + 1) / i;
    }
    return result;
}

int main(){
    int q;
    if(std::cin >> q){
        BinomialPrefixSum<mint, 200> table;
        while(q--){
            std::string type;
            int n, k;
            std::cin >> type >> n >> k;
            if(type == "COMB") std::cout << table.combination(n, k) << '\n';
            else std::cout << table.prefix_sum(n, k) << '\n';
        }
        return 0;
    }

    BinomialPrefixSum<mint, 30> table;
    for(int n = 0; n <= 30; n++){
        long long prefix = 0;
        for(int k = 0; k <= 35; k++){
            if(k <= n) prefix += comb_naive(n, k);
            assert(table.combination(n, k).val() == comb_naive(n, k));
            assert(table.prefix_sum(n, k).val() == prefix % 998244353);
        }
        assert(table.prefix_sum(n, -1).val() == 0);
    }
    assert(table.prefix_sum(5, 2).val() == 16);
    assert(table.prefix_sum(5, 5).val() == 32);
}
