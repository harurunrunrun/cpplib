// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/matching/bipartite/stable_marriage.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        int proposer_count;
        int acceptor_count;
        std::cin >> proposer_count >> acceptor_count;
        std::vector<std::vector<int>> proposer_preferences(
            static_cast<std::size_t>(proposer_count)
        );
        std::vector<std::vector<int>> acceptor_preferences(
            static_cast<std::size_t>(acceptor_count)
        );
        for(auto& row: proposer_preferences){
            int size;
            std::cin >> size;
            row.resize(static_cast<std::size_t>(size));
            for(int& value: row) std::cin >> value;
        }
        for(auto& row: acceptor_preferences){
            int size;
            std::cin >> size;
            row.resize(static_cast<std::size_t>(size));
            for(int& value: row) std::cin >> value;
        }
        StableMarriageResult result = stable_marriage(
            proposer_preferences, acceptor_preferences
        );
        std::vector<std::vector<int>> rank(
            static_cast<std::size_t>(acceptor_count),
            std::vector<int>(static_cast<std::size_t>(proposer_count), -1)
        );
        for(int acceptor = 0; acceptor < acceptor_count; ++acceptor){
            for(int index = 0;
                index < static_cast<int>(
                    acceptor_preferences[static_cast<std::size_t>(acceptor)]
                        .size()
                );
                ++index){
                rank[static_cast<std::size_t>(acceptor)][static_cast<std::size_t>(
                    acceptor_preferences[static_cast<std::size_t>(acceptor)]
                        [static_cast<std::size_t>(index)]
                )] = index;
            }
        }
        for(int proposer = 0; proposer < proposer_count; ++proposer){
            for(int acceptor: proposer_preferences[
                static_cast<std::size_t>(proposer)
            ]){
                int proposer_rank = rank[static_cast<std::size_t>(acceptor)]
                    [static_cast<std::size_t>(proposer)];
                if(proposer_rank == -1) continue;
                int matched_acceptor =
                    result.proposer_match[static_cast<std::size_t>(proposer)];
                bool proposer_prefers = matched_acceptor == -1;
                for(int candidate: proposer_preferences[
                    static_cast<std::size_t>(proposer)
                ]){
                    if(candidate == acceptor) break;
                    if(candidate == matched_acceptor) proposer_prefers = false;
                }
                int previous =
                    result.acceptor_match[static_cast<std::size_t>(acceptor)];
                bool acceptor_prefers = previous == -1
                    || proposer_rank < rank[static_cast<std::size_t>(acceptor)]
                        [static_cast<std::size_t>(previous)];
                if(proposer_prefers && acceptor_prefers) return 2;
            }
        }
        for(int index = 0; index < proposer_count; ++index){
            if(index != 0) std::cout << ' ';
            std::cout << result.proposer_match[static_cast<std::size_t>(index)];
        }
        std::cout << '\n';
    }
}
