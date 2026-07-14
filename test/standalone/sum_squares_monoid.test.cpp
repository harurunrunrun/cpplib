// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>

#include "../../src/structure/segtree/lazysegtree.hpp"
#include "../../src/structure/types/sum_squares_monoid.hpp"

template<class T>
inline constexpr AffineSumSquaresMonoidAct<T> affine_sum_squares{};

static_assert(std::is_same_v<
    AffineSumSquaresMonoidAct<long long>,
    Monoid_Act<
        sum_squares_monoid_internal::sum_squares_op<long long>,
        sum_squares_monoid_internal::sum_squares_identity<long long>,
        sum_squares_monoid_internal::affine_sum_squares_mapping<long long>,
        common_monoid_internal::affine_composition<long long>,
        common_monoid_internal::affine_id<long long>
    >
>);
static_assert(
    sum_squares_singleton(-3LL)
    == SumSquaresAggregate<long long>{-3, 9, 1}
);
static_assert(sum_squares_assign(7LL) == MonoidAffine<long long>{0, 7});
static_assert(sum_squares_add(-4LL) == MonoidAffine<long long>{1, -4});
static_assert(
    affine_sum_squares<long long>.op(
        SumSquaresAggregate<long long>{2, 4, 1},
        SumSquaresAggregate<long long>{-3, 9, 1}
    ) == SumSquaresAggregate<long long>{-1, 13, 2}
);
static_assert(
    affine_sum_squares<long long>.mapping(
        sum_squares_add(-2LL),
        SumSquaresAggregate<long long>{5, 13, 2}
    ) == SumSquaresAggregate<long long>{1, 1, 2}
);
static_assert(
    affine_sum_squares<long long>.mapping(
        sum_squares_assign(-4LL),
        SumSquaresAggregate<long long>{5, 13, 2}
    ) == SumSquaresAggregate<long long>{-8, 32, 2}
);
static_assert(
    affine_sum_squares<long long>.composition(
        sum_squares_assign(7LL),
        sum_squares_add(-3LL)
    ) == MonoidAffine<long long>{0, 7}
);
static_assert(
    affine_sum_squares<long long>.composition(
        sum_squares_add(-3LL),
        sum_squares_assign(7LL)
    ) == MonoidAffine<long long>{0, 4}
);

namespace{

using Aggregate = SumSquaresAggregate<long long>;
using Tree = LazySegtree<affine_sum_squares<long long>, 100000>;

void self_test(){
    constexpr auto empty = affine_sum_squares<long long>.e();
    static_assert(empty == Aggregate{});
    static_assert(
        affine_sum_squares<long long>.mapping(sum_squares_add(9LL), empty)
        == empty
    );
    constexpr Aggregate value{5, 13, 2};
    constexpr auto older = sum_squares_assign(4LL);
    constexpr auto newer = sum_squares_add(-7LL);
    static_assert(
        affine_sum_squares<long long>.mapping(
            affine_sum_squares<long long>.composition(newer, older),
            value
        ) == affine_sum_squares<long long>.mapping(
            newer,
            affine_sum_squares<long long>.mapping(older, value)
        )
    );

    std::vector<Aggregate> initial{
        sum_squares_singleton(-2LL),
        sum_squares_singleton(3LL),
        sum_squares_singleton(0LL)
    };
    LazySegtree<affine_sum_squares<long long>, 4> tree(initial);
    assert((tree.prod(0, 3) == Aggregate{1, 13, 3}));
    tree.apply(0, 2, sum_squares_add(5LL));
    assert((tree.prod(0, 3) == Aggregate{11, 73, 3}));
    tree.apply(1, 3, sum_squares_assign(-4LL));
    assert((tree.prod(0, 3) == Aggregate{-5, 41, 3}));
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    int operation_count;
    if(!(std::cin >> size >> operation_count)){
        self_test();
        return 0;
    }

    std::vector<Aggregate> initial(static_cast<std::size_t>(size));
    for(auto& value: initial){
        long long element;
        std::cin >> element;
        value = sum_squares_singleton(element);
    }
    auto tree = std::make_unique<Tree>(initial);

    for(int operation_index = 0;
        operation_index < operation_count;
        ++operation_index){
        char operation;
        int left;
        int right;
        std::cin >> operation >> left >> right;
        if(operation == 'S'){
            long long value;
            std::cin >> value;
            tree->apply(left, right, sum_squares_assign(value));
        }else if(operation == 'A'){
            long long value;
            std::cin >> value;
            tree->apply(left, right, sum_squares_add(value));
        }else{
            const Aggregate result = tree->prod(left, right);
            std::cout << result.sum << ' ' << result.square_sum << '\n';
        }
    }
}
