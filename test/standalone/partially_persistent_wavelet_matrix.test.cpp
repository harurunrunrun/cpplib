// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/wavelet_matrix/partially_persistent_wavelet_matrix.hpp"

namespace{

using Matrix = PartiallyPersistentWaveletMatrix<int, 256, 512, 32, 17>;

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
    PartiallyPersistentWaveletMatrix<unsigned, 8, 1, 5, 3> matrix(
        std::vector<unsigned>{0, 3, 31, 7}
    );
    int version = matrix.set(2, 5);
    assert(version == 1);
    assert(matrix.access(0, 2) == 31);
    assert(matrix.access(1, 2) == 5);
    assert(throws_runtime_error([&]{ matrix.set(1, 8); }));
    assert(matrix.versions() == 2);
    assert(matrix.access(0, 2) == 31);
    assert(matrix.access(1, 2) == 5);
    assert(throws_runtime_error([&]{ matrix.set(0, 32); }));
    assert(matrix.versions() == 2);
    assert(matrix.rank(0, 31, 0, 4) == 1);
    assert(matrix.range_freq(1, 0, 4, 8U) == 4);

    PartiallyPersistentWaveletMatrix<int, 0, 0> empty(std::vector<int>{});
    assert(empty.size() == 0);
    assert(empty.versions() == 1);
    assert(empty.rank(0, 0, 0) == 0);
}

} // namespace

int main(){
    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }

    std::vector<int> initial(static_cast<std::size_t>(n));
    for(int& value: initial) std::cin >> value;
    Matrix matrix(initial);

    while(q--){
        std::string operation;
        std::cin >> operation;
        if(operation == "SET"){
            int k, value;
            std::cin >> k >> value;
            std::cout << matrix.set(k, value) << '\n';
        }else if(operation == "ACCESS"){
            int version, k;
            std::cin >> version >> k;
            std::cout << matrix.access(version, k) << '\n';
        }else if(operation == "RANK"){
            int version, value, l, r;
            std::cin >> version >> value >> l >> r;
            std::cout << matrix.rank(version, value, l, r) << '\n';
        }else if(operation == "SELECT"){
            int version, value, k;
            std::cin >> version >> value >> k;
            std::cout << matrix.select(version, value, k) << '\n';
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
            std::cin >> k >> value;
            int old = matrix.access(0, k);
            bool thrown = throws_runtime_error([&]{ matrix.set(k, value); });
            std::cout << thrown << ' ' << matrix.versions() << ' '
                      << matrix.access(matrix.latest_version(), k) << ' ' << old << '\n';
        }
    }
}
