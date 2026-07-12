#include <algorithm>
#include <cassert>
#include <random>
#include <vector>
#include "../../../src/structure/wavelet_matrix/dynamic_fully_indexable_dictionary.hpp"
#include "../../../src/structure/wavelet_matrix/fully_indexable_dictionary.hpp"

inline void run_fully_indexable_dictionary_standalone_check(){
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
