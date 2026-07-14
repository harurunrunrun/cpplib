// competitive-verifier: PROBLEM https://www.spoj.com/problems/OTOCI/

#include <iostream>
#include <string>
#include <vector>
#include "../../src/structure/tree/penguin_forest.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(long long& value: values) std::cin >> value;
    PenguinForest<30000> forest(values);
    int q;
    std::cin >> q;
    while(q-- != 0){
        std::string command;
        int u, value;
        std::cin >> command >> u >> value;
        --u;
        if(command == "bridge") std::cout << (forest.bridge(u, value - 1) ? "yes\n" : "no\n");
        else if(command == "penguins") forest.set_penguins(u, value);
        else{
            const auto answer = forest.excursion_sum(u, value - 1);
            if(answer) std::cout << *answer << '\n';
            else std::cout << "impossible\n";
        }
    }
}
