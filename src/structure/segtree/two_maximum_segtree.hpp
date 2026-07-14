#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "segtree.hpp"

template<class T>
struct TwoMaximumAggregate{
    std::array<T, 2> value{};
    int count = 0;

    friend constexpr bool operator==(
        const TwoMaximumAggregate&,
        const TwoMaximumAggregate&
    ) = default;
};

namespace two_maximum_segtree_internal{

template<class T>
constexpr void insert(TwoMaximumAggregate<T>& aggregate, const T& value){
    if(aggregate.count == 0){
        aggregate.value[0] = value;
        aggregate.count = 1;
        return;
    }
    if(aggregate.count == 1){
        if(aggregate.value[0] < value){
            aggregate.value[1] = aggregate.value[0];
            aggregate.value[0] = value;
        }else{
            aggregate.value[1] = value;
        }
        aggregate.count = 2;
        return;
    }
    if(aggregate.value[0] < value){
        aggregate.value[1] = aggregate.value[0];
        aggregate.value[0] = value;
    }else if(aggregate.value[1] < value){
        aggregate.value[1] = value;
    }
}

template<class T>
constexpr TwoMaximumAggregate<T> op(
    TwoMaximumAggregate<T> left,
    const TwoMaximumAggregate<T>& right
){
    for(int index = 0; index < right.count; ++index){
        insert(left, right.value[static_cast<std::size_t>(index)]);
    }
    return left;
}

template<class T>
constexpr TwoMaximumAggregate<T> identity(){
    return {};
}

template<class T>
constexpr TwoMaximumAggregate<T> singleton(const T& value){
    TwoMaximumAggregate<T> result;
    insert(result, value);
    return result;
}

template<class T>
struct Monoid{
    using S = TwoMaximumAggregate<T>;

    constexpr S op(const S& left, const S& right) const{
        return two_maximum_segtree_internal::op(left, right);
    }

    constexpr S e() const{
        return identity<T>();
    }
};

template<class T>
inline constexpr Monoid<T> monoid{};

} // namespace two_maximum_segtree_internal

template<class T, int MAX_SIZE>
class TwoMaximumSegtree{
    int size_;
    Segtree<two_maximum_segtree_internal::monoid<T>, MAX_SIZE> tree_;

    static std::vector<TwoMaximumAggregate<T>> default_initial(int size){
        if(size < 0 || MAX_SIZE < size)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        return std::vector<TwoMaximumAggregate<T>>(
            static_cast<std::size_t>(size),
            two_maximum_segtree_internal::singleton(T{})
        );
    }

    static std::vector<TwoMaximumAggregate<T>> make_initial(
        const std::vector<T>& values
    ){
        std::vector<TwoMaximumAggregate<T>> initial;
        initial.reserve(values.size());
        for(const T& value: values){
            initial.push_back(
                two_maximum_segtree_internal::singleton(value)
            );
        }
        return initial;
    }

public:
    explicit TwoMaximumSegtree(int n = MAX_SIZE):
        size_(n), tree_(default_initial(n)){}

    explicit TwoMaximumSegtree(const std::vector<T>& values):
        size_(static_cast<int>(values.size())), tree_(make_initial(values)){}

    int size() const{
        return size_;
    }

    void set(int position, const T& value){
        tree_.set(
            position,
            two_maximum_segtree_internal::singleton(value)
        );
    }

    T sum_of_two_largest(int left, int right) const{
        if(right - left < 2)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range must contain two values "
                "(sum_of_two_largest)."
            );
        }
        const auto aggregate = tree_.prod(left, right);
        return aggregate.value[0] + aggregate.value[1];
    }
};
