#define PROBLEM "https://www.spoj.com/problems/SEGSQRSS/"

#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/segtree/lazysegtree.hpp"
#include "../../src/structure/types/sum_squares_monoid.hpp"

namespace{

constexpr AffineSumSquaresMonoidAct<long long> sum_squares_action{};
using Aggregate = SumSquaresAggregate<long long>;
using Tree = LazySegtree<sum_squares_action, 100000>;

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    for(int test_index = 1; test_index <= test_count; ++test_index){
        int size;
        int operation_count;
        std::cin >> size >> operation_count;
        std::vector<Aggregate> initial(static_cast<std::size_t>(size));
        for(auto& aggregate: initial){
            long long value;
            std::cin >> value;
            aggregate = sum_squares_singleton(value);
        }
        auto tree = std::make_unique<Tree>(initial);

        std::cout << "Case " << test_index << ":\n";
        while(operation_count-- != 0){
            int operation;
            int left;
            int right;
            std::cin >> operation >> left >> right;
            --left;
            if(operation == 0){
                long long value;
                std::cin >> value;
                tree->apply(left, right, sum_squares_assign(value));
            }else if(operation == 1){
                long long value;
                std::cin >> value;
                tree->apply(left, right, sum_squares_add(value));
            }else{
                std::cout << tree->prod(left, right).square_sum << '\n';
            }
        }
    }
}
