// competitive-verifier: STANDALONE
#include <cassert>
#include <cstdint>
#include <iostream>
#include <vector>
#include "../../src/structure/compressed/elias_fano.hpp"
int main() {
    std::vector<std::uint64_t> values{0, 1, 1, 7, 20, 1000};
    EliasFano encoded(values);
    for (std::size_t i = 0; i < values.size(); ++i) assert(encoded.select(i) == values[i]);
    assert(encoded.lower_bound(2) == 3 && encoded.rank(1) == 3);
    std::cout << "OK\n";
}
