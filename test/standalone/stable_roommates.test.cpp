// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/matching/general/stable_roommates.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int count;
        std::cin >> count;
        std::vector<std::vector<int>> preferences(
            static_cast<std::size_t>(count),
            std::vector<int>(static_cast<std::size_t>(
                count == 0 ? 0 : count - 1
            ))
        );
        for(auto& row: preferences){
            for(int& value: row) std::cin >> value;
        }
        auto result = stable_roommates(preferences);
        if(result){
            std::vector<std::vector<int>> rank(
                static_cast<std::size_t>(count),
                std::vector<int>(static_cast<std::size_t>(count), -1)
            );
            for(int person = 0; person < count; ++person){
                for(int position = 0; position < count - 1; ++position){
                    rank[static_cast<std::size_t>(person)]
                        [static_cast<std::size_t>(
                            preferences[static_cast<std::size_t>(person)]
                                [static_cast<std::size_t>(position)]
                        )] = position;
                }
            }
            for(int first = 0; first < count; ++first){
                int partner =
                    (*result)[static_cast<std::size_t>(first)];
                if(partner < 0 || count <= partner
                    || (*result)[static_cast<std::size_t>(partner)]
                        != first) return 2;
                for(int second = first + 1; second < count; ++second){
                    if(second == partner) continue;
                    if(rank[static_cast<std::size_t>(first)]
                            [static_cast<std::size_t>(second)]
                        < rank[static_cast<std::size_t>(first)]
                            [static_cast<std::size_t>(partner)]
                        && rank[static_cast<std::size_t>(second)]
                            [static_cast<std::size_t>(first)]
                        < rank[static_cast<std::size_t>(second)]
                            [static_cast<std::size_t>(
                                (*result)[static_cast<std::size_t>(second)]
                            )]){
                        return 3;
                    }
                }
            }
        }
        std::cout << static_cast<int>(result.has_value()) << '\n';
    }
}
