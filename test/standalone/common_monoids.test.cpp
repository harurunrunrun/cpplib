// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "../../src/structure/segtree/dynamic_lazysegtree.hpp"
#include "../../src/structure/segtree/lazysegtree.hpp"
#include "../../src/structure/segtree/segtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AddMonoid<long long> add_monoid{};
constexpr MulMonoid<long long> mul_monoid{};
constexpr MinMonoid<int> min_monoid{};
constexpr MaxMonoid<int> max_monoid{};
constexpr GcdMonoid<int> gcd_monoid{};
constexpr LcmMonoid<int> lcm_monoid{};
constexpr XorMonoid<int> xor_monoid{};
constexpr BitAndMonoid<int> bit_and_monoid{};
constexpr BitOrMonoid<int> bit_or_monoid{};

constexpr AddMinMonoidAct<long long> add_min_act{};
constexpr AddMaxMonoidAct<long long> add_max_act{};
constexpr AssignMinMonoidAct<long long> assign_min_act{};
constexpr AssignMaxMonoidAct<long long> assign_max_act{};
constexpr ChminMinMonoidAct<long long> chmin_min_act{};
constexpr ChmaxMaxMonoidAct<long long> chmax_max_act{};
constexpr AddSumMonoidAct<long long> add_sum_act{};
constexpr MulSumMonoidAct<long long> mul_sum_act{};
constexpr FlipCountMonoidAct<long long> flip_count_act{};
constexpr AssignSumMonoidAct<long long> assign_sum_act{};
constexpr AffineSumMonoidAct<long long> affine_sum_act{};
constexpr XorXorMonoidAct<int> xor_xor_act{};

static_assert(std::is_same_v<
    AddMonoid<int>,
    Monoid<
        common_monoid_internal::add_op<int>,
        common_monoid_internal::zero<int>
    >
>);
static_assert(std::is_same_v<
    AddMinMonoidAct<long long>,
    Monoid_Act<
        common_monoid_internal::minimum_op<long long>,
        common_monoid_internal::constant<
            long long,
            std::numeric_limits<long long>::max()
        >,
        common_monoid_internal::add_extremum_mapping<
            long long,
            std::numeric_limits<long long>::max()
        >,
        common_monoid_internal::add_composition<long long>,
        common_monoid_internal::zero<long long>
    >
>);
static_assert(std::is_same_v<
    AffineSumMonoidAct<long long>,
    Monoid_Act_Len<
        common_monoid_internal::sum_op<long long>,
        common_monoid_internal::zero<long long>,
        common_monoid_internal::affine_sum_mapping<long long>,
        common_monoid_internal::affine_composition<long long>,
        common_monoid_internal::affine_id<long long>
    >
>);
static_assert(!std::is_same_v<
    MonoidAssignment<long long>,
    MonoidAffine<long long>
>);

template<class M, std::size_t N>
constexpr bool satisfies_monoid_laws(
    M monoid,
    const std::array<typename M::S, N>& values
){
    for(auto x : values){
        if(monoid.op(monoid.e(), x) != x) return false;
        if(monoid.op(x, monoid.e()) != x) return false;
        for(auto y : values){
            for(auto z : values){
                if(
                    monoid.op(monoid.op(x, y), z)
                    != monoid.op(x, monoid.op(y, z))
                ) return false;
            }
        }
    }
    return true;
}

template<class M, std::size_t NS, std::size_t NT>
constexpr bool satisfies_action_laws(
    M action,
    const std::array<typename M::S, NS>& values,
    const std::array<typename M::T, NT>& operators
){
    for(auto f : operators){
        if(action.composition(action.id(), f) != f) return false;
        if(action.mapping(f, action.e()) != action.e()) return false;
        if(action.composition(f, action.id()) != f) return false;
        for(auto x : values){
            if(action.mapping(action.id(), x) != x) return false;
            for(auto y : values){
                if(
                    action.mapping(f, action.op(x, y))
                    != action.op(action.mapping(f, x), action.mapping(f, y))
                ) return false;
            }
            for(auto g : operators){
                if(
                    action.mapping(action.composition(f, g), x)
                    != action.mapping(f, action.mapping(g, x))
                ) return false;
                for(auto h : operators){
                    if(
                        action.composition(action.composition(f, g), h)
                        != action.composition(f, action.composition(g, h))
                    ) return false;
                }
            }
        }
    }
    return true;
}

template<class M, std::size_t NS, std::size_t NT>
constexpr bool satisfies_length_action_laws(
    M action,
    const std::array<std::pair<typename M::S, long long>, NS>& values,
    const std::array<typename M::T, NT>& operators
){
    for(auto [x, x_length] : values){
        if(action.op(action.e(), 0, x, x_length) != x) return false;
        if(action.op(x, x_length, action.e(), 0) != x) return false;
        for(auto f : operators){
            if(action.mapping(f, action.e(), 0) != action.e()) return false;
            if(action.composition(action.id(), f) != f) return false;
            if(action.composition(f, action.id()) != f) return false;
            if(action.mapping(action.id(), x, x_length) != x) return false;
            for(auto [y, y_length] : values){
                if(
                    action.mapping(
                        f,
                        action.op(x, x_length, y, y_length),
                        x_length + y_length
                    )
                    != action.op(
                        action.mapping(f, x, x_length),
                        x_length,
                        action.mapping(f, y, y_length),
                        y_length
                    )
                ) return false;
                for(auto [z, z_length] : values){
                    if(
                        action.op(
                            action.op(x, x_length, y, y_length),
                            x_length + y_length,
                            z,
                            z_length
                        )
                        != action.op(
                            x,
                            x_length,
                            action.op(y, y_length, z, z_length),
                            y_length + z_length
                        )
                    ) return false;
                }
            }
            for(auto g : operators){
                if(
                    action.mapping(
                        action.composition(f, g),
                        x,
                        x_length
                    )
                    != action.mapping(
                        f,
                        action.mapping(g, x, x_length),
                        x_length
                    )
                ) return false;
                for(auto h : operators){
                    if(
                        action.composition(action.composition(f, g), h)
                        != action.composition(f, action.composition(g, h))
                    ) return false;
                }
            }
        }
    }
    return true;
}

static_assert(satisfies_monoid_laws(
    add_monoid,
    std::array<long long, 4>{-3, 0, 2, 7}
));
static_assert(satisfies_monoid_laws(
    mul_monoid,
    std::array<long long, 4>{0, 1, 2, 4}
));
static_assert(satisfies_monoid_laws(
    min_monoid,
    std::array<int, 4>{min_monoid.e(), -4, 0, 8}
));
static_assert(satisfies_monoid_laws(
    max_monoid,
    std::array<int, 4>{max_monoid.e(), -4, 0, 8}
));
static_assert(satisfies_monoid_laws(
    gcd_monoid,
    std::array<int, 4>{0, 6, 10, 15}
));
static_assert(satisfies_monoid_laws(
    lcm_monoid,
    std::array<int, 4>{1, 2, 3, 6}
));
static_assert(satisfies_monoid_laws(
    xor_monoid,
    std::array<int, 4>{0, 3, 5, 12}
));
static_assert(satisfies_monoid_laws(
    bit_and_monoid,
    std::array<int, 4>{bit_and_monoid.e(), 3, 5, 12}
));
static_assert(satisfies_monoid_laws(
    bit_or_monoid,
    std::array<int, 4>{0, 3, 5, 12}
));
static_assert(MinMonoid<long long, (1LL << 60)>{}.e() == (1LL << 60));
static_assert(MaxMonoid<long long, -(1LL << 60)>{}.e() == -(1LL << 60));

static_assert(satisfies_action_laws(
    add_min_act,
    std::array<long long, 4>{add_min_act.e(), -5, 0, 8},
    std::array<long long, 3>{-3, 0, 4}
));
static_assert(satisfies_action_laws(
    add_max_act,
    std::array<long long, 4>{add_max_act.e(), -5, 0, 8},
    std::array<long long, 3>{-3, 0, 4}
));
static_assert(satisfies_action_laws(
    assign_min_act,
    std::array<long long, 4>{assign_min_act.e(), -5, 0, 8},
    std::array<MonoidAssignment<long long>, 4>{
        assign_min_act.id(),
        MonoidAssignment<long long>{true, -7},
        MonoidAssignment<long long>{true, 5},
        MonoidAssignment<long long>{false, 99}
    }
));
static_assert(satisfies_action_laws(
    assign_max_act,
    std::array<long long, 4>{assign_max_act.e(), -5, 0, 8},
    std::array<MonoidAssignment<long long>, 4>{
        assign_max_act.id(),
        MonoidAssignment<long long>{true, -7},
        MonoidAssignment<long long>{true, 5},
        MonoidAssignment<long long>{false, 99}
    }
));

static_assert(satisfies_action_laws(
    chmin_min_act,
    std::array<long long, 4>{chmin_min_act.e(), -5, 0, 8},
    std::array<long long, 4>{chmin_min_act.id(), -7, 1, 9}
));
static_assert(satisfies_action_laws(
    chmax_max_act,
    std::array<long long, 4>{chmax_max_act.e(), -5, 0, 8},
    std::array<long long, 4>{chmax_max_act.id(), -7, 1, 9}
));

constexpr std::array<std::pair<long long, long long>, 4> sum_values{{
    {0, 0}, {3, 1}, {-4, 2}, {10, 4}
}};
static_assert(satisfies_length_action_laws(
    add_sum_act,
    sum_values,
    std::array<long long, 3>{-3, 0, 4}
));
static_assert(satisfies_length_action_laws(
    mul_sum_act,
    sum_values,
    std::array<long long, 4>{-2, 0, 1, 3}
));
static_assert(satisfies_length_action_laws(
    flip_count_act,
    std::array<std::pair<long long, long long>, 4>{{
        {0, 0}, {0, 1}, {1, 1}, {2, 5}
    }},
    std::array<bool, 2>{false, true}
));
static_assert(satisfies_length_action_laws(
    assign_sum_act,
    sum_values,
    std::array<MonoidAssignment<long long>, 4>{
        assign_sum_act.id(),
        MonoidAssignment<long long>{true, -7},
        MonoidAssignment<long long>{true, 5},
        MonoidAssignment<long long>{false, 99}
    }
));
static_assert(satisfies_length_action_laws(
    affine_sum_act,
    sum_values,
    std::array<MonoidAffine<long long>, 4>{
        affine_sum_act.id(),
        MonoidAffine<long long>{0, 3},
        MonoidAffine<long long>{1, -2},
        MonoidAffine<long long>{2, 1}
    }
));
static_assert(satisfies_length_action_laws(
    xor_xor_act,
    std::array<std::pair<int, long long>, 4>{{
        {0, 0}, {3, 1}, {5, 2}, {12, 5}
    }},
    std::array<int, 4>{0, 3, 5, 12}
));

static_assert(
    assign_sum_act.composition(
        MonoidAssignment<long long>{true, 7},
        MonoidAssignment<long long>{true, 3}
    ) == MonoidAssignment<long long>{true, 7}
);
static_assert(
    affine_sum_act.composition(
        MonoidAffine<long long>{2, 5},
        MonoidAffine<long long>{3, 7}
    ) == MonoidAffine<long long>{6, 19}
);

void test_direct(){
    static_assert(add_monoid.op(2, 3) == 5);
    static_assert(add_monoid.e() == 0);
    static_assert(mul_monoid.op(2, 3) == 6);
    static_assert(mul_monoid.e() == 1);
    static_assert(min_monoid.op(5, -1) == -1);
    static_assert(max_monoid.op(5, -1) == 5);
    static_assert(gcd_monoid.op(18, 24) == 6);
    static_assert(gcd_monoid.e() == 0);
    static_assert(lcm_monoid.op(6, 10) == 30);
    static_assert(lcm_monoid.e() == 1);
    static_assert(xor_monoid.op(6, 3) == 5);
    static_assert(bit_and_monoid.op(6, 3) == 2);
    static_assert(bit_or_monoid.op(6, 3) == 7);
}

void test_segtree_add(){
    Segtree<add_monoid, 8> seg(std::vector<long long>{1, 2, 3, 4, 5});
    assert(seg.prod(0, 5) == 15);
    assert(seg.prod(1, 4) == 9);
    seg.set(2, 10);
    assert(seg.prod(0, 5) == 22);
}

void test_segtree_min_max(){
    Segtree<min_monoid, 8> mn(std::vector<int>{5, 2, 7, -1, 4});
    Segtree<max_monoid, 8> mx(std::vector<int>{5, 2, 7, -1, 4});
    assert(mn.prod(0, 5) == -1);
    assert(mn.prod(0, 3) == 2);
    assert(mx.prod(0, 5) == 7);
    assert(mx.prod(3, 5) == 4);
}

void test_segtree_other(){
    Segtree<gcd_monoid, 8> gcd_seg(std::vector<int>{12, 18, 30, 42});
    Segtree<lcm_monoid, 8> lcm_seg(std::vector<int>{2, 3, 4, 5});
    Segtree<xor_monoid, 8> xor_seg(std::vector<int>{1, 2, 3, 4});
    Segtree<bit_and_monoid, 8> and_seg(std::vector<int>{7, 6, 3});
    Segtree<bit_or_monoid, 8> or_seg(std::vector<int>{1, 2, 4});

    assert(gcd_seg.prod(0, 4) == 6);
    assert(gcd_seg.prod(1, 3) == 6);
    assert(lcm_seg.prod(0, 4) == 60);
    assert(xor_seg.prod(0, 4) == 4);
    assert(and_seg.prod(0, 3) == 2);
    assert(or_seg.prod(0, 3) == 7);
}

void test_lazy_actions(){
    LazySegtree<add_min_act, 8> add_min_seg(
        std::vector<long long>{5, 2, 7, -1, 4}
    );
    add_min_seg.apply(1, 4, 3);
    assert(add_min_seg.prod(0, 5) == 2);
    add_min_seg.apply(0, 5, -4);
    assert(add_min_seg.all_prod() == -2);

    LazySegtree<assign_max_act, 8> assign_max_seg(
        std::vector<long long>{1, 5, 2, 4, 3}
    );
    assign_max_seg.apply(1, 5, MonoidAssignment<long long>{true, 7});
    assign_max_seg.apply(2, 4, MonoidAssignment<long long>{true, -3});
    assert(assign_max_seg.prod(0, 5) == 7);
    assert(assign_max_seg.get(2) == -3);
}

void test_dynamic_length_actions(){
    DynamicLazySegtree<add_sum_act, 32> add_sum_seg(10);
    add_sum_seg.apply(2, 7, 3);
    add_sum_seg.apply(4, 10, -1);
    assert(add_sum_seg.all_prod() == 9);
    assert(add_sum_seg.prod(3, 8) == 8);

    DynamicLazySegtree<affine_sum_act, 32> affine_seg(5);
    for(int i = 0; i < 5; ++i) affine_seg.set(i, i + 1);
    affine_seg.apply(1, 4, MonoidAffine<long long>{2, 1});
    assert(affine_seg.all_prod() == 27);
    affine_seg.apply(0, 5, MonoidAffine<long long>{1, -2});
    assert(affine_seg.all_prod() == 17);

    DynamicLazySegtree<flip_count_act, 32> flip_seg(10);
    flip_seg.apply(2, 8, true);
    assert(flip_seg.all_prod() == 6);
    flip_seg.apply(5, 10, true);
    assert(flip_seg.all_prod() == 5);
    assert(flip_seg.prod(0, 5) == 3);
}

int main(){
    int q;
    if(std::cin >> q){
        while(q--){
            std::string type;
            long long a, b;
            std::cin >> type;
            if(type == "IDADD"){
                std::cout << add_sum_act.id() << '\n';
                continue;
            }
            if(type == "IDASSIGN"){
                const auto value = assign_sum_act.id();
                std::cout << value.assigned << ' ' << value.value << '\n';
                continue;
            }
            if(type == "IDAFFINE"){
                const auto value = affine_sum_act.id();
                std::cout << value.multiplier << ' ' << value.addend << '\n';
                continue;
            }
            if(type == "IDFLIP"){
                std::cout << flip_count_act.id() << '\n';
                continue;
            }
            if(type == "ADDMIN" || type == "ADDMAX"){
                long long x, f;
                std::cin >> x >> f;
                std::cout << (
                    type == "ADDMIN"
                    ? add_min_act.mapping(f, x)
                    : add_max_act.mapping(f, x)
                ) << '\n';
                continue;
            }
            if(type == "ASSIGNMIN" || type == "ASSIGNMAX"){
                long long x, value;
                int assigned;
                std::cin >> x >> assigned >> value;
                const MonoidAssignment<long long> f{
                    assigned != 0,
                    value
                };
                std::cout << (
                    type == "ASSIGNMIN"
                    ? assign_min_act.mapping(f, x)
                    : assign_max_act.mapping(f, x)
                ) << '\n';
                continue;
            }
            if(type == "CHMINMIN" || type == "CHMAXMAX"){
                long long x, bound;
                std::cin >> x >> bound;
                std::cout << (
                    type == "CHMINMIN"
                    ? chmin_min_act.mapping(bound, x)
                    : chmax_max_act.mapping(bound, x)
                ) << '\n';
                continue;
            }
            if(
                type == "ADDSUM"
                || type == "MULSUM"
                || type == "XORXOR"
            ){
                long long x, length, f;
                std::cin >> x >> length >> f;
                if(type == "ADDSUM"){
                    std::cout << add_sum_act.mapping(f, x, length) << '\n';
                }else if(type == "MULSUM"){
                    std::cout << mul_sum_act.mapping(f, x, length) << '\n';
                }else{
                    std::cout << xor_xor_act.mapping(
                        static_cast<int>(f),
                        static_cast<int>(x),
                        length
                    ) << '\n';
                }
                continue;
            }
            if(type == "FLIPCOUNT"){
                long long count, length;
                int flip;
                std::cin >> count >> length >> flip;
                std::cout << flip_count_act.mapping(
                    flip != 0,
                    count,
                    length
                ) << '\n';
                continue;
            }
            if(type == "ASSIGNSUM"){
                long long sum, length, value;
                int assigned;
                std::cin >> sum >> length >> assigned >> value;
                std::cout << assign_sum_act.mapping(
                    MonoidAssignment<long long>{assigned != 0, value},
                    sum,
                    length
                ) << '\n';
                continue;
            }
            if(type == "AFFINESUM"){
                long long sum, length, multiplier, addend;
                std::cin >> sum >> length >> multiplier >> addend;
                std::cout << affine_sum_act.mapping(
                    MonoidAffine<long long>{multiplier, addend},
                    sum,
                    length
                ) << '\n';
                continue;
            }
            if(type == "COMPOSE_ASSIGN"){
                int f_assigned, g_assigned;
                long long f_value, g_value;
                std::cin
                    >> f_assigned >> f_value
                    >> g_assigned >> g_value;
                const auto result = assign_sum_act.composition(
                    {f_assigned != 0, f_value},
                    {g_assigned != 0, g_value}
                );
                std::cout
                    << result.assigned << ' '
                    << (result.assigned ? result.value : 0) << '\n';
                continue;
            }
            if(type == "COMPOSE_AFFINE"){
                long long fm, fa, gm, ga;
                std::cin >> fm >> fa >> gm >> ga;
                const auto result = affine_sum_act.composition(
                    {fm, fa},
                    {gm, ga}
                );
                std::cout
                    << result.multiplier << ' '
                    << result.addend << '\n';
                continue;
            }
            if(type == "COMPOSE_FLIP"){
                int f, g;
                std::cin >> f >> g;
                std::cout << flip_count_act.composition(
                    f != 0,
                    g != 0
                ) << '\n';
                continue;
            }
            if(type.size() > 1 && type[0] == 'E'){
                if(type == "EADD") std::cout << add_monoid.e() << '\n';
                if(type == "EMUL") std::cout << mul_monoid.e() << '\n';
                if(type == "EMIN") std::cout << min_monoid.e() << '\n';
                if(type == "EMAX") std::cout << max_monoid.e() << '\n';
                if(type == "EGCD") std::cout << gcd_monoid.e() << '\n';
                if(type == "ELCM") std::cout << lcm_monoid.e() << '\n';
                if(type == "EXOR") std::cout << xor_monoid.e() << '\n';
                if(type == "EAND") std::cout << bit_and_monoid.e() << '\n';
                if(type == "EOR") std::cout << bit_or_monoid.e() << '\n';
                continue;
            }
            std::cin >> a >> b;
            if(type == "ADD") std::cout << add_monoid.op(a, b) << '\n';
            if(type == "MUL") std::cout << mul_monoid.op(a, b) << '\n';
            if(type == "MIN") std::cout << min_monoid.op(static_cast<int>(a), static_cast<int>(b)) << '\n';
            if(type == "MAX") std::cout << max_monoid.op(static_cast<int>(a), static_cast<int>(b)) << '\n';
            if(type == "GCD") std::cout << gcd_monoid.op(static_cast<int>(a), static_cast<int>(b)) << '\n';
            if(type == "LCM") std::cout << lcm_monoid.op(static_cast<int>(a), static_cast<int>(b)) << '\n';
            if(type == "XOR") std::cout << xor_monoid.op(static_cast<int>(a), static_cast<int>(b)) << '\n';
            if(type == "AND") std::cout << bit_and_monoid.op(static_cast<int>(a), static_cast<int>(b)) << '\n';
            if(type == "OR") std::cout << bit_or_monoid.op(static_cast<int>(a), static_cast<int>(b)) << '\n';
        }
        return 0;
    }

    test_direct();
    test_segtree_add();
    test_segtree_min_max();
    test_segtree_other();
    test_lazy_actions();
    test_dynamic_length_actions();
}
