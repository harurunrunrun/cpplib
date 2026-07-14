// competitive-verifier: PROBLEM https://www.spoj.com/problems/CORNET/

#include <iostream>
#include "../../src/structure/dsu/enterprise_network.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int tests;
    std::cin >> tests;
    while(tests-- != 0){
        int n;
        std::cin >> n;
        EnterpriseNetwork<20000> network(n);
        char command;
        while(std::cin >> command && command != 'O'){
            int enterprise;
            std::cin >> enterprise;
            --enterprise;
            if(command == 'E') std::cout << network.distance_to_center(enterprise) << '\n';
            else{
                int destination;
                std::cin >> destination;
                network.connect(enterprise, destination - 1);
            }
        }
    }
}
