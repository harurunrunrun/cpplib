// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/math/linear_algebra/gf2_determinant.hpp"

void self_test(){
    assert(gf2_determinant({}) == 1);
    assert(gf2_determinant({"1"}) == 1);
    assert(gf2_determinant({"0"}) == 0);
    assert(gf2_determinant({"10", "01"}) == 1);
    assert(gf2_determinant({"11", "11"}) == 0);

    std::vector<std::string> identity(65, std::string(65, '0'));
    for(std::size_t i = 0; i < identity.size(); ++i) identity[i][i] = '1';
    assert(gf2_determinant(identity) == 1);

    bool thrown = false;
    try{ (void)gf2_determinant({"10"}); }
    catch(const std::invalid_argument&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)gf2_determinant({"2"}); }
    catch(const std::invalid_argument&){ thrown = true; }
    assert(thrown);
}

int main(){
    self_test();
    int size;
    if(!(std::cin >> size)) return 0;
    std::vector<std::string> matrix(static_cast<std::size_t>(size));
    for(std::string& row: matrix) std::cin >> row;
    std::cout << gf2_determinant(matrix) << '\n';
}
