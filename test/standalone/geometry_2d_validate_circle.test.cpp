// competitive-verifier: STANDALONE

#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/geometry/2d/predicate/validate_circle.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        long double radius;
        std::cin >> radius;
        try{
            validate_circle({{0.0L, 0.0L}, radius});
            std::cout << "OK\n";
        }catch(const std::invalid_argument&){
            std::cout << "INVALID\n";
        }
    }
}
