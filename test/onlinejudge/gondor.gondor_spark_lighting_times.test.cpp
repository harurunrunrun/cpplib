// competitive-verifier: PROBLEM https://www.spoj.com/problems/GONDOR/

#include <iomanip>
#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/gondor_spark_lighting_times.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int spark_count;
    if(!(std::cin >> spark_count)) return 0;
    std::vector<GondorSpark> sparks(static_cast<std::size_t>(spark_count));
    for(int spark = 0; spark < spark_count; spark++){
        auto& data = sparks[static_cast<std::size_t>(spark)];
        std::cin >> data.x >> data.y >> data.arrow_count;
        data.instruction_order.resize(static_cast<std::size_t>(spark_count - 1));
        for(int& target: data.instruction_order){
            std::cin >> target;
            target--;
        }
    }
    const auto answer = gondor_spark_lighting_times(sparks);
    std::cout << std::fixed << std::setprecision(6);
    for(double time: answer) std::cout << time << '\n';
}
