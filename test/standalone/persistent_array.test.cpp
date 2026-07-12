// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/array/persistent_array.hpp"

void check_version(
    const PersistentArray<long long, 64, 512>& array,
    const std::vector<std::vector<long long>>& naive,
    int version
){
    assert(array.size() == static_cast<int>(naive[0].size()));
    for(int i = 0; i < array.size(); i++){
        assert(array.get(version, i) == naive[static_cast<std::size_t>(version)][static_cast<std::size_t>(i)]);
    }
}

void self_test(){
    PersistentArray<long long, 64, 512> array(std::vector<long long>{1, 2, 3, 4});
    std::vector<std::vector<long long>> naive = {{1, 2, 3, 4}};

    int v1 = array.set(0, 1, 20);
    naive.push_back({1, 20, 3, 4});
    int v2 = array.set(v1, 3, -5);
    naive.push_back({1, 20, 3, -5});
    int v3 = array.fork(0);
    naive.push_back(naive[0]);
    int v4 = array.set(v3, 0, 100);
    naive.push_back({100, 2, 3, 4});
    assert(v1 == 1 && v2 == 2 && v3 == 3 && v4 == 4);
    assert(array.versions() == 5);
    assert(array.latest_version() == 4);
    for(int v = 0; v < array.versions(); v++) check_version(array, naive, v);

    std::mt19937 rng(20261116);
    while(array.versions() < 250){
        int base = static_cast<int>(rng() % naive.size());
        int type = static_cast<int>(rng() % 5);
        if(type == 0){
            int next = array.fork(base);
            naive.push_back(naive[static_cast<std::size_t>(base)]);
            assert(next == static_cast<int>(naive.size()) - 1);
        }else{
            int k = static_cast<int>(rng() % array.size());
            long long x = static_cast<int>(rng() % 2001) - 1000;
            std::vector<long long> expected = naive[static_cast<std::size_t>(base)];
            expected[static_cast<std::size_t>(k)] = x;
            int next = array.set(base, k, x);
            naive.push_back(expected);
            assert(next == static_cast<int>(naive.size()) - 1);
        }
        int target = static_cast<int>(rng() % naive.size());
        check_version(array, naive, target);
    }

    bool thrown = false;
    try{
        (void)array.get(array.versions(), 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }
    std::vector<long long> initial(static_cast<std::size_t>(n));
    for(auto& x: initial) std::cin >> x;
    PersistentArray<long long, 256, 2048> array(initial);
    while(q--){
        std::string type;
        std::cin >> type;
        if(type == "SET"){
            int v, k;
            long long x;
            std::cin >> v >> k >> x;
            std::cout << array.set(v, k, x) << '\n';
        }else if(type == "FORK"){
            int v;
            std::cin >> v;
            std::cout << array.fork(v) << '\n';
        }else if(type == "GET"){
            int v, k;
            std::cin >> v >> k;
            std::cout << array.get(v, k) << '\n';
        }else if(type == "SIZE"){
            std::cout << array.size() << '\n';
        }else if(type == "VERSIONS"){
            std::cout << array.versions() << '\n';
        }else if(type == "LATEST"){
            std::cout << array.latest_version() << '\n';
        }
    }
}
