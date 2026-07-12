// competitive-verifier: STANDALONE

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/wavelet_matrix/partially_persistent_fully_indexable_dictionary.hpp"
#include "../../src/structure/wavelet_matrix/partially_persistent_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/partially_persistent_weighted_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/persistent_fully_indexable_dictionary.hpp"
#include "../../src/structure/wavelet_matrix/persistent_wavelet_matrix.hpp"
#include "../../src/structure/wavelet_matrix/persistent_weighted_wavelet_matrix.hpp"

template<class F>
bool throws_capacity(F&& operation){
    try{
        operation();
    }catch(const std::runtime_error&){
        return true;
    }
    return false;
}

int main(){
    int q;
    if(!(std::cin >> q)) return 0;
    while(q--){
        std::string type;
        std::cin >> type;
        if(type == "PFID_SET"){
            PersistentFullyIndexableDictionary<8, 1> dictionary(
                std::vector<bool>{false, true}
            );
            int version = dictionary.set(0, 0, true);
            bool thrown = throws_capacity([&]{ dictionary.set(version, 1, false); });
            std::cout << thrown << ' ' << dictionary.versions() << ' '
                      << dictionary.get(0, 0) << ' ' << dictionary.get(version, 0) << ' '
                      << dictionary.get(version, 1) << '\n';
        }else if(type == "PFID_FORK"){
            PersistentFullyIndexableDictionary<8, 1> dictionary(
                std::vector<bool>{false, true}
            );
            int version = dictionary.fork(0);
            bool thrown = throws_capacity([&]{ dictionary.fork(version); });
            std::cout << thrown << ' ' << dictionary.versions() << ' '
                      << dictionary.get(version, 1) << '\n';
        }else if(type == "PPFID_SET"){
            PartiallyPersistentFullyIndexableDictionary<8, 1> dictionary(
                std::vector<bool>{false, true}
            );
            int version = dictionary.set(0, true);
            bool thrown = throws_capacity([&]{ dictionary.set(1, false); });
            std::cout << thrown << ' ' << dictionary.versions() << ' '
                      << dictionary.get(0, 0) << ' ' << dictionary.get(version, 0) << ' '
                      << dictionary.get(version, 1) << '\n';
        }else if(type == "PWM_SET"){
            PersistentWaveletMatrix<int, 8, 1, 32, 4> matrix(
                std::vector<int>{1, 2}
            );
            int version = matrix.set(0, 0, 3);
            bool thrown = throws_capacity([&]{ matrix.set(version, 1, 4); });
            std::cout << thrown << ' ' << matrix.versions() << ' '
                      << matrix.access(0, 0) << ' ' << matrix.access(version, 0) << ' '
                      << matrix.access(version, 1) << '\n';
        }else if(type == "PWM_FORK"){
            PersistentWaveletMatrix<int, 8, 1, 32, 4> matrix(
                std::vector<int>{1, 2}
            );
            int version = matrix.fork(0);
            bool thrown = throws_capacity([&]{ matrix.fork(version); });
            std::cout << thrown << ' ' << matrix.versions() << ' '
                      << matrix.access(version, 1) << '\n';
        }else if(type == "PPWM_SET"){
            PartiallyPersistentWaveletMatrix<int, 8, 1, 32, 4> matrix(
                std::vector<int>{1, 2}
            );
            int version = matrix.set(0, 3);
            bool thrown = throws_capacity([&]{ matrix.set(1, 4); });
            std::cout << thrown << ' ' << matrix.versions() << ' '
                      << matrix.access(0, 0) << ' ' << matrix.access(version, 0) << ' '
                      << matrix.access(version, 1) << '\n';
        }else if(type == "PWWM_SET"){
            PersistentWeightedWaveletMatrix<int, long long, 8, 1, 32, 4> matrix(
                std::vector<int>{1, 2}, std::vector<long long>{10, 20}
            );
            int version = matrix.set(0, 0, 3, 30);
            bool thrown = throws_capacity([&]{ matrix.set(version, 1, 4, 40); });
            std::cout << thrown << ' ' << matrix.versions() << ' '
                      << matrix.access(0, 0) << ' ' << matrix.weight(0, 0) << ' '
                      << matrix.access(version, 0) << ' ' << matrix.weight(version, 0) << '\n';
        }else if(type == "PWWM_FORK"){
            PersistentWeightedWaveletMatrix<int, long long, 8, 1, 32, 4> matrix(
                std::vector<int>{1, 2}, std::vector<long long>{10, 20}
            );
            int version = matrix.fork(0);
            bool thrown = throws_capacity([&]{ matrix.fork(version); });
            std::cout << thrown << ' ' << matrix.versions() << ' '
                      << matrix.access(version, 1) << ' ' << matrix.weight(version, 1) << '\n';
        }else if(type == "PPWWM_SET"){
            PartiallyPersistentWeightedWaveletMatrix<int, long long, 8, 1, 32, 4> matrix(
                std::vector<int>{1, 2}, std::vector<long long>{10, 20}
            );
            int version = matrix.set(0, 3, 30);
            bool thrown = throws_capacity([&]{ matrix.set(1, 4, 40); });
            std::cout << thrown << ' ' << matrix.versions() << ' '
                      << matrix.access(0, 0) << ' ' << matrix.weight(0, 0) << ' '
                      << matrix.access(version, 0) << ' ' << matrix.weight(version, 0) << '\n';
        }
    }
}
