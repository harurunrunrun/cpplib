// competitive-verifier: STANDALONE

#define main onlinejudge_adapter_main
#include "../onlinejudge/apio10a.maximum_quadratic_partition_score.test.cpp"
#undef main

#include <cassert>
#include <limits>
#include <stdexcept>

int main(){
    assert(maximum_quadratic_partition_score({1, 2}, -1, 0, 0) == -5);

    bool invalid_coefficient_rejected = false;
    try{
        (void)maximum_quadratic_partition_score({1}, 0, 0, 0);
    }catch(const std::runtime_error&){
        invalid_coefficient_rejected = true;
    }
    assert(invalid_coefficient_rejected);

    bool overflow_rejected = false;
    try{
        (void)maximum_quadratic_partition_score(
            {std::numeric_limits<long long>::max()},
            -1,
            0,
            0
        );
    }catch(const std::overflow_error&){
        overflow_rejected = true;
    }
    assert(overflow_rejected);
    return onlinejudge_adapter_main();
}
