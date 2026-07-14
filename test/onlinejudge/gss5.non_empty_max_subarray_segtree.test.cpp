#define PROBLEM "https://www.spoj.com/problems/GSS5/"

#include <algorithm>
#include <iostream>
#include <vector>

#include "../../src/structure/segtree/segtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- != 0){
        int size;
        std::cin >> size;
        std::vector<NonEmptyMaxSubarrayAggregate<long long>> values(
            static_cast<std::size_t>(size)
        );
        for(auto& value: values){
            long long input;
            std::cin >> input;
            value = non_empty_max_subarray_singleton(input);
        }
        Segtree<NonEmptyMaxSubarrayMonoid<long long>{}, 10000> tree(values);

        int query_count;
        std::cin >> query_count;
        while(query_count-- != 0){
            int x1, y1, x2, y2;
            std::cin >> x1 >> y1 >> x2 >> y2;
            --x1;
            --y1;
            --x2;
            --y2;

            long long answer;
            if(y1 < x2){
                answer = tree.prod(x1, y1 + 1).suffix
                    + tree.prod(y1 + 1, x2).sum
                    + tree.prod(x2, y2 + 1).prefix;
            }else{
                const auto overlap = tree.prod(x2, y1 + 1);
                answer = overlap.best;
                if(x1 < x2){
                    answer = std::max(
                        answer,
                        tree.prod(x1, x2).suffix
                            + tree.prod(x2, y2 + 1).prefix
                    );
                }
                if(y1 < y2){
                    answer = std::max(
                        answer,
                        overlap.suffix
                            + tree.prod(y1 + 1, y2 + 1).prefix
                    );
                }
            }
            std::cout << answer << '\n';
        }
    }
}
