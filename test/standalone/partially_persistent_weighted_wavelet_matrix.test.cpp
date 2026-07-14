// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/wavelet_matrix/detail/partially_persistent_weighted_wavelet_matrix_base.hpp"
#include "../../src/structure/wavelet_matrix/partially_persistent_functional_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/partially_persistent_weighted_wavelet_matrix.hpp"

namespace{

using Matrix = PartiallyPersistentWeightedWaveletMatrix<
    int, long long, 256, 384, 32, 19>;

template<class F>
bool throws_runtime_error(F&& operation){
    try{
        operation();
    }catch(const std::runtime_error&){
        return true;
    }
    return false;
}

void self_test(){
    PartiallyPersistentWeightedWaveletMatrix<unsigned, int, 8, 1, 5, 3> matrix(
        std::vector<unsigned>{0, 3, 31, 7},
        std::vector<int>{1, 2, 3, 4}
    );
    int version = matrix.set_weight(2, 9);
    assert(version == 1);
    assert(matrix.access(0, 2) == 31);
    assert(matrix.weight(0, 2) == 3);
    assert(matrix.access(1, 2) == 31);
    assert(matrix.weight(1, 2) == 9);
    assert(throws_runtime_error([&]{ matrix.set(1, 5, 6); }));
    assert(matrix.versions() == 2);
    assert(matrix.access(1, 2) == 31);
    assert(matrix.weight(1, 2) == 9);

    PartiallyPersistentWeightedWaveletMatrix<unsigned, int, 4, 2, 3, 2> narrow(
        std::vector<unsigned>{1, 7}, std::vector<int>{4, 5}
    );
    assert(throws_runtime_error([&]{ narrow.set_value(0, 8); }));
    assert(narrow.versions() == 1);
    assert(narrow.access(0, 0) == 1);
    assert(narrow.weight(0, 0) == 4);

    PartiallyPersistentFunctionalWaveletMatrix<int, 8, 2, long long, 32, 3>
        functional(std::vector<int>{3, -1, 4});
    int functional_version = functional.set(1, 5);
    assert(functional.sum(0, 0, 3) == 6);
    assert(functional.sum(functional_version, 0, 3) == 12);
}

} // namespace

int main(){
    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }

    std::vector<int> values(static_cast<std::size_t>(n));
    std::vector<long long> weights(static_cast<std::size_t>(n));
    for(int& value: values) std::cin >> value;
    for(long long& weight: weights) std::cin >> weight;
    Matrix matrix(values, weights);

    while(q--){
        std::string operation;
        std::cin >> operation;
        if(operation == "SET"){
            int k, value;
            long long weight;
            std::cin >> k >> value >> weight;
            std::cout << matrix.set(k, value, weight) << '\n';
        }else if(operation == "SET_VALUE"){
            int k, value;
            std::cin >> k >> value;
            std::cout << matrix.set_value(k, value) << '\n';
        }else if(operation == "SET_WEIGHT"){
            int k;
            long long weight;
            std::cin >> k >> weight;
            std::cout << matrix.set_weight(k, weight) << '\n';
        }else if(operation == "ACCESS"){
            int version, k;
            std::cin >> version >> k;
            std::cout << matrix.access(version, k) << '\n';
        }else if(operation == "WEIGHT"){
            int version, k;
            std::cin >> version >> k;
            std::cout << matrix.weight(version, k) << '\n';
        }else if(operation == "RANK"){
            int version, value, l, r;
            std::cin >> version >> value >> l >> r;
            std::cout << matrix.rank(version, value, l, r) << '\n';
        }else if(operation == "KTH_SMALL"){
            int version, l, r, k;
            std::cin >> version >> l >> r >> k;
            std::cout << matrix.kth_smallest(version, l, r, k) << '\n';
        }else if(operation == "KTH_LARGE"){
            int version, l, r, k;
            std::cin >> version >> l >> r >> k;
            std::cout << matrix.kth_largest(version, l, r, k) << '\n';
        }else if(operation == "FREQ"){
            int version, l, r, upper;
            std::cin >> version >> l >> r >> upper;
            std::cout << matrix.range_freq(version, l, r, upper) << '\n';
        }else if(operation == "FREQ_RANGE"){
            int version, l, r, lower, upper;
            std::cin >> version >> l >> r >> lower >> upper;
            std::cout << matrix.range_freq(version, l, r, lower, upper) << '\n';
        }else if(operation == "SUM"){
            int version, l, r;
            std::cin >> version >> l >> r;
            std::cout << matrix.sum(version, l, r) << '\n';
        }else if(operation == "SUM_RANGE"){
            int version, l, r, lower, upper;
            std::cin >> version >> l >> r >> lower >> upper;
            std::cout << matrix.range_sum(version, l, r, lower, upper) << '\n';
        }else if(operation == "SUM_K_SMALL"){
            int version, l, r, k;
            std::cin >> version >> l >> r >> k;
            std::cout << matrix.sum_k_smallest(version, l, r, k) << '\n';
        }else if(operation == "SUM_K_LARGE"){
            int version, l, r, k;
            std::cin >> version >> l >> r >> k;
            std::cout << matrix.sum_k_largest(version, l, r, k) << '\n';
        }else if(operation == "PREV"){
            int version, l, r, upper;
            std::cin >> version >> l >> r >> upper;
            std::optional<int> answer = matrix.prev_value(version, l, r, upper);
            if(answer) std::cout << *answer << '\n';
            else std::cout << "NONE\n";
        }else if(operation == "NEXT"){
            int version, l, r, lower;
            std::cin >> version >> l >> r >> lower;
            std::optional<int> answer = matrix.next_value(version, l, r, lower);
            if(answer) std::cout << *answer << '\n';
            else std::cout << "NONE\n";
        }else if(operation == "VERSIONS"){
            std::cout << matrix.versions() << '\n';
        }else if(operation == "TRY_SET"){
            int k, value;
            long long weight;
            std::cin >> k >> value >> weight;
            int old_value = matrix.access(0, k);
            long long old_weight = matrix.weight(0, k);
            bool thrown = throws_runtime_error([&]{ matrix.set(k, value, weight); });
            int latest = matrix.latest_version();
            std::cout << thrown << ' ' << matrix.versions() << ' '
                      << matrix.access(latest, k) << ' ' << matrix.weight(latest, k) << ' '
                      << old_value << ' ' << old_weight << '\n';
        }
    }
}
