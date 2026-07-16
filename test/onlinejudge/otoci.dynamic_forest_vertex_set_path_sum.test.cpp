// competitive-verifier: PROBLEM https://www.spoj.com/problems/OTOCI/

#include <iostream>
#include <string>
#include <vector>

#include "../../src/structure/tree/dynamic_forest_vertex_set_path_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int vertex_count;
    std::cin >> vertex_count;
    std::vector<long long> values(static_cast<std::size_t>(vertex_count));
    for(long long& value: values) std::cin >> value;
    DynamicForestVertexSetPathSum<30000> forest(values);

    int query_count;
    std::cin >> query_count;
    while(query_count-- > 0){
        std::string command;
        int left, value;
        std::cin >> command >> left >> value;
        --left;
        if(command == "bridge"){
            std::cout << (forest.link(left, value - 1) ? "yes\n" : "no\n");
        }else if(command == "penguins"){
            forest.set(left, value);
        }else{
            const auto answer = forest.path_sum(left, value - 1);
            if(answer.has_value()) std::cout << *answer << '\n';
            else std::cout << "impossible\n";
        }
    }
}
