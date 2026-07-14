// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>

#include "../../src/structure/types/common_monoids.hpp"

constexpr NonEmptyMaxSubarrayMonoid<long long> non_empty_max_subarray{};

static_assert(std::is_same_v<
    NonEmptyMaxSubarrayMonoid<int>,
    Monoid<
        common_monoid_internal::non_empty_max_subarray_op<int>,
        common_monoid_internal::non_empty_max_subarray_identity<int>
    >
>);
static_assert(non_empty_max_subarray.e().empty);
static_assert(
    non_empty_max_subarray_singleton(-7LL)
    == NonEmptyMaxSubarrayAggregate<long long>{false, -7, -7, -7, -7}
);
static_assert(
    non_empty_max_subarray.op(
        non_empty_max_subarray_singleton(-5LL),
        non_empty_max_subarray_singleton(-2LL)
    ).best == -2
);
static_assert(
    non_empty_max_subarray.op(
        non_empty_max_subarray.e(),
        non_empty_max_subarray_singleton(0LL)
    ) == non_empty_max_subarray_singleton(0LL)
);

template<class T>
NonEmptyMaxSubarrayAggregate<T> fold_left(const std::vector<T>& values){
    constexpr NonEmptyMaxSubarrayMonoid<T> monoid{};
    auto result = monoid.e();
    for(T value: values){
        result = monoid.op(result, non_empty_max_subarray_singleton(value));
    }
    return result;
}

template<class T>
NonEmptyMaxSubarrayAggregate<T> fold_right(const std::vector<T>& values){
    constexpr NonEmptyMaxSubarrayMonoid<T> monoid{};
    auto result = monoid.e();
    for(auto iterator = values.rbegin(); iterator != values.rend(); ++iterator){
        result = monoid.op(non_empty_max_subarray_singleton(*iterator), result);
    }
    return result;
}

template<class T>
void run_case(int size){
    std::vector<T> values(static_cast<std::size_t>(size));
    for(T& value: values) std::cin >> value;
    const auto left = fold_left(values);
    const auto right = fold_right(values);
    assert(left == right);
    std::cout << (left.empty ? 1 : 0) << ' '
              << left.sum << ' '
              << left.prefix << ' '
              << left.suffix << ' '
              << left.best << '\n';
}

void self_test(){
    {
        const std::vector<int> values = {-8, -3, -5, -2, -9};
        const auto result = fold_left(values);
        assert(!result.empty);
        assert(result.sum == -27);
        assert(result.prefix == -8);
        assert(result.suffix == -9);
        assert(result.best == -2);
    }
    {
        const std::vector<int> values = {0, -4, 0};
        const auto result = fold_left(values);
        assert(result.best == 0);
        assert(result.prefix == 0);
        assert(result.suffix == 0);
    }
    {
        const int low = std::numeric_limits<int>::lowest();
        assert(non_empty_max_subarray_singleton(low).best == low);
        const long long high = std::numeric_limits<long long>::max();
        assert(non_empty_max_subarray_singleton(high).sum == high);
    }
}

int main(){
    int test_count;
    if(!(std::cin >> test_count)){
        self_test();
        return 0;
    }
    for(int test = 0; test < test_count; ++test){
        std::string type;
        int size;
        std::cin >> type >> size;
        if(type == "I32"){
            run_case<int>(size);
        }else{
            run_case<long long>(size);
        }
    }
}
