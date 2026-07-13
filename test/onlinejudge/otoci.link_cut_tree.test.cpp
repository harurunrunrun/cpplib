#define PROBLEM "https://www.spoj.com/problems/OTOCI/"

#include <iostream>
#include <string>
#include <vector>

#include "../../src/structure/tree/link_cut_tree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    std::cin >> vertex_count;
    std::vector<long long> penguin_count(static_cast<std::size_t>(vertex_count));
    for(long long& count: penguin_count){
        std::cin >> count;
    }

    LinkCutTree<AddMonoid<long long>{}, 30000> forest(penguin_count);
    int command_count;
    std::cin >> command_count;
    while(command_count-- != 0){
        std::string command;
        int left, right;
        std::cin >> command >> left >> right;
        --left;

        if(command == "bridge"){
            --right;
            std::cout << (forest.link(left, right) ? "yes" : "no") << '\n';
        }else if(command == "penguins"){
            forest.set(left, right);
        }else{
            --right;
            if(!forest.connected(left, right)){
                std::cout << "impossible\n";
            }else{
                std::cout << forest.path_prod(left, right) << '\n';
            }
        }
    }
}
