// competitive-verifier: PROBLEM https://www.spoj.com/problems/MTOTALF/

#include <iostream>
#include <vector>

#include "../../src/algorithm/graph/letter_pipe_max_flow.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int pipe_count;
    if(!(std::cin >> pipe_count)) return 0;
    std::vector<LetterPipe<long long>> pipes;
    pipes.reserve(static_cast<std::size_t>(pipe_count));
    for(int index = 0; index < pipe_count; index++){
        char from, to;
        long long capacity;
        std::cin >> from >> to >> capacity;
        pipes.push_back({from, to, capacity});
    }
    std::cout << letter_pipe_max_flow<long long>(pipes) << '\n';
}
