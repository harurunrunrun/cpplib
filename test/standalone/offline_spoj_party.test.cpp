// competitive-verifier: STANDALONE

#define main onlinejudge_adapter_main
#include "../onlinejudge/party.knapsack_max_value_min_cost.test.cpp"
#undef main

#include <cassert>
#include <stdexcept>

namespace {

void generic_boundary_test(){
    {
        const auto optimum = knapsack_max_value_min_cost(0, {});
        assert(optimum.minimum_cost == 0);
        assert(optimum.maximum_value == 0);
    }
    {
        const auto optimum = knapsack_max_value_min_cost(
            0,
            {{0, 3}, {1, 100}, {0, 7}}
        );
        assert(optimum.minimum_cost == 0);
        assert(optimum.maximum_value == 10);
    }
    {
        const auto optimum = knapsack_max_value_min_cost(
            10,
            {{4, 10}, {6, 10}, {8, 20}}
        );
        assert(optimum.minimum_cost == 8);
        assert(optimum.maximum_value == 20);
    }
    {
        const auto optimum = knapsack_max_value_min_cost(
            3,
            {{4, 100}, {3, 0}}
        );
        assert(optimum.minimum_cost == 0);
        assert(optimum.maximum_value == 0);
    }

    bool negative_capacity_rejected = false;
    try{
        (void)knapsack_max_value_min_cost(-1, {});
    }catch(const std::runtime_error&){
        negative_capacity_rejected = true;
    }
    assert(negative_capacity_rejected);

    bool negative_item_rejected = false;
    try{
        (void)knapsack_max_value_min_cost(1, {{-1, 0}});
    }catch(const std::runtime_error&){
        negative_item_rejected = true;
    }
    assert(negative_item_rejected);
}

} // namespace

int main(){
    generic_boundary_test();
    return onlinejudge_adapter_main();
}
