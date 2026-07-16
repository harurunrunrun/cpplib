// competitive-verifier: STANDALONE

#define main onlinejudge_adapter_main
#include "../onlinejudge/mkbudget.minimum_cost_capacity_level_schedule.test.cpp"
#undef main

#include <cassert>
#include <limits>
#include <stdexcept>

int main(){
    assert(minimum_cost_capacity_level_schedule({2, 0, 2}, 10, 1, 10) == 26);

    bool invalid_level_rejected = false;
    try{
        (void)minimum_cost_capacity_level_schedule({-1}, 1, 1, 1);
    }catch(const std::runtime_error&){
        invalid_level_rejected = true;
    }
    assert(invalid_level_rejected);

    bool overflow_rejected = false;
    try{
        (void)minimum_cost_capacity_level_schedule(
            {1, 1},
            std::numeric_limits<long long>::max(),
            std::numeric_limits<long long>::max(),
            0
        );
    }catch(const std::overflow_error&){
        overflow_rejected = true;
    }
    assert(overflow_rejected);
    return onlinejudge_adapter_main();
}
