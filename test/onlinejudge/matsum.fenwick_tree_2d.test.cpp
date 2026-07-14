// competitive-verifier: PROBLEM https://www.spoj.com/problems/MATSUM/

#include <iostream>
#include <memory>
#include <string>

#include "../../src/structure/other/fenwick_tree_2d.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    for(int case_index = 0; case_index < test_count; case_index++){
        int n;
        std::cin >> n;
        auto fenwick = std::make_unique<FenwickTree2D<long long, 1024, 1024>>(n, n);

        std::string command;
        while(std::cin >> command && command != "END"){
            if(command == "SET"){
                int x, y;
                long long value;
                std::cin >> x >> y >> value;
                fenwick->set(y, x, value);
            }else{
                int x1, y1, x2, y2;
                std::cin >> x1 >> y1 >> x2 >> y2;
                std::cout << fenwick->sum(y1, x1, y2 + 1, x2 + 1) << '\n';
            }
        }
        std::cout << '\n';
    }
}
