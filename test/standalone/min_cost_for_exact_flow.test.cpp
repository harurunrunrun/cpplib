// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/matching/min_cost_flow.hpp"

void self_test(){
    {
        MinCostFlow<long long> graph(4);
        graph.add_edge(0, 1, 2, 1);
        graph.add_edge(0, 2, 1, 5);
        graph.add_edge(1, 2, 1, -2);
        graph.add_edge(1, 3, 1, 2);
        graph.add_edge(2, 3, 2, 1);
        const auto cost = graph.min_cost_for_exact_flow(0, 3, 2);
        assert(cost && *cost == 3);
    }
    {
        MinCostFlow<long long> graph(2);
        graph.add_edge(0, 1, 1, -1);
        const auto cost = graph.min_cost_for_exact_flow(0, 1, 1);
        assert(cost && *cost == -1);
    }
    {
        MinCostFlow<long long> graph(2);
        graph.add_edge(0, 1, 2, 4);
        assert(!graph.min_cost_for_exact_flow(0, 1, 3));
        assert(graph.edges[0].flow == 2);
    }
    {
        MinCostFlow<long long> graph(2);
        const auto cost = graph.min_cost_for_exact_flow(0, 1, 0);
        assert(cost && *cost == 0);
    }
    {
        bool thrown = false;
        try{
            MinCostFlow<long long> graph(2);
            (void)graph.min_cost_for_exact_flow(0, 1, -1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
    }
}

int main(){
    int n, m, source, sink;
    long long required_flow;
    if(!(std::cin >> n >> m >> source >> sink >> required_flow)){
        self_test();
        return 0;
    }
    MinCostFlow<long long> graph(n);
    for(int id = 0; id < m; ++id){
        int from, to;
        long long capacity, cost;
        std::cin >> from >> to >> capacity >> cost;
        graph.add_edge(from, to, capacity, cost);
    }
    const auto answer = graph.min_cost_for_exact_flow(source, sink, required_flow);
    if(answer) std::cout << *answer << '\n';
    else std::cout << "impossible\n";
}
