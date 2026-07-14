// competitive-verifier: PROBLEM https://br.spoj.com/SPOJ/problems/CNTPRIME/

#include <iostream>
#include <vector>

#include "../../src/structure/segtree/dynamic_prime_count_range_assign.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int test_count;
    std::cin >> test_count;
    for(int test_case = 1; test_case <= test_count; ++test_case){
        int size, query_count;
        std::cin >> size >> query_count;
        std::vector<int> initial(static_cast<std::size_t>(size));
        for(int& value: initial) std::cin >> value;
        DynamicPrimeCountRangeAssign<10000, 1000000> values(initial);

        std::cout << "Case " << test_case << ":\n";
        while(query_count-- != 0){
            int type, left, right;
            std::cin >> type >> left >> right;
            --left;
            if(type == 0){
                int value;
                std::cin >> value;
                values.assign(left, right, value);
            }else{
                std::cout << values.count_primes(left, right) << '\n';
            }
        }
    }
}
