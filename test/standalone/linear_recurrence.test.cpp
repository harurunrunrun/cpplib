// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/math/linear_recurrence.hpp"
#include "../../src/structure/modint/modint.hpp"

using mint = Modint998244353;

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string type;
    int size;
    std::uint64_t index;
    std::cin >> type >> size >> index;
    std::vector<mint> sequence(static_cast<std::size_t>(size));
    for(auto& value: sequence) std::cin >> value;

    if(type == "KNOWN"){
        std::vector<mint> coefficient(static_cast<std::size_t>(size));
        for(auto& value: coefficient) std::cin >> value;
        const LinearRecurrence<mint, 128> recurrence(sequence, coefficient);
        const mint fixed_answer = recurrence.kth_term(index);
        const mint ntt_answer = bostan_mori(sequence, coefficient, index);
        assert(fixed_answer == ntt_answer);
        std::cout << fixed_answer << '\n';
    }else{
        const auto recurrence = berlekamp_massey<mint, 128>(sequence);
        std::cout << recurrence.kth_term(index) << '\n';
    }
}
