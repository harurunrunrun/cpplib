#define PROBLEM "https://www.spoj.com/problems/GSS7/"

#include <iostream>
#include <vector>

#include "../../src/structure/tree/lazy_link_cut_tree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AssignMaxSubarrayMonoidAct<long long> max_subarray_assignment{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    std::cin >> vertex_count;
    std::vector<MaxSubarrayAggregate<long long>> values(
        static_cast<std::size_t>(vertex_count)
    );
    for(auto& value: values){
        long long input;
        std::cin >> input;
        value = max_subarray_singleton(input);
    }

    LazyLinkCutTree<max_subarray_assignment, 100000> tree(values);
    for(int edge = 1; edge < vertex_count; ++edge){
        int left, right;
        std::cin >> left >> right;
        tree.link(left - 1, right - 1);
    }

    int query_count;
    std::cin >> query_count;
    while(query_count-- != 0){
        int type, left, right;
        std::cin >> type >> left >> right;
        --left;
        --right;
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
