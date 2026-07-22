#ifndef CPPLIB_SRC_STRUCTURE_DSU_MODULAR_POTENTIAL_DSU_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_DSU_MODULAR_POTENTIAL_DSU_HPP_INCLUDED

#include <optional>

#include "weighted_dsu.hpp"

template<int MOD>
struct ModularPotentialValue{
    static_assert(MOD > 0);
    int value = 0;

    ModularPotentialValue(long long x = 0): value(static_cast<int>(x % MOD)){
        if(value < 0) value += MOD;
    }

    friend ModularPotentialValue operator+(
        ModularPotentialValue left,
        const ModularPotentialValue& right
    ){
        left.value = static_cast<int>(
            (static_cast<long long>(left.value) + right.value) % MOD
        );
        return left;
    }

    friend ModularPotentialValue operator-(
        ModularPotentialValue left,
        const ModularPotentialValue& right
    ){
        left.value = static_cast<int>(
            (static_cast<long long>(left.value) - right.value + MOD) % MOD
        );
        return left;
    }

    friend bool operator==(
        const ModularPotentialValue& left,
        const ModularPotentialValue& right
    ){
        return left.value == right.value;
    }
};

template<int MOD, int MAX_SIZE>
class ModularPotentialDSU{
    using Potential = ModularPotentialValue<MOD>;
    WeightedDSU<Potential, MAX_SIZE> dsu_;

public:
    explicit ModularPotentialDSU(int n = MAX_SIZE): dsu_(n){}

    int size() const{ return dsu_.size(); }
    int groups() const{ return dsu_.groups(); }

    bool merge(int u, int v, long long difference){
        return dsu_.merge(v, u, Potential(difference));
    }

    std::optional<int> difference(int u, int v){
        const auto result = dsu_.diff_if_connected(v, u);
        if(!result) return std::nullopt;
        return result->value;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_DSU_MODULAR_POTENTIAL_DSU_HPP_INCLUDED
