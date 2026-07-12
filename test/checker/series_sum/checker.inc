#include <cassert>
#include <stdexcept>

#include "../../../src/algorithm/math/series_sum.hpp"
#include "../../../src/structure/modint/modint.hpp"

using mint = Modint<998244353>;

void test_integer(){
    assert(math::arithmetic_progression_sum<long long>(3, 2, 5) == 35);
    assert(math::arithmetic_progression_sum<long long>(10, -3, 4) == 22);
    assert(math::arithmetic_progression_sum<long long>(7, 5, 0) == 0);
    assert(math::geometric_series_sum<long long>(3, 0) == 0);
    assert(math::geometric_series_sum<long long>(3, 5) == 121);
    assert(math::geometric_progression_sum<long long>(2, 3, 5) == 242);
    assert(math::geometric_progression_sum<long long>(5, 1, 4) == 20);
}

void test_modint(){
    assert(math::arithmetic_progression_sum(mint(3), mint(2), 5).val() == 35);
    assert(math::arithmetic_progression_sum(mint(10), mint(-3), 4).val() == 22);
    assert(math::geometric_series_sum(mint(3), 5).val() == 121);
    assert(math::geometric_progression_sum(mint(2), mint(3), 5).val() == 242);

    const auto [p, s] = math::geometric_series_pow_sum(mint(5), 7);
    assert(p.val() == mint(5).pow(7).val());
    mint expected = 0;
    mint cur = 1;
    for(int i = 0; i < 7; i++){
        expected += cur;
        cur *= 5;
    }
    assert(s == expected);
}

void test_exceptions(){
    bool thrown = false;
    try{
        (void)math::arithmetic_progression_sum<long long>(1, 1, -1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)math::geometric_series_sum<long long>(2, -1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

inline void run_series_sum_standalone_check(){
    test_integer();
    test_modint();
    test_exceptions();
}
