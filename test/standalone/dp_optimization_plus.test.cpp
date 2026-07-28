// competitive-verifier: STANDALONE

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <limits>
#include <vector>

#include "../../src/algorithm/other/dynamic_programming/interval_partition/divide_and_conquer_dp_optimization.hpp"
#include "../../src/algorithm/other/dynamic_programming/interval_partition/knuth_optimization.hpp"
#include "../../src/algorithm/other/dynamic_programming/interval_partition/lagrangian_relaxation.hpp"
#include "../../src/algorithm/other/dynamic_programming/interval_partition/monge_dp.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    constexpr long long inf = std::numeric_limits<long long>::max() / 8;
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int kind;
        std::cin >> kind;
        if(kind == 0){
            int size;
            std::cin >> size;
            std::vector<long long> previous(
                static_cast<std::size_t>(size + 1)
            );
            std::vector<long long> prefix(
                static_cast<std::size_t>(size + 1), 0
            );
            for(long long& value: previous) std::cin >> value;
            for(int index = 0; index < size; ++index){
                long long value;
                std::cin >> value;
                prefix[static_cast<std::size_t>(index + 1)] =
                    prefix[static_cast<std::size_t>(index)] + value;
            }
            auto cost = [&](int left, int right){
                const long long sum =
                    prefix[static_cast<std::size_t>(right)]
                    - prefix[static_cast<std::size_t>(left)];
                return sum * sum;
            };
            const auto divide =
                divide_and_conquer_dp_layer(previous, cost, inf);
            const auto monge = monge_dp_layer(previous, cost, inf);
            if(divide.value != monge.value) return 2;
            for(int end = 1; end <= size; ++end){
                long long brute = inf;
                for(int split = 0; split < end; ++split){
                    brute = std::min(
                        brute,
                        previous[static_cast<std::size_t>(split)]
                            + cost(split, end)
                    );
                }
                if(brute != divide.value[static_cast<std::size_t>(end)]){
                    return 3;
                }
            }
            std::cout << divide.value[static_cast<std::size_t>(size)]
                      << '\n';
        }else if(kind == 1){
            int size;
            std::cin >> size;
            std::vector<long long> prefix(
                static_cast<std::size_t>(size + 1), 0
            );
            for(int index = 0; index < size; ++index){
                long long weight;
                std::cin >> weight;
                prefix[static_cast<std::size_t>(index + 1)] =
                    prefix[static_cast<std::size_t>(index)] + weight;
            }
            const auto result = knuth_optimization<long long>(
                size,
                [&](int left, int right){
                    return prefix[static_cast<std::size_t>(right)]
                        - prefix[static_cast<std::size_t>(left)];
                },
                inf
            );
            std::cout << result.value[0][static_cast<std::size_t>(size)]
                      << '\n';
        }else{
            int count;
            long long target;
            std::cin >> count >> target;
            std::vector<long long> values(
                static_cast<std::size_t>(count)
            );
            for(long long& value: values) std::cin >> value;
            const long long answer =
                lagrangian_relaxation_exact_count<long long>(
                    target, -1000, 1000,
                    [&](long long penalty){
                        LagrangianRelaxationState<long long> state{0, 0};
                        for(long long value: values){
                            if(value + penalty < 0){
                                state.penalized_cost += value + penalty;
                                ++state.count;
                            }
                        }
                        return state;
                    }
                );
            std::cout << answer << '\n';
        }
    }
}
