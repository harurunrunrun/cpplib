#pragma once

#include "common_monoids.hpp"

template<class T>
struct SumSquaresAggregate{
    T sum{};
    T square_sum{};
    long long length = 0;

    friend constexpr bool operator==(
        const SumSquaresAggregate&,
        const SumSquaresAggregate&
    ) = default;
};

template<class T>
constexpr SumSquaresAggregate<T> sum_squares_singleton(T value){
    return {value, value * value, 1};
}

template<class T>
constexpr MonoidAffine<T> sum_squares_assign(T value){
    return {T(0), value};
}

template<class T>
constexpr MonoidAffine<T> sum_squares_add(T value){
    return {T(1), value};
}

namespace sum_squares_monoid_internal{

template<class T>
constexpr SumSquaresAggregate<T> sum_squares_op(
    SumSquaresAggregate<T> left,
    SumSquaresAggregate<T> right
){
    return {
        left.sum + right.sum,
        left.square_sum + right.square_sum,
        left.length + right.length
    };
}

template<class T>
constexpr SumSquaresAggregate<T> sum_squares_identity(){
    return {};
}

template<class T>
constexpr SumSquaresAggregate<T> affine_sum_squares_mapping(
    MonoidAffine<T> operation,
    SumSquaresAggregate<T> value
){
    const T length = static_cast<T>(value.length);
    return {
        operation.multiplier * value.sum + operation.addend * length,
        operation.multiplier * operation.multiplier * value.square_sum
            + T(2) * operation.multiplier * operation.addend * value.sum
            + operation.addend * operation.addend * length,
        value.length
    };
}

} // namespace sum_squares_monoid_internal

template<class T>
using AffineSumSquaresMonoidAct = Monoid_Act<
    sum_squares_monoid_internal::sum_squares_op<T>,
    sum_squares_monoid_internal::sum_squares_identity<T>,
    sum_squares_monoid_internal::affine_sum_squares_mapping<T>,
    common_monoid_internal::affine_composition<T>,
    common_monoid_internal::affine_id<T>
>;
