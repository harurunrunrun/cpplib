#pragma once

#include <algorithm>
#include <limits>
#include <numeric>
#include <type_traits>

#include "monoid.hpp"
#include "monoid_act.hpp"
#include "monoid_act_len.hpp"

namespace common_monoid_internal{

template<class T>
constexpr T add_op(T left, T right){ return left + right; }

template<class T>
constexpr T multiply_op(T left, T right){ return left * right; }

template<class T>
constexpr T minimum_op(T left, T right){ return std::min(left, right); }

template<class T>
constexpr T maximum_op(T left, T right){ return std::max(left, right); }

template<class T>
constexpr T gcd_op(T left, T right){
    static_assert(std::is_integral_v<T>);
    return std::gcd(left, right);
}

template<class T>
constexpr T lcm_op(T left, T right){
    static_assert(std::is_integral_v<T>);
    return std::lcm(left, right);
}

template<class T>
constexpr T xor_op(T left, T right){
    static_assert(std::is_integral_v<T>);
    return left ^ right;
}

template<class T>
constexpr T bit_and_op(T left, T right){
    static_assert(std::is_integral_v<T>);
    return left & right;
}

template<class T>
constexpr T bit_or_op(T left, T right){
    static_assert(std::is_integral_v<T>);
    return left | right;
}

template<class T>
constexpr T zero(){ return T(0); }

template<class T>
constexpr T one(){ return T(1); }

template<class T>
constexpr T all_bits(){
    static_assert(std::is_integral_v<T>);
    return ~T(0);
}

template<class T, T Value>
constexpr T constant(){ return Value; }

} // namespace common_monoid_internal

template<class T>
using AddMonoid = Monoid<
    common_monoid_internal::add_op<T>,
    common_monoid_internal::zero<T>
>;

template<class T>
using MulMonoid = Monoid<
    common_monoid_internal::multiply_op<T>,
    common_monoid_internal::one<T>
>;

template<class T, T Identity = std::numeric_limits<T>::max()>
using MinMonoid = Monoid<
    common_monoid_internal::minimum_op<T>,
    common_monoid_internal::constant<T, Identity>
>;

template<class T, T Identity = std::numeric_limits<T>::lowest()>
using MaxMonoid = Monoid<
    common_monoid_internal::maximum_op<T>,
    common_monoid_internal::constant<T, Identity>
>;

template<class T>
using GcdMonoid = Monoid<
    common_monoid_internal::gcd_op<T>,
    common_monoid_internal::zero<T>
>;

template<class T>
using LcmMonoid = Monoid<
    common_monoid_internal::lcm_op<T>,
    common_monoid_internal::one<T>
>;

template<class T>
using XorMonoid = Monoid<
    common_monoid_internal::xor_op<T>,
    common_monoid_internal::zero<T>
>;

template<class T>
using BitAndMonoid = Monoid<
    common_monoid_internal::bit_and_op<T>,
    common_monoid_internal::all_bits<T>
>;

template<class T>
using BitOrMonoid = Monoid<
    common_monoid_internal::bit_or_op<T>,
    common_monoid_internal::zero<T>
>;

template<class T>
struct MonoidAssignment{
    bool assigned = false;
    T value{};

    friend constexpr bool operator==(
        const MonoidAssignment& left,
        const MonoidAssignment& right
    ){
        return left.assigned == right.assigned
            && (!left.assigned || left.value == right.value);
    }
};

template<class T>
struct MonoidAffine{
    T multiplier = T(1);
    T addend = T(0);

    friend constexpr bool operator==(
        const MonoidAffine&,
        const MonoidAffine&
    ) = default;
};

template<class T>
struct SumLengthAggregate{
    T sum{};
    long long length = 0;

    friend constexpr bool operator==(
        const SumLengthAggregate&,
        const SumLengthAggregate&
    ) = default;
};

template<class T>
struct AffineCompositionAggregate{
    MonoidAffine<T> affine{};
    long long length = 0;

    friend constexpr bool operator==(
        const AffineCompositionAggregate&,
        const AffineCompositionAggregate&
    ) = default;
};

template<class T>
struct MaxSubarrayAggregate{
    T sum{};
    T prefix{};
    T suffix{};
    T best{};

    friend constexpr bool operator==(
        const MaxSubarrayAggregate&,
        const MaxSubarrayAggregate&
    ) = default;
};

template<class T>
constexpr MaxSubarrayAggregate<T> max_subarray_singleton(T value){
    const T nonnegative = std::max(T(0), value);
    return {value, nonnegative, nonnegative, nonnegative};
}

template<class T>
struct NonEmptyMaxSubarrayAggregate{
    bool empty = true;
    T sum{};
    T prefix{};
    T suffix{};
    T best{};

    friend constexpr bool operator==(
        const NonEmptyMaxSubarrayAggregate&,
        const NonEmptyMaxSubarrayAggregate&
    ) = default;
};

template<class T>
constexpr NonEmptyMaxSubarrayAggregate<T>
non_empty_max_subarray_singleton(T value){
    return {false, value, value, value, value};
}

template<class T>
struct HistoricalMaxAggregate{
    T current{};
    T historical{};

    friend constexpr bool operator==(
        const HistoricalMaxAggregate&,
        const HistoricalMaxAggregate&
    ) = default;
};

template<class T>
constexpr HistoricalMaxAggregate<T> historical_max_singleton(T value){
    return {value, value};
}

template<class T>
struct HistoricalAdd{
    T total{};
    T peak{};

    friend constexpr bool operator==(
        const HistoricalAdd&,
        const HistoricalAdd&
    ) = default;
};

template<class T>
constexpr HistoricalAdd<T> historical_add(T delta){
    static_assert(
        std::numeric_limits<T>::is_specialized &&
        std::numeric_limits<T>::is_signed
    );
    return {delta, std::max(T(0), delta)};
}

namespace common_monoid_internal{

template<class T, T Identity>
constexpr T add_extremum_mapping(T f, T x){
    return x == Identity ? Identity : x + f;
}

template<class T>
constexpr T add_composition(T f, T g){ return f + g; }

template<class T, T Identity>
constexpr T assign_extremum_mapping(MonoidAssignment<T> f, T x){
    if(x == Identity || !f.assigned) return x;
    return f.value;
}

template<class T>
constexpr MonoidAssignment<T> assignment_composition(
    MonoidAssignment<T> f,
    MonoidAssignment<T> g
){
    return f.assigned ? f : g;
}

template<class T>
constexpr MonoidAssignment<T> assignment_id(){
    return {};
}

template<class T>
constexpr T sum_op(T left, long long, T right, long long){
    return left + right;
}

template<class T>
constexpr T add_sum_mapping(T f, T x, long long length){
    return x + f * T(length);
}

template<class T>
constexpr T assign_sum_mapping(
    MonoidAssignment<T> f,
    T x,
    long long length
){
    return f.assigned ? f.value * T(length) : x;
}

template<class T>
constexpr T affine_sum_mapping(
    MonoidAffine<T> f,
    T x,
    long long length
){
    return f.multiplier * x + f.addend * T(length);
}

template<class T>
constexpr MonoidAffine<T> affine_composition(
    MonoidAffine<T> f,
    MonoidAffine<T> g
){
    return {
        f.multiplier * g.multiplier,
        f.multiplier * g.addend + f.addend
    };
}

template<class T>
constexpr MonoidAffine<T> affine_id(){
    return {};
}

template<class T>
constexpr MonoidAffine<T> affine_sequence_op(
    MonoidAffine<T> left,
    MonoidAffine<T> right
){
    return affine_composition(right, left);
}

template<class T>
constexpr SumLengthAggregate<T> sum_length_op(
    SumLengthAggregate<T> left,
    SumLengthAggregate<T> right
){
    return {
        left.sum + right.sum,
        left.length + right.length
    };
}

template<class T>
constexpr SumLengthAggregate<T> sum_length_identity(){
    return {};
}

template<class T>
constexpr SumLengthAggregate<T> affine_sum_length_mapping(
    MonoidAffine<T> f,
    SumLengthAggregate<T> aggregate
){
    return {
        f.multiplier * aggregate.sum
            + f.addend * T(aggregate.length),
        aggregate.length
    };
}

template<class T>
constexpr MonoidAffine<T> affine_power(
    MonoidAffine<T> base,
    long long exponent
){
    MonoidAffine<T> result{};
    while(exponent > 0){
        if(exponent & 1LL){
            result = affine_composition(base, result);
        }
        base = affine_composition(base, base);
        exponent >>= 1;
    }
    return result;
}

template<class T>
constexpr AffineCompositionAggregate<T> affine_aggregate_op(
    AffineCompositionAggregate<T> left,
    AffineCompositionAggregate<T> right
){
    return {
        affine_sequence_op(left.affine, right.affine),
        left.length + right.length
    };
}

template<class T>
constexpr AffineCompositionAggregate<T> affine_aggregate_identity(){
    return {};
}

template<class T>
constexpr AffineCompositionAggregate<T> assign_affine_mapping(
    MonoidAssignment<MonoidAffine<T>> assignment,
    AffineCompositionAggregate<T> aggregate
){
    if(!assignment.assigned) return aggregate;
    return {
        affine_power(assignment.value, aggregate.length),
        aggregate.length
    };
}

template<class T>
constexpr T xor_len_op(T left, long long, T right, long long){
    static_assert(std::is_integral_v<T>);
    return left ^ right;
}

template<class T>
constexpr T xor_xor_mapping(T f, T x, long long length){
    static_assert(std::is_integral_v<T>);
    return (length & 1LL) ? (x ^ f) : x;
}

template<class T, T Identity>
constexpr T chmin_mapping(T f, T x){
    return x == Identity ? Identity : std::min(f, x);
}

template<class T, T Identity>
constexpr T chmax_mapping(T f, T x){
    return x == Identity ? Identity : std::max(f, x);
}

template<class T>
constexpr T multiply_sum_mapping(T f, T x, long long){
    return f * x;
}

template<class T>
constexpr T flip_count_mapping(bool f, T x, long long length){
    return f ? T(length) - x : x;
}

template<class T>
constexpr MaxSubarrayAggregate<T> max_subarray_op(
    MaxSubarrayAggregate<T> left,
    long long,
    MaxSubarrayAggregate<T> right,
    long long
){
    return {
        left.sum + right.sum,
        std::max(left.prefix, left.sum + right.prefix),
        std::max(right.suffix, right.sum + left.suffix),
        std::max({left.best, right.best, left.suffix + right.prefix})
    };
}

template<class T>
constexpr MaxSubarrayAggregate<T> max_subarray_identity(){
    return {};
}

template<class T>
constexpr MaxSubarrayAggregate<T> assign_max_subarray_mapping(
    MonoidAssignment<T> f,
    MaxSubarrayAggregate<T> x,
    long long length
){
    if(!f.assigned) return x;
    if(length == 0) return {};
    const T sum = f.value * T(length);
    const T nonnegative = std::max(T(0), sum);
    return {sum, nonnegative, nonnegative, nonnegative};
}

template<class T>
constexpr NonEmptyMaxSubarrayAggregate<T> non_empty_max_subarray_op(
    NonEmptyMaxSubarrayAggregate<T> left,
    NonEmptyMaxSubarrayAggregate<T> right
){
    if(left.empty) return right;
    if(right.empty) return left;
    return {
        false,
        left.sum + right.sum,
        std::max(left.prefix, left.sum + right.prefix),
        std::max(right.suffix, right.sum + left.suffix),
        std::max({left.best, right.best, left.suffix + right.prefix})
    };
}

template<class T>
constexpr NonEmptyMaxSubarrayAggregate<T> non_empty_max_subarray_identity(){
    return {};
}

template<class T>
constexpr HistoricalMaxAggregate<T> historical_max_op(
    HistoricalMaxAggregate<T> left,
    HistoricalMaxAggregate<T> right
){
    static_assert(
        std::numeric_limits<T>::is_specialized &&
        std::numeric_limits<T>::is_signed
    );
    return {
        std::max(left.current, right.current),
        std::max(left.historical, right.historical)
    };
}

template<class T>
constexpr HistoricalMaxAggregate<T> historical_max_identity(){
    static_assert(
        std::numeric_limits<T>::is_specialized &&
        std::numeric_limits<T>::is_signed
    );
    const T identity = std::numeric_limits<T>::lowest();
    return {identity, identity};
}

template<class T>
constexpr HistoricalMaxAggregate<T> historical_add_mapping(
    HistoricalAdd<T> operation,
    HistoricalMaxAggregate<T> aggregate
){
    static_assert(
        std::numeric_limits<T>::is_specialized &&
        std::numeric_limits<T>::is_signed
    );
    if(aggregate.current == std::numeric_limits<T>::lowest()){
        return historical_max_identity<T>();
    }
    return {
        aggregate.current + operation.total,
        std::max(
            aggregate.historical,
            aggregate.current + operation.peak
        )
    };
}

template<class T>
constexpr HistoricalAdd<T> historical_add_composition(
    HistoricalAdd<T> newer,
    HistoricalAdd<T> older
){
    static_assert(
        std::numeric_limits<T>::is_specialized &&
        std::numeric_limits<T>::is_signed
    );
    return {
        older.total + newer.total,
        std::max(older.peak, older.total + newer.peak)
    };
}

template<class T>
constexpr HistoricalAdd<T> historical_add_identity(){
    static_assert(
        std::numeric_limits<T>::is_specialized &&
        std::numeric_limits<T>::is_signed
    );
    return {};
}

constexpr bool bool_xor(bool f, bool g){ return f != g; }

constexpr bool false_value(){ return false; }

} // namespace common_monoid_internal

template<class T>
using NonEmptyMaxSubarrayMonoid = Monoid<
    common_monoid_internal::non_empty_max_subarray_op<T>,
    common_monoid_internal::non_empty_max_subarray_identity<T>
>;

template<class T>
using AffineCompositionMonoid = Monoid<
    common_monoid_internal::affine_sequence_op<T>,
    common_monoid_internal::affine_id<T>
>;

template<class T>
using AffineSumAggregateMonoidAct = Monoid_Act<
    common_monoid_internal::sum_length_op<T>,
    common_monoid_internal::sum_length_identity<T>,
    common_monoid_internal::affine_sum_length_mapping<T>,
    common_monoid_internal::affine_composition<T>,
    common_monoid_internal::affine_id<T>
>;

template<class T>
using AssignAffineCompositionMonoidAct = Monoid_Act<
    common_monoid_internal::affine_aggregate_op<T>,
    common_monoid_internal::affine_aggregate_identity<T>,
    common_monoid_internal::assign_affine_mapping<T>,
    common_monoid_internal::assignment_composition<MonoidAffine<T>>,
    common_monoid_internal::assignment_id<MonoidAffine<T>>
>;

template<class T>
using AddHistoricalMaxMonoidAct = Monoid_Act<
    common_monoid_internal::historical_max_op<T>,
    common_monoid_internal::historical_max_identity<T>,
    common_monoid_internal::historical_add_mapping<T>,
    common_monoid_internal::historical_add_composition<T>,
    common_monoid_internal::historical_add_identity<T>
>;

template<class T, T Identity = std::numeric_limits<T>::max()>
using AddMinMonoidAct = Monoid_Act<
    common_monoid_internal::minimum_op<T>,
    common_monoid_internal::constant<T, Identity>,
    common_monoid_internal::add_extremum_mapping<T, Identity>,
    common_monoid_internal::add_composition<T>,
    common_monoid_internal::zero<T>
>;

template<class T, T Identity = std::numeric_limits<T>::lowest()>
using AddMaxMonoidAct = Monoid_Act<
    common_monoid_internal::maximum_op<T>,
    common_monoid_internal::constant<T, Identity>,
    common_monoid_internal::add_extremum_mapping<T, Identity>,
    common_monoid_internal::add_composition<T>,
    common_monoid_internal::zero<T>
>;

template<class T, T Identity = std::numeric_limits<T>::max()>
using AssignMinMonoidAct = Monoid_Act<
    common_monoid_internal::minimum_op<T>,
    common_monoid_internal::constant<T, Identity>,
    common_monoid_internal::assign_extremum_mapping<T, Identity>,
    common_monoid_internal::assignment_composition<T>,
    common_monoid_internal::assignment_id<T>
>;

template<class T, T Identity = std::numeric_limits<T>::lowest()>
using AssignMaxMonoidAct = Monoid_Act<
    common_monoid_internal::maximum_op<T>,
    common_monoid_internal::constant<T, Identity>,
    common_monoid_internal::assign_extremum_mapping<T, Identity>,
    common_monoid_internal::assignment_composition<T>,
    common_monoid_internal::assignment_id<T>
>;

template<class T, T Identity = std::numeric_limits<T>::max()>
using ChminMinMonoidAct = Monoid_Act<
    common_monoid_internal::minimum_op<T>,
    common_monoid_internal::constant<T, Identity>,
    common_monoid_internal::chmin_mapping<T, Identity>,
    common_monoid_internal::minimum_op<T>,
    common_monoid_internal::constant<T, Identity>
>;

template<class T, T Identity = std::numeric_limits<T>::lowest()>
using ChmaxMaxMonoidAct = Monoid_Act<
    common_monoid_internal::maximum_op<T>,
    common_monoid_internal::constant<T, Identity>,
    common_monoid_internal::chmax_mapping<T, Identity>,
    common_monoid_internal::maximum_op<T>,
    common_monoid_internal::constant<T, Identity>
>;

template<class T>
using AddSumMonoidAct = Monoid_Act_Len<
    common_monoid_internal::sum_op<T>,
    common_monoid_internal::zero<T>,
    common_monoid_internal::add_sum_mapping<T>,
    common_monoid_internal::add_composition<T>,
    common_monoid_internal::zero<T>
>;

template<class T>
using MulSumMonoidAct = Monoid_Act_Len<
    common_monoid_internal::sum_op<T>,
    common_monoid_internal::zero<T>,
    common_monoid_internal::multiply_sum_mapping<T>,
    common_monoid_internal::multiply_op<T>,
    common_monoid_internal::one<T>
>;

template<class T>
using FlipCountMonoidAct = Monoid_Act_Len<
    common_monoid_internal::sum_op<T>,
    common_monoid_internal::zero<T>,
    common_monoid_internal::flip_count_mapping<T>,
    common_monoid_internal::bool_xor,
    common_monoid_internal::false_value
>;

template<class T>
using AssignSumMonoidAct = Monoid_Act_Len<
    common_monoid_internal::sum_op<T>,
    common_monoid_internal::zero<T>,
    common_monoid_internal::assign_sum_mapping<T>,
    common_monoid_internal::assignment_composition<T>,
    common_monoid_internal::assignment_id<T>
>;

template<class T>
using AffineSumMonoidAct = Monoid_Act_Len<
    common_monoid_internal::sum_op<T>,
    common_monoid_internal::zero<T>,
    common_monoid_internal::affine_sum_mapping<T>,
    common_monoid_internal::affine_composition<T>,
    common_monoid_internal::affine_id<T>
>;

template<class T>
using XorXorMonoidAct = Monoid_Act_Len<
    common_monoid_internal::xor_len_op<T>,
    common_monoid_internal::zero<T>,
    common_monoid_internal::xor_xor_mapping<T>,
    common_monoid_internal::xor_op<T>,
    common_monoid_internal::zero<T>
>;

template<class T>
using AssignMaxSubarrayMonoidAct = Monoid_Act_Len<
    common_monoid_internal::max_subarray_op<T>,
    common_monoid_internal::max_subarray_identity<T>,
    common_monoid_internal::assign_max_subarray_mapping<T>,
    common_monoid_internal::assignment_composition<T>,
    common_monoid_internal::assignment_id<T>
>;
