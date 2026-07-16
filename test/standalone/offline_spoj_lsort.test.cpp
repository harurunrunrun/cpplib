// competitive-verifier: STANDALONE

#define main onlinejudge_adapter_main
#include "../onlinejudge/lsort.minimum_rank_removal_sort_cost.test.cpp"
#undef main

#include <cassert>
#include <stdexcept>

int main(){
    assert(minimum_rank_removal_sort_cost({2, 1}) == 3);

    bool invalid_permutation_rejected = false;
    try{
        (void)minimum_rank_removal_sort_cost({1, 1});
    }catch(const std::runtime_error&){
        invalid_permutation_rejected = true;
    }
    assert(invalid_permutation_rejected);
    return onlinejudge_adapter_main();
}
