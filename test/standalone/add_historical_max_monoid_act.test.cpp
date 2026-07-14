// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "../../src/structure/segtree/lazysegtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

template<class T>
inline constexpr AddHistoricalMaxMonoidAct<T> add_historical_max{};

static_assert(std::is_same_v<
    AddHistoricalMaxMonoidAct<int>,
    Monoid_Act<
        common_monoid_internal::historical_max_op<int>,
        common_monoid_internal::historical_max_identity<int>,
        common_monoid_internal::historical_add_mapping<int>,
        common_monoid_internal::historical_add_composition<int>,
        common_monoid_internal::historical_add_identity<int>
    >
>);
static_assert(historical_add(-7) == HistoricalAdd<int>{-7, 0});
static_assert(historical_add(7LL) == HistoricalAdd<long long>{7, 7});
static_assert(
    add_historical_max<int>.composition(
        historical_add(-3),
        historical_add(5)
    ) == HistoricalAdd<int>{2, 5}
);
static_assert(
    add_historical_max<int>.mapping(
        historical_add(-3),
        HistoricalMaxAggregate<int>{10, 12}
    ) == HistoricalMaxAggregate<int>{7, 12}
);
static_assert(
    add_historical_max<int>.mapping(
        historical_add(5),
        HistoricalMaxAggregate<int>{10, 12}
    ) == HistoricalMaxAggregate<int>{15, 15}
);
static_assert(
    add_historical_max<int>.mapping(
        historical_add(40),
        HistoricalMaxAggregate<int>{
            std::numeric_limits<int>::max() - 100,
            std::numeric_limits<int>::max() - 80
        }
    ).current == std::numeric_limits<int>::max() - 60
);
static_assert(
    add_historical_max<long long>.mapping(
        historical_add(-40LL),
        HistoricalMaxAggregate<long long>{
            std::numeric_limits<long long>::lowest() + 100,
            std::numeric_limits<long long>::lowest() + 80
        }
    ).current == std::numeric_limits<long long>::lowest() + 60
);

template<class T>
void run(){
    int size;
    int operation_count;
    std::cin >> size >> operation_count;
    std::vector<HistoricalMaxAggregate<T>> initial(static_cast<std::size_t>(size));
    for(auto& value: initial){
        std::cin >> value.current;
        value.historical = value.current;
    }
    using Tree = LazySegtree<add_historical_max<T>, 100000>;
    auto tree = std::make_unique<Tree>(initial);
    for(int operation = 0; operation < operation_count; ++operation){
        char type;
        int left;
        int right;
        std::cin >> type >> left >> right;
        if(type == 'A'){
            T delta;
            std::cin >> delta;
            tree->apply(left, right, historical_add(delta));
        }else{
            const auto result = tree->prod(left, right);
            std::cout << result.current << ' ' << result.historical << '\n';
        }
    }
}

void self_test(){
    constexpr auto action = add_historical_max<long long>;
    constexpr auto empty = action.e();
    static_assert(action.mapping(historical_add(9LL), empty) == empty);
    constexpr auto value = HistoricalMaxAggregate<long long>{4, 7};
    constexpr auto older = historical_add(5LL);
    constexpr auto newer = historical_add(-8LL);
    static_assert(
        action.mapping(action.composition(newer, older), value)
        == action.mapping(newer, action.mapping(older, value))
    );

    std::vector<HistoricalMaxAggregate<long long>> initial = {
        historical_max_singleton(0LL),
        historical_max_singleton(-3LL),
        historical_max_singleton(5LL)
    };
    LazySegtree<add_historical_max<long long>, 4> tree(initial);
    tree.apply(0, 2, historical_add(4LL));
    tree.apply(0, 3, historical_add(-10LL));
    const auto result = tree.prod(0, 3);
    assert(result.current == -5);
    assert(result.historical == 5);
}

int main(){
    std::string type;
    if(!(std::cin >> type)){
        self_test();
        return 0;
    }
    if(type == "I32"){
        run<int>();
    }else{
        run<long long>();
    }
}
