// competitive-verifier: PROBLEM https://www.spoj.com/problems/TEMPLEQ/

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "../../src/structure/segtree/lazysegtree.hpp"
#include "../../src/structure/types/common_monoids.hpp"

constexpr AddMaxMonoidAct<long long> add_max_monoid{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int queue_count, query_count;
    std::cin >> queue_count >> query_count;

    std::vector<std::pair<long long, int>> queues;
    queues.reserve(static_cast<std::size_t>(queue_count));
    for(int original = 0; original < queue_count; original++){
        long long length;
        std::cin >> length;
        queues.emplace_back(length, original);
    }
    std::sort(queues.begin(), queues.end());

    std::vector<long long> sorted_length(static_cast<std::size_t>(queue_count));
    std::vector<int> original_at_position(static_cast<std::size_t>(queue_count));
    std::vector<int> position_of_original(static_cast<std::size_t>(queue_count));
    for(int position = 0; position < queue_count; position++){
        sorted_length[position] = queues[position].first;
        original_at_position[position] = queues[position].second;
        position_of_original[queues[position].second] = position;
    }
    auto tree = std::make_unique<LazySegtree<add_max_monoid, 100000>>(
        sorted_length
    );

    const auto first_at_least = [&tree](long long threshold){
        return tree->max_right(
            0,
            [](long long maximum, long long limit){ return maximum < limit; },
            threshold
        );
    };

    while(query_count--){
        int type;
        long long argument;
        std::cin >> type >> argument;
        if(type == 1){
            const int original = static_cast<int>(argument) - 1;
            const int position = position_of_original[original];
            const long long length = tree->get(position);
            const int last_equal = first_at_least(length + 1) - 1;

            const int displaced_original = original_at_position[last_equal];
            std::swap(
                original_at_position[position],
                original_at_position[last_equal]
            );
            position_of_original[original] = last_equal;
            position_of_original[displaced_original] = position;
            tree->apply(last_equal, 1LL);
        }else if(type == 2){
            std::cout << queue_count - first_at_least(argument) << '\n';
        }else{
            const int begin = first_at_least(argument);
            tree->apply(begin, queue_count, -1LL);
        }
    }
}
