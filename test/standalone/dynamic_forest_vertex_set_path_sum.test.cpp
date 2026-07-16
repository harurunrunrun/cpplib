// competitive-verifier: STANDALONE

#include <iostream>
#include <vector>

#include "../../src/structure/tree/dynamic_forest_vertex_set_path_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        int vertex_count, operation_count;
        std::cin >> vertex_count >> operation_count;
        std::vector<long long> values(static_cast<std::size_t>(vertex_count));
        for(long long& value: values) std::cin >> value;
        DynamicForestVertexSetPathSum<64> forest(values);

        while(operation_count-- > 0){
            char type;
            int left;
            std::cin >> type >> left;
            if(type == 'U'){
                long long value;
                std::cin >> value;
                forest.set(left, value);
            }else if(type == 'G'){
                std::cout << forest.get(left) << '\n';
            }else{
                int right;
                std::cin >> right;
                if(type == 'L'){
                    std::cout << static_cast<int>(
                        forest.link(left, right)
                    ) << '\n';
                }else if(type == 'C'){
                    std::cout << static_cast<int>(
                        forest.cut(left, right)
                    ) << '\n';
                }else if(type == 'Q'){
                    std::cout << static_cast<int>(
                        forest.connected(left, right)
                    ) << '\n';
                }else{
                    const auto answer = forest.path_sum(left, right);
                    if(answer.has_value()) std::cout << *answer << '\n';
                    else std::cout << "X\n";
                }
            }
        }
    }
}
