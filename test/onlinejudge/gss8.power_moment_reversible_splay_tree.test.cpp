#define PROBLEM "https://www.spoj.com/problems/GSS8/"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

#include "../../src/structure/bbst/reversible_splay_tree.hpp"
#include "../../src/structure/types/power_moment_monoid.hpp"

namespace{

constexpr std::size_t max_power = 10;
constexpr PowerMomentMonoid<std::uint32_t, max_power> power_moment_monoid{};
using Aggregate = PowerMomentAggregate<std::uint32_t, max_power>;

std::uint32_t read_value(){
    std::uint64_t value;
    std::cin >> value;
    return static_cast<std::uint32_t>(value);
}

Aggregate singleton(std::uint32_t value){
    return power_moment_singleton<std::uint32_t, max_power>(value);
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<Aggregate> initial(static_cast<std::size_t>(size));
    for(auto& value: initial) value = singleton(read_value());

    ReversibleSplayTree<power_moment_monoid, 200000> sequence(initial);
    int query_count;
    std::cin >> query_count;
    while(query_count-- != 0){
        char operation;
        int position;
        std::cin >> operation >> position;
        if(operation == 'I'){
            sequence.insert(position, singleton(read_value()));
        }else if(operation == 'D'){
            sequence.erase(position);
        }else if(operation == 'R'){
            sequence.set(position, singleton(read_value()));
        }else{
            int right;
            std::size_t power;
            std::cin >> right >> power;
            std::cout << sequence.prod(position, right + 1).moment[power] << '\n';
        }
    }
}
