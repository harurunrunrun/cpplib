#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_STABLE_MARRIAGE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_STABLE_MARRIAGE_HPP_INCLUDED

#include <queue>
#include <stdexcept>
#include <vector>

struct StableMarriageResult{
    std::vector<int> proposer_match;
    std::vector<int> acceptor_match;
};

inline StableMarriageResult stable_marriage(
    const std::vector<std::vector<int>>& proposer_preferences,
    const std::vector<std::vector<int>>& acceptor_preferences
){
    const int proposer_count =
        static_cast<int>(proposer_preferences.size());
    const int acceptor_count =
        static_cast<int>(acceptor_preferences.size());
    std::vector<std::vector<int>> rank(
        static_cast<std::size_t>(acceptor_count),
        std::vector<int>(static_cast<std::size_t>(proposer_count), -1)
    );
    for(int acceptor = 0; acceptor < acceptor_count; ++acceptor){
        const auto& preference =
            acceptor_preferences[static_cast<std::size_t>(acceptor)];
        for(int index = 0; index < static_cast<int>(preference.size()); ++index){
            int proposer = preference[static_cast<std::size_t>(index)];
            if(proposer < 0 || proposer_count <= proposer
                || rank[static_cast<std::size_t>(acceptor)]
                    [static_cast<std::size_t>(proposer)] != -1)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: invalid preference "
                    "(stable_marriage)."
                );
            }
            rank[static_cast<std::size_t>(acceptor)]
                [static_cast<std::size_t>(proposer)] = index;
        }
    }
    for(const auto& preference: proposer_preferences){
        std::vector<unsigned char> seen(
            static_cast<std::size_t>(acceptor_count), 0
        );
        for(int acceptor: preference){
            if(acceptor < 0 || acceptor_count <= acceptor
                || seen[static_cast<std::size_t>(acceptor)] != 0)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: invalid preference "
                    "(stable_marriage)."
                );
            }
            seen[static_cast<std::size_t>(acceptor)] = 1;
        }
    }
    StableMarriageResult result{
        std::vector<int>(static_cast<std::size_t>(proposer_count), -1),
        std::vector<int>(static_cast<std::size_t>(acceptor_count), -1)
    };
    std::vector<std::size_t> next(
        static_cast<std::size_t>(proposer_count), 0
    );
    std::queue<int> free;
    for(int proposer = 0; proposer < proposer_count; ++proposer){
        free.push(proposer);
    }
    while(!free.empty()){
        int proposer = free.front();
        free.pop();
        const auto& preference =
            proposer_preferences[static_cast<std::size_t>(proposer)];
        while(next[static_cast<std::size_t>(proposer)] < preference.size()){
            int acceptor = preference[
                next[static_cast<std::size_t>(proposer)]++
            ];
            int proposer_rank = rank[static_cast<std::size_t>(acceptor)]
                [static_cast<std::size_t>(proposer)];
            if(proposer_rank == -1) continue;
            int previous =
                result.acceptor_match[static_cast<std::size_t>(acceptor)];
            if(previous == -1
                || proposer_rank < rank[static_cast<std::size_t>(acceptor)]
                    [static_cast<std::size_t>(previous)]){
                result.proposer_match[static_cast<std::size_t>(proposer)] =
                    acceptor;
                result.acceptor_match[static_cast<std::size_t>(acceptor)] =
                    proposer;
                if(previous != -1){
                    result.proposer_match[static_cast<std::size_t>(previous)] =
                        -1;
                    free.push(previous);
                }
                break;
            }
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_STABLE_MARRIAGE_HPP_INCLUDED
