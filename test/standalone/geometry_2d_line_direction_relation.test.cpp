// competitive-verifier: STANDALONE

#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/geometry/2d/line_direction_relation.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Line first, second;
        std::cin >> first.a.x >> first.a.y >> first.b.x >> first.b.y;
        std::cin >> second.a.x >> second.a.y >> second.b.x >> second.b.y;
        try{
            std::cout << static_cast<int>(
                line_direction_relation(first, second)
            ) << '\n';
        }catch(const std::invalid_argument&){
            std::cout << "ERR\n";
        }
    }
}
