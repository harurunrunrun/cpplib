// competitive-verifier: STANDALONE

#define main onlinejudge_adapter_main
#include "../onlinejudge/bat3.longest_decreasing_subsequence_with_pivot_reset.test.cpp"
#undef main

#include <cassert>
#include <stdexcept>

int main(){
    assert(longest_decreasing_subsequence_with_pivot_reset(
        std::vector<int>{5, 4, 100, 3}, 1
    ) == 4);

    bool invalid_pivot_rejected = false;
    try{
        (void)longest_decreasing_subsequence_with_pivot_reset({1}, -1);
    }catch(const std::runtime_error&){
        invalid_pivot_rejected = true;
    }
    assert(invalid_pivot_rejected);
    return onlinejudge_adapter_main();
}
