// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "../../src/algorithm/tree/tree_path_order_statistics.hpp"

namespace{

using Statistics = TreePathOrderStatistics<long long, 256>;

struct LessOnly{
    int value;
};
bool operator<(const LessOnly& left, const LessOnly& right){
    return left.value < right.value;
}

template<class Function>
void expect_runtime_error(Function function){
    bool thrown = false;
    try{
        function();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void test_contract(){
    static_assert(!std::is_copy_constructible_v<Statistics>);
    static_assert(!std::is_copy_assignable_v<Statistics>);
    static_assert(std::is_move_constructible_v<Statistics>);
    static_assert(std::is_move_assignable_v<Statistics>);

    Statistics empty(0);
    empty.build({});
    assert(empty.size() == 0);
    assert(empty.root() == -1);
    expect_runtime_error([&]{ (void)empty.parent(0); });

    TreePathOrderStatistics<int, 1> minimum_capacity(1);
    minimum_capacity.build({7});
    assert(minimum_capacity.root() == 0);
    assert(minimum_capacity.parent(0) == -1);
    assert(minimum_capacity.depth(0) == 0);
    assert(minimum_capacity.lca(0, 0) == 0);
    assert(minimum_capacity.path_size(0, 0) == 1);
    assert(minimum_capacity.kth_smallest(0, 0, 0) == 7);

    Statistics single(1);
    expect_runtime_error([&]{ (void)single.root(); });
    expect_runtime_error([&]{ (void)single.value(0); });
    single.build({-4});
    assert(single.value(0) == -4);
    assert(single.kth_smallest(0, 0, 0) == -4);
    assert(single.kth_largest(0, 0, 0) == -4);
    assert(single.count_less(0, 0, -4) == 0);
    assert(single.count_less(0, 0, -3) == 1);
    assert(single.count_equal(0, 0, -4) == 1);
    assert(single.count_equal(0, 0, 4) == 0);
    assert(single.range_freq(0, 0, -4, -4) == 0);
    assert(single.range_freq(0, 0, -4, -3) == 1);

    TreePathOrderStatistics<LessOnly, 4> custom_value(2);
    custom_value.add_edge(0, 1);
    custom_value.build({LessOnly{2}, LessOnly{1}});
    assert(custom_value.kth_smallest(0, 1, 0).value == 1);
    assert(custom_value.count_equal(0, 1, LessOnly{2}) == 1);

    Statistics tree(6);
    tree.add_edge(0, 1);
    tree.add_edge(1, 2);
    tree.add_edge(1, 3);
    tree.add_edge(3, 4);
    tree.add_edge(3, 5);
    tree.build({5, -2, 5, 7, -2, 9}, 3);
    assert(tree.root() == 3);
    assert(tree.parent(3) == -1);
    assert(tree.parent(2) == 1);
    assert(tree.depth(2) == 2);
    assert(tree.lca(0, 2) == 1);
    assert(tree.lca(0, 4) == 3);
    assert(tree.path_size(2, 4) == 4);
    assert(tree.kth_smallest(2, 4, 0) == -2);
    assert(tree.kth_smallest(2, 4, 1) == -2);
    assert(tree.kth_smallest(2, 4, 2) == 5);
    assert(tree.kth_smallest(2, 4, 3) == 7);
    assert(tree.kth_largest(2, 4, 0) == 7);
    assert(tree.kth_largest(2, 4, 3) == -2);
    assert(tree.count_less(2, 4, -2) == 0);
    assert(tree.count_less(2, 4, 6) == 3);
    assert(tree.count_less(2, 4, 10) == 4);
    assert(tree.count_equal(2, 4, -2) == 2);
    assert(tree.count_equal(2, 4, 6) == 0);
    assert(tree.range_freq(2, 4, -2, 6) == 3);
    assert(tree.range_freq(2, 4, 5, 5) == 0);

    tree.build({1, 1, 2, 3, 5, 8}, 0);
    assert(tree.root() == 0);
    assert(tree.value(5) == 8);
    assert(tree.kth_smallest(2, 5, 2) == 3);

    expect_runtime_error([]{
        TreePathOrderStatistics<int, 4> too_large(5);
        (void)too_large;
    });
    expect_runtime_error([]{
        TreePathOrderStatistics<int, 4> negative(-1);
        (void)negative;
    });

    Statistics recover_after_failure(2);
    recover_after_failure.add_edge(0, 1);
    expect_runtime_error([&]{ recover_after_failure.build({1}); });
    recover_after_failure.build({1, 2}, 1);
    assert(recover_after_failure.kth_smallest(0, 1, 1) == 2);

    Statistics disconnected(2);
    expect_runtime_error([&]{ disconnected.build({1, 2}); });

    Statistics self_loop(2);
    self_loop.add_edge(0, 0);
    expect_runtime_error([&]{ self_loop.build({1, 2}); });

    Statistics cyclic(4);
    cyclic.add_edge(0, 1);
    cyclic.add_edge(1, 2);
    cyclic.add_edge(2, 0);
    expect_runtime_error([&]{ cyclic.build({1, 2, 3, 4}); });

    TreePathOrderStatistics<int, 2> edge_capacity(2);
    edge_capacity.add_edge(0, 1);
    expect_runtime_error([&]{ edge_capacity.add_edge(0, 1); });

    Statistics invalid_root(2);
    invalid_root.add_edge(0, 1);
    expect_runtime_error([&]{ invalid_root.build({1, 2}, 2); });

    expect_runtime_error([&]{ (void)tree.lca(-1, 0); });
    expect_runtime_error([&]{ (void)tree.path_size(0, tree.size()); });
    expect_runtime_error([&]{ (void)tree.kth_smallest(2, 4, -1); });
    expect_runtime_error([&]{ (void)tree.kth_smallest(2, 4, 4); });
    expect_runtime_error([&]{ (void)tree.kth_largest(2, 4, 4); });
    expect_runtime_error([&]{ (void)tree.count_less(0, tree.size(), 0); });
    expect_runtime_error([&]{ (void)tree.range_freq(0, 1, 3, 2); });
    expect_runtime_error([&]{ tree.add_edge(0, 5); });
}

} // namespace

int main(){
    test_contract();

    int test_count;
    if(!(std::cin >> test_count)){
        return 0;
    }
    while(test_count-- != 0){
        int n, query_count, root;
        std::cin >> n >> query_count >> root;
        std::vector<long long> values(static_cast<std::size_t>(n));
        for(auto& value: values){
            std::cin >> value;
        }
        Statistics statistics(n);
        for(int edge = 1; edge < n; ++edge){
            int left, right;
            std::cin >> left >> right;
            statistics.add_edge(left, right);
        }
        statistics.build(values, root);
        while(query_count-- != 0){
            char operation;
            int left, right;
            std::cin >> operation >> left >> right;
            if(operation == 'K'){
                int order;
                std::cin >> order;
                std::cout << statistics.kth_smallest(left, right, order) << '\n';
            }else if(operation == 'G'){
                int order;
                std::cin >> order;
                std::cout << statistics.kth_largest(left, right, order) << '\n';
            }else if(operation == 'C'){
                long long upper;
                std::cin >> upper;
                std::cout << statistics.count_less(left, right, upper) << '\n';
            }else if(operation == 'E'){
                long long target;
                std::cin >> target;
                std::cout << statistics.count_equal(left, right, target) << '\n';
            }else if(operation == 'R'){
                long long lower, upper;
                std::cin >> lower >> upper;
                std::cout << statistics.range_freq(left, right, lower, upper) << '\n';
            }else{
                assert(operation == 'S');
                std::cout << statistics.path_size(left, right) << '\n';
            }
        }
    }
}
