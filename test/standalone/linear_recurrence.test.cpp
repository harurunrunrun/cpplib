// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/math/linear_recurrence.hpp"
#include "../../src/structure/modint/modint.hpp"

using mint = Modint998244353;

void self_test(){
    const mint initial[] = {mint(0), mint(1)};
    const mint coefficient[] = {mint(1), mint(1)};
    const std::vector<mint> sequence = {
        mint(0), mint(1), mint(1), mint(2),
        mint(3), mint(5), mint(8), mint(13)
    };

    const LinearRecurrence<mint, 8> pointer_recurrence(
        initial, coefficient, 2
    );
    assert(pointer_recurrence.degree() == 2);
    assert(pointer_recurrence.initial_value(0) == mint(0));
    assert(pointer_recurrence.initial_value(1) == mint(1));
    assert(pointer_recurrence.coefficient(0) == mint(1));
    assert(pointer_recurrence.coefficient(1) == mint(1));
    assert(pointer_recurrence.kth_term(20) == mint(6765));

    const LinearRecurrence<mint, 8> vector_recurrence(
        std::vector<mint>{mint(0), mint(1)},
        std::vector<mint>{mint(1), mint(1)}
    );
    assert(vector_recurrence.kth_term(20) == mint(6765));

    const auto pointer_inferred = berlekamp_massey<mint, 8>(
        sequence.data(), static_cast<int>(sequence.size())
    );
    const auto vector_inferred = berlekamp_massey<mint, 8>(sequence);
    assert(pointer_inferred.degree() == 2);
    assert(vector_inferred.degree() == 2);
    assert(pointer_inferred.kth_term(20) == mint(6765));
    assert(vector_inferred.kth_term(20) == mint(6765));
    assert(bostan_mori(
        std::vector<mint>{mint(0), mint(1)},
        std::vector<mint>{mint(1), mint(1)},
        20
    ) == mint(6765));

    bool thrown = false;
    try{
        (void)pointer_recurrence.initial_value(-1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)pointer_recurrence.coefficient(2);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)LinearRecurrence<mint, 8>(nullptr, coefficient, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();
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
