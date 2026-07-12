// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "../../src/structure/wavelet_matrix/persistent_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/persistent_weighted_wavelet_matrix.hpp"

namespace{

constexpr int max_size = 32768;
constexpr int max_version = 30000;
constexpr int block_size = 8;

using Plain = PersistentWaveletMatrix<
    int, max_size, max_version, 32, block_size>;
using Weighted = PersistentWeightedWaveletMatrix<
    int, long long, max_size, max_version, 32, block_size>;

static_assert(sizeof(Plain) < 16 * 1024 * 1024);
static_assert(sizeof(Weighted) < 32 * 1024 * 1024);

} // namespace

int main(){
    int n, q;
    if(!(std::cin >> n >> q)) return 0;

    std::vector<int> values(static_cast<std::size_t>(n));
    std::vector<long long> weights(static_cast<std::size_t>(n));
    for(int& value: values) std::cin >> value;
    for(long long& weight: weights) std::cin >> weight;

    auto plain = std::make_unique<Plain>(values);
    auto weighted = std::make_unique<Weighted>(values, weights);

    while(q--){
        std::string operation;
        std::cin >> operation;
        if(operation == "FORK"){
            int version, probe;
            std::cin >> version >> probe;
            int plain_version = plain->fork(version);
            int weighted_version = weighted->fork(version);
            assert(plain_version == weighted_version);
            std::cout << plain_version << ' '
                      << plain->access(plain_version, probe) << ' '
                      << weighted->weight(weighted_version, probe) << '\n';
        }else if(operation == "SET"){
            int version, k, value;
            long long weight;
            std::cin >> version >> k >> value >> weight;
            int plain_version = plain->set(version, k, value);
            int weighted_version = weighted->set(version, k, value, weight);
            assert(plain_version == weighted_version);
            std::cout << plain_version << ' '
                      << plain->access(plain_version, k) << ' '
                      << weighted->weight(weighted_version, k) << ' '
                      << plain->access(version, k) << ' '
                      << weighted->weight(version, k) << '\n';
        }else if(operation == "VERSIONS"){
            assert(plain->versions() == weighted->versions());
            std::cout << plain->versions() << '\n';
        }
    }
}
