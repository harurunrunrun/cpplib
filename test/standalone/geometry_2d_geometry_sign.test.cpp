// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/geometry_sign.hpp"
#include "geometry_2d_test_common.hpp"

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        long double value;
        std::cin >> value;
        std::cout << geometry_sign(value) << '\n';
    }
}
