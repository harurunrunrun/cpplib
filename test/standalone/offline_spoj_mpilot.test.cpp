// competitive-verifier: STANDALONE

#define main onlinejudge_adapter_main
#include "../onlinejudge/mpilot.minimum_ordered_pair_assignment_cost.test.cpp"
#undef main

#include <cassert>
#include <limits>
#include <stdexcept>

int main(){
    assert(minimum_ordered_pair_assignment_cost({
        {-5, 100},
        {10, 1}
    }) == -4);

    bool odd_count_rejected = false;
    try{
        (void)minimum_ordered_pair_assignment_cost({{0, 0}});
    }catch(const std::runtime_error&){
        odd_count_rejected = true;
    }
    assert(odd_count_rejected);

    bool overflow_rejected = false;
    try{
        (void)minimum_ordered_pair_assignment_cost({
            {std::numeric_limits<long long>::max(), 0},
            {0, std::numeric_limits<long long>::max()}
        });
    }catch(const std::overflow_error&){
        overflow_rejected = true;
    }
    assert(overflow_rejected);
    return onlinejudge_adapter_main();
}
