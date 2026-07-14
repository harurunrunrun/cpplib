#pragma once

#include <array>
#include <cstddef>

#include "monoid_act.hpp"

template<int MOD>
struct ResidueShift{
    static_assert(MOD > 0);

private:
    static constexpr int normalize(long long delta){
        long long result = delta % static_cast<long long>(MOD);
        if(result < 0) result += static_cast<long long>(MOD);
        return static_cast<int>(result);
    }

public:
    int shift = 0;

    constexpr ResidueShift() = default;
    constexpr ResidueShift(long long delta): shift(normalize(delta)){}

    friend constexpr bool operator==(
        const ResidueShift&,
        const ResidueShift&
    ) = default;
};

template<class Count, int MOD>
struct ResidueCountAggregate{
    static_assert(MOD > 0);

    std::array<Count, static_cast<std::size_t>(MOD)> count{};

    friend constexpr bool operator==(
        const ResidueCountAggregate&,
        const ResidueCountAggregate&
    ) = default;
};

template<class Count, int MOD>
constexpr ResidueCountAggregate<Count, MOD>
residue_count_singleton(long long value){
    ResidueCountAggregate<Count, MOD> result;
    const int residue = ResidueShift<MOD>{value}.shift;
    result.count[static_cast<std::size_t>(residue)] = Count(1);
    return result;
}

namespace residue_count_monoid_internal{

template<class Count, int MOD>
constexpr ResidueCountAggregate<Count, MOD> residue_count_op(
    ResidueCountAggregate<Count, MOD> left,
    ResidueCountAggregate<Count, MOD> right
){
    for(int residue = 0; residue < MOD; ++residue){
        const std::size_t index = static_cast<std::size_t>(residue);
        left.count[index] += right.count[index];
    }
    return left;
}

template<class Count, int MOD>
constexpr ResidueCountAggregate<Count, MOD> residue_count_identity(){
    return {};
}

template<class Count, int MOD>
constexpr ResidueCountAggregate<Count, MOD> residue_shift_mapping(
    ResidueShift<MOD> action,
    ResidueCountAggregate<Count, MOD> value
){
    ResidueCountAggregate<Count, MOD> result;
    const int normalized_shift = ResidueShift<MOD>{action.shift}.shift;
    for(int residue = 0; residue < MOD; ++residue){
        const long long shifted =
            static_cast<long long>(residue) + normalized_shift;
        const int destination = static_cast<int>(
            shifted % static_cast<long long>(MOD)
        );
        result.count[static_cast<std::size_t>(destination)] =
            value.count[static_cast<std::size_t>(residue)];
    }
    return result;
}

template<int MOD>
constexpr ResidueShift<MOD> residue_shift_composition(
    ResidueShift<MOD> newer,
    ResidueShift<MOD> older
){
    return ResidueShift<MOD>{
        static_cast<long long>(ResidueShift<MOD>{newer.shift}.shift)
        + ResidueShift<MOD>{older.shift}.shift
    };
}

template<int MOD>
constexpr ResidueShift<MOD> residue_shift_identity(){
    return {};
}

} // namespace residue_count_monoid_internal

template<class Count, int MOD>
using AddResidueCountMonoidAct = Monoid_Act<
    residue_count_monoid_internal::residue_count_op<Count, MOD>,
    residue_count_monoid_internal::residue_count_identity<Count, MOD>,
    residue_count_monoid_internal::residue_shift_mapping<Count, MOD>,
    residue_count_monoid_internal::residue_shift_composition<MOD>,
    residue_count_monoid_internal::residue_shift_identity<MOD>
>;
