// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "../../src/structure/wavelet_matrix/dynamic_fully_indexable_dictionary.hpp"
#include "../../src/structure/wavelet_matrix/fully_indexable_dictionary.hpp"

int main(){
    int input_n, q;
    if(std::cin >> input_n >> q){
        std::vector<bool> input(static_cast<std::size_t>(input_n));
        for(int k = 0; k < input_n; k++){
            int value;
            std::cin >> value;
            input[static_cast<std::size_t>(k)] = value;
        }
        FullyIndexableDictionary<512> fixed(input);
        DynamicFullyIndexableDictionary<512> dynamic(input);
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "FGET" || type == "DGET"){
                int k;
                std::cin >> k;
                std::cout << (type == "FGET" ? fixed[k] : dynamic[k]) << '\n';
            }else if(type == "FRANK" || type == "DRANK"){
                int value, l, r;
                std::cin >> value >> l >> r;
                if(type == "FRANK") std::cout << fixed.rank(value, l, r) << '\n';
                else std::cout << dynamic.rank(value, l, r) << '\n';
            }else if(type == "FSELECT" || type == "DSELECT"){
                int value, k;
                std::cin >> value >> k;
                if(type == "FSELECT") std::cout << fixed.select(value, k) << '\n';
                else std::cout << dynamic.select(value, k) << '\n';
            }else if(type == "DSET"){
                int k, value;
                std::cin >> k >> value;
                dynamic.set(k, value);
            }else if(type == "DFLIP"){
                int k;
                std::cin >> k;
                dynamic.flip(k);
            }
        }
        return 0;
    }

    constexpr int n = 257;
    std::mt19937 rng(712367);
    std::vector<bool> values(n);
    for(int k = 0; k < n; k++) values[k] = rng() & 1;

    FullyIndexableDictionary<300> fixed(values);
    DynamicFullyIndexableDictionary<300> dynamic(values);

    auto check = [&]{
        for(int value = 0; value < 2; value++){
            int count = 0;
            for(int r = 0; r <= n; r++){
                assert(fixed.rank(value, r) == count);
                assert(dynamic.rank(value, r) == count);
                if(r < n && values[r] == static_cast<bool>(value)) count++;
            }
            for(int k = 0; k <= count; k++){
                int expected = n;
                int order = 0;
                for(int p = 0; p < n; p++){
                    if(values[p] == static_cast<bool>(value) && order++ == k){
                        expected = p;
                        break;
                    }
                }
                assert(fixed.select(value, k) == expected);
                assert(dynamic.select(value, k) == expected);
            }
        }
    };
    check();

    for(int step = 0; step < 1000; step++){
        int k = static_cast<int>(rng() % n);
        if(rng() & 1){
            values[k] = !values[k];
            dynamic.flip(k);
        }else{
            bool value = rng() & 1;
            values[k] = value;
            dynamic.set(k, value);
        }
        for(int query = 0; query < 10; query++){
            int r = static_cast<int>(rng() % (n + 1));
            bool value = rng() & 1;
            assert(dynamic.rank(value, r) ==
                std::count(values.begin(), values.begin() + r, value));
        }
    }
}
