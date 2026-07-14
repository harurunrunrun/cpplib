// competitive-verifier: PROBLEM https://www.spoj.com/problems/GSS8/

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

#include "../../src/structure/bbst/dynamic_power_moment_sequence.hpp"

namespace{

std::uint32_t read_value(){
    std::uint64_t value;
    std::cin >> value;
    return static_cast<std::uint32_t>(value);
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    std::cin >> size;
    std::vector<std::uint32_t> values(static_cast<std::size_t>(size));
    for(auto& value: values) value = read_value();
    DynamicPowerMomentSequence<std::uint32_t, 10, 200000> sequence(values);

    int query_count;
    std::cin >> query_count;
    while(query_count-- != 0){
        char operation;
        int position;
        std::cin >> operation >> position;
        if(operation == 'I') sequence.insert(position, read_value());
        else if(operation == 'D') sequence.erase(position);
        else if(operation == 'R') sequence.set(position, read_value());
        else{
            int right;
            std::size_t power;
            std::cin >> right >> power;
            std::cout << sequence.power_moment(position, right + 1, power)
                      << '\n';
        }
    }
}
