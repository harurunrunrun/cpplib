// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "../../src/structure/tree/lazy_link_cut_tree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AssignMaxSubarrayMonoidAct<long long> max_subarray_assignment{};

static_assert(std::is_same_v<
    typename decltype(max_subarray_assignment)::S,
    MaxSubarrayAggregate<long long>
>);
static_assert(max_subarray_singleton(-7LL)
    == MaxSubarrayAggregate<long long>{-7, 0, 0, 0});
static_assert(max_subarray_singleton(7LL)
    == MaxSubarrayAggregate<long long>{7, 7, 7, 7});
static_assert(max_subarray_assignment.e()
    == MaxSubarrayAggregate<long long>{});
static_assert(max_subarray_assignment.op(
    max_subarray_singleton(-3LL),
    1,
    max_subarray_singleton(5LL),
    1
) == MaxSubarrayAggregate<long long>{2, 2, 5, 5});
static_assert(max_subarray_assignment.mapping(
    MonoidAssignment<long long>{true, -4},
    max_subarray_assignment.e(),
    3
) == MaxSubarrayAggregate<long long>{-12, 0, 0, 0});
static_assert(max_subarray_assignment.mapping(
    MonoidAssignment<long long>{true, 9},
    max_subarray_assignment.e(),
    0
) == max_subarray_assignment.e());

template<class Function>
void expect_runtime_error(Function&& function){
    bool thrown = false;
    try{
        function();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void self_test(){
    std::vector<long long> raw = {-5, 4, -2, 7, -20, 6};
    std::vector<MaxSubarrayAggregate<long long>> values;
    for(const long long value: raw){
        values.push_back(max_subarray_singleton(value));
    }
    LazyLinkCutTree<max_subarray_assignment, 16> tree(values);
    assert(tree.link(0, 1));
    assert(tree.link(1, 2));
    assert(tree.link(2, 3));
    assert(tree.link(2, 4));
    assert(tree.link(4, 5));

    assert(tree.path_prod(0, 3).best == 9);
    assert(tree.path_prod(3, 0).best == 9);
    assert(tree.path_prod(3, 5).best == 7);
    assert(tree.path_prod(5, 3).best == 7);
    assert(tree.path_prod(0, 0).best == 0);

    tree.path_apply(3, 0, MonoidAssignment<long long>{true, -3});
    assert(tree.path_prod(0, 3).best == 0);
    tree.path_apply(0, 3, MonoidAssignment<long long>{true, 5});
    assert(tree.path_prod(3, 0).best == 20);
    tree.path_apply(2, 2, MonoidAssignment<long long>{true, -100});
    assert(tree.path_prod(0, 3).best == 10);

    expect_runtime_error([&]{
        tree.path_apply(-1, 0, MonoidAssignment<long long>{true, 0});
    });
    expect_runtime_error([&]{ (void)tree.path_prod(0, 16); });
}

int main(){
    self_test();

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- != 0){
        int vertex_count, operation_count;
        std::cin >> vertex_count >> operation_count;
        std::vector<MaxSubarrayAggregate<long long>> initial(
            static_cast<std::size_t>(vertex_count)
        );
        for(auto& value: initial){
            long long input;
            std::cin >> input;
            value = max_subarray_singleton(input);
        }
        LazyLinkCutTree<max_subarray_assignment, 100000> tree(initial);
        for(int edge = 1; edge < vertex_count; ++edge){
            int left, right;
            std::cin >> left >> right;
            assert(tree.link(left, right));
        }
        while(operation_count-- != 0){
            int type, left, right;
            std::cin >> type >> left >> right;
            if(type == 1){
                std::cout << tree.path_prod(left, right).best << '\n';
            }else{
                long long value;
                std::cin >> value;
                tree.path_apply(
                    left,
                    right,
                    MonoidAssignment<long long>{true, value}
                );
            }
        }
    }
}
