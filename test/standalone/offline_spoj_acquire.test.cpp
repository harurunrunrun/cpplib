// competitive-verifier: STANDALONE

#define main onlinejudge_adapter_main
#include "../onlinejudge/acquire.minimum_rectangle_batch_cost.test.cpp"
#undef main

#include <cassert>
#include <limits>
#include <stdexcept>

int main(){
    assert(minimum_rectangle_batch_cost({
        {3, 3},
        {3, 1},
        {2, 5}
    }) == 15);

    bool invalid_dimension_rejected = false;
    try{
        (void)minimum_rectangle_batch_cost({{0, 1}});
    }catch(const std::runtime_error&){
        invalid_dimension_rejected = true;
    }
    assert(invalid_dimension_rejected);

    bool overflow_rejected = false;
    try{
        (void)minimum_rectangle_batch_cost({{
            std::numeric_limits<long long>::max(),
            std::numeric_limits<long long>::max()
        }});
    }catch(const std::overflow_error&){
        overflow_rejected = true;
    }
    assert(overflow_rejected);
    return onlinejudge_adapter_main();
}
