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
            }else if(type == "DINSERT"){
                int k, value;
                std::cin >> k >> value;
                dynamic.insert(k, value != 0);
            }else if(type == "DERASE"){
                int k;
                std::cin >> k;
                std::cout << dynamic.erase(k) << '\n';
            }else if(type == "DPUSH"){
                int value;
                std::cin >> value;
                dynamic.push_back(value != 0);
            }else if(type == "DPOP"){
                std::cout << dynamic.pop_back() << '\n';
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

    for(int length = 0; length <= 130; ++length){
        for(int pattern = 0; pattern < 4; ++pattern){
            std::vector<bool> bits(static_cast<std::size_t>(length));
            for(int i = 0; i < length; ++i){
                if(pattern == 1) bits[static_cast<std::size_t>(i)] = true;
                else if(pattern == 2) bits[static_cast<std::size_t>(i)] = (i & 1) != 0;
                else if(pattern == 3) bits[static_cast<std::size_t>(i)] = i % 3 == 0;
            }
            FullyIndexableDictionary<130> dictionary(bits);
            for(int value = 0; value < 2; ++value){
                std::vector<int> position;
                for(int i = 0; i < length; ++i){
                    if(bits[static_cast<std::size_t>(i)] == static_cast<bool>(value)) position.push_back(i);
                }
                for(int k = 0; k <= static_cast<int>(position.size()); ++k){
                    const int expected = k == static_cast<int>(position.size()) ? length : position[static_cast<std::size_t>(k)];
                    assert(dictionary.select(value, k) == expected);
                }
            }
        }
    }

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

    std::vector<bool> changing(values.begin(), values.begin() + 41);
    DynamicFullyIndexableDictionary<512> sequence(changing);
    for(int step = 0; step < 5000; step++){
        const int operation = static_cast<int>(rng() % 6);
        if(operation == 0 && changing.size() < 500){
            const int position = static_cast<int>(rng() % (changing.size() + 1));
            const bool value = (rng() & 1U) != 0;
            changing.insert(changing.begin() + position, value);
            sequence.insert(position, value);
        }else if(operation == 1 && !changing.empty()){
            const int position = static_cast<int>(rng() % changing.size());
            const bool expected = changing[static_cast<std::size_t>(position)];
            changing.erase(changing.begin() + position);
            assert(sequence.erase(position) == expected);
        }else if(operation == 2 && changing.size() < 500){
            const bool value = (rng() & 1U) != 0;
            changing.push_back(value);
            sequence.push_back(value);
        }else if(operation == 3 && !changing.empty()){
            const bool expected = changing.back();
            changing.pop_back();
            assert(sequence.pop_back() == expected);
        }else if(operation == 4 && !changing.empty()){
            const int position = static_cast<int>(rng() % changing.size());
            changing[static_cast<std::size_t>(position)] =
                !changing[static_cast<std::size_t>(position)];
            sequence.flip(position);
        }else if(operation == 5 && !changing.empty()){
            const int position = static_cast<int>(rng() % changing.size());
            const bool value = (rng() & 1U) != 0;
            changing[static_cast<std::size_t>(position)] = value;
            sequence.set(position, value);
        }
        assert(sequence.size() == static_cast<int>(changing.size()));
        for(int query = 0; query < 4; query++){
            const int r = static_cast<int>(rng() % (changing.size() + 1));
            const bool value = (rng() & 1U) != 0;
            assert(sequence.rank(value, r) ==
                std::count(changing.begin(), changing.begin() + r, value));
            const int occurrence = static_cast<int>(
                rng() % (changing.size() + 1)
            );
            int expected = static_cast<int>(changing.size());
            int seen = 0;
            for(int i = 0; i < static_cast<int>(changing.size()); i++){
                if(changing[static_cast<std::size_t>(i)] == value &&
                   seen++ == occurrence){
                    expected = i;
                    break;
                }
            }
            assert(sequence.select(value, occurrence) == expected);
        }
    }
}
