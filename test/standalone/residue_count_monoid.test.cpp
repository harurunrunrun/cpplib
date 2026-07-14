// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "../../src/structure/segtree/lazysegtree.hpp"
#include "../../src/structure/types/residue_count_monoid.hpp"

template<class Count, int MOD>
inline constexpr AddResidueCountMonoidAct<Count, MOD> residue_count_action{};

static_assert(std::is_same_v<
    AddResidueCountMonoidAct<int, 3>,
    Monoid_Act<
        residue_count_monoid_internal::residue_count_op<int, 3>,
        residue_count_monoid_internal::residue_count_identity<int, 3>,
        residue_count_monoid_internal::residue_shift_mapping<int, 3>,
        residue_count_monoid_internal::residue_shift_composition<3>,
        residue_count_monoid_internal::residue_shift_identity<3>
    >
>);
static_assert(ResidueShift<3>{-1}.shift == 2);
static_assert(ResidueShift<3>{4}.shift == 1);
static_assert(ResidueShift<1>{std::numeric_limits<long long>::lowest()}.shift == 0);
static_assert(ResidueShift<7>{std::numeric_limits<long long>::lowest()}.shift == 6);
static_assert(ResidueShift<7>{std::numeric_limits<long long>::max()}.shift == 0);
static_assert(
    residue_count_singleton<int, 3>(-4)
    == ResidueCountAggregate<int, 3>{{0, 0, 1}}
);
static_assert(
    residue_count_action<int, 3>.composition(
        ResidueShift<3>{-8},
        ResidueShift<3>{100}
    ) == ResidueShift<3>{92}
);
static_assert(
    residue_count_action<int, 3>.mapping(
        ResidueShift<3>{-1},
        ResidueCountAggregate<int, 3>{{2, 3, 5}}
    ) == ResidueCountAggregate<int, 3>{{3, 5, 2}}
);

template<class Count, int MOD>
void run_case(){
    int size;
    int operation_count;
    std::cin >> size >> operation_count;
    std::vector<ResidueCountAggregate<Count, MOD>> initial(
        static_cast<std::size_t>(size)
    );
    for(auto& aggregate: initial){
        long long value;
        std::cin >> value;
        aggregate = residue_count_singleton<Count, MOD>(value);
    }

    using Tree = LazySegtree<residue_count_action<Count, MOD>, 100000>;
    auto tree = std::make_unique<Tree>(initial);
    for(int operation = 0; operation < operation_count; ++operation){
        char type;
        int left;
        int right;
        std::cin >> type >> left >> right;
        if(type == 'A'){
            long long delta;
            std::cin >> delta;
            tree->apply(left, right, ResidueShift<MOD>{delta});
        }else{
            const auto result = tree->prod(left, right);
            for(int residue = 0; residue < MOD; ++residue){
                if(residue != 0) std::cout << ' ';
                std::cout << result.count[static_cast<std::size_t>(residue)];
            }
            std::cout << '\n';
        }
    }
}

void self_test(){
    constexpr auto action = residue_count_action<long long, 5>;
    constexpr auto empty = action.e();
    static_assert(action.mapping(ResidueShift<5>{-9}, empty) == empty);
    constexpr auto value = ResidueCountAggregate<long long, 5>{{1, 2, 3, 4, 5}};
    constexpr auto older = ResidueShift<5>{std::numeric_limits<long long>::max()};
    constexpr auto newer = ResidueShift<5>{std::numeric_limits<long long>::lowest()};
    static_assert(
        action.mapping(action.composition(newer, older), value)
        == action.mapping(newer, action.mapping(older, value))
    );
    static_assert(action.op(action.e(), value) == value);
    static_assert(action.op(value, action.e()) == value);

    std::vector<ResidueCountAggregate<int, 3>> initial = {
        residue_count_singleton<int, 3>(0),
        residue_count_singleton<int, 3>(1),
        residue_count_singleton<int, 3>(-1),
        residue_count_singleton<int, 3>(6)
    };
    LazySegtree<residue_count_action<int, 3>, 4> tree(initial);
    assert((tree.prod(0, 4) == ResidueCountAggregate<int, 3>{{2, 1, 1}}));
    tree.apply(1, 4, ResidueShift<3>{-4});
    assert((tree.prod(0, 4) == ResidueCountAggregate<int, 3>{{2, 1, 1}}));
    tree.apply(0, 2, ResidueShift<3>{std::numeric_limits<long long>::max()});
    assert((tree.prod(0, 4) == ResidueCountAggregate<int, 3>{{0, 3, 1}}));
}

int main(){
    std::string count_type;
    int modulus;
    if(!(std::cin >> count_type >> modulus)){
        self_test();
        return 0;
    }
    if(count_type == "I32"){
        if(modulus == 1) run_case<int, 1>();
        else if(modulus == 2) run_case<int, 2>();
        else if(modulus == 3) run_case<int, 3>();
        else if(modulus == 5) run_case<int, 5>();
        else run_case<int, 7>();
    }else{
        if(modulus == 1) run_case<long long, 1>();
        else if(modulus == 2) run_case<long long, 2>();
        else if(modulus == 3) run_case<long long, 3>();
        else if(modulus == 5) run_case<long long, 5>();
        else run_case<long long, 7>();
    }
}

static_assert([]{
    constexpr auto action = residue_count_action<int, 3>;
    ResidueShift<3> malformed;
    malformed.shift = -100;
    return action.mapping(
        malformed,
        ResidueCountAggregate<int, 3>{{2, 3, 5}}
    ) == ResidueCountAggregate<int, 3>{{3, 5, 2}};
}());
