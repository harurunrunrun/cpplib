// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/wavelet_matrix/partially_persistent_fully_indexable_dictionary.hpp"
#include "../../src/structure/wavelet_matrix/persistent_fully_indexable_dictionary.hpp"

int main(){
    int input_n, input_q;
    if(std::cin >> input_n >> input_q){
        std::string bits;
        std::cin >> bits;
        std::vector<bool> initial(static_cast<std::size_t>(input_n));
        for(int k = 0; k < input_n; k++){
            initial[static_cast<std::size_t>(k)] = bits[static_cast<std::size_t>(k)] == '1';
        }
        auto dictionary =
            std::make_unique<PersistentFullyIndexableDictionary<512, 2048>>(initial);
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "SET"){
                int version, k, value;
                std::cin >> version >> k >> value;
                std::cout << dictionary->set(version, k, value != 0) << '\n';
            }else if(type == "FLIP"){
                int version, k;
                std::cin >> version >> k;
                std::cout << dictionary->flip(version, k) << '\n';
            }else if(type == "FORK"){
                int version;
                std::cin >> version;
                std::cout << dictionary->fork(version) << '\n';
            }else if(type == "GET"){
                int version, k;
                std::cin >> version >> k;
                std::cout << dictionary->get(version, k) << '\n';
            }else if(type == "RANK"){
                int version, value, r;
                std::cin >> version >> value >> r;
                std::cout << dictionary->rank(version, value != 0, r) << '\n';
            }else if(type == "RANGE"){
                int version, value, l, r;
                std::cin >> version >> value >> l >> r;
                std::cout << dictionary->rank(version, value != 0, l, r) << '\n';
            }else if(type == "SELECT"){
                int version, value, occurrence;
                std::cin >> version >> value >> occurrence;
                std::cout << dictionary->select(version, value != 0, occurrence) << '\n';
            }
        }
        return 0;
    }

    constexpr int n = 257;
    constexpr int max_version = 1800;
    std::mt19937 rng(1234567);
    std::vector<bool> initial(n);
    for(int i = 0; i < n; i++) initial[i] = rng() & 1;

    PersistentFullyIndexableDictionary<300, max_version> persistent(initial);
    std::vector<std::vector<bool>> versions(1, initial);

    for(int q = 0; q < max_version; q++){
        int base = static_cast<int>(rng() % versions.size());
        int next_version;
        if(rng() % 5 == 0){
            next_version = persistent.fork(base);
            versions.push_back(versions[static_cast<std::size_t>(base)]);
        }else{
            int k = static_cast<int>(rng() % n);
            auto next = versions[static_cast<std::size_t>(base)];
            if(rng() & 1){
                next[k] = !next[k];
                next_version = persistent.flip(base, k);
            }else{
                bool value = rng() & 1;
                next[k] = value;
                next_version = persistent.set(base, k, value);
            }
            versions.push_back(std::move(next));
        }
        assert(next_version == static_cast<int>(versions.size()) - 1);
        assert(persistent.latest_version() == next_version);

        for(int rep = 0; rep < 8; rep++){
            int version = static_cast<int>(rng() % versions.size());
            int r = static_cast<int>(rng() % (n + 1));
            bool value = rng() & 1;
            assert(persistent.rank(version, value, r) ==
                std::count(versions[static_cast<std::size_t>(version)].begin(),
                    versions[static_cast<std::size_t>(version)].begin() + r,
                    value));
            int k = static_cast<int>(rng() % n);
            assert(persistent.get(version, k) == versions[static_cast<std::size_t>(version)][k]);
        }
    }

    for(int value = 0; value < 2; value++){
        int version = static_cast<int>(rng() % versions.size());
        int occurrence = 0;
        for(int k = 0; k < n; k++){
            if(versions[static_cast<std::size_t>(version)][k] == static_cast<bool>(value)){
                assert(persistent.select(version, value, occurrence++) == k);
            }
        }
        assert(persistent.select(version, value, occurrence) == n);
    }

    PartiallyPersistentFullyIndexableDictionary<300, max_version> partial(initial);
    std::vector<std::vector<bool>> history(1, initial);
    for(int q = 0; q < max_version; q++){
        int k = static_cast<int>(rng() % n);
        auto next = history.back();
        int version;
        if(rng() & 1){
            next[k] = !next[k];
            version = partial.flip(k);
        }else{
            bool value = rng() & 1;
            next[k] = value;
            version = partial.set(k, value);
        }
        history.push_back(std::move(next));
        assert(version == static_cast<int>(history.size()) - 1);
        assert(partial.latest_version() == version);

        for(int rep = 0; rep < 8; rep++){
            int query_version = static_cast<int>(rng() % history.size());
            int r = static_cast<int>(rng() % (n + 1));
            bool value = rng() & 1;
            assert(partial.rank(query_version, value, r) ==
                std::count(history[static_cast<std::size_t>(query_version)].begin(),
                    history[static_cast<std::size_t>(query_version)].begin() + r,
                    value));
            int index = static_cast<int>(rng() % n);
            assert(partial.get(query_version, index) ==
                history[static_cast<std::size_t>(query_version)][index]);
        }
    }

    bool thrown = false;
    try{
        persistent.get(-1, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}
