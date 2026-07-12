// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/array/rollback_array.hpp"

void check_all(const RollbackArray<long long, 64, 512>& array, const std::vector<long long>& naive){
    assert(array.size() == static_cast<int>(naive.size()));
    for(int i = 0; i < array.size(); i++){
        assert(array.get(i) == naive[static_cast<std::size_t>(i)]);
    }
}

void self_test(){
    RollbackArray<long long, 64, 512> array(std::vector<long long>{1, 2, 3, 4});
    std::vector<std::vector<long long>> states = {{1, 2, 3, 4}};
    std::vector<long long> naive = states.back();

    array.set(1, 20);
    naive[1] = 20;
    states.push_back(naive);
    int s = array.snapshot();
    array.set(3, -5);
    naive[3] = -5;
    states.push_back(naive);
    check_all(array, naive);
    array.rollback(s);
    states.resize(static_cast<std::size_t>(s + 1));
    naive = states.back();
    check_all(array, naive);
    array.undo();
    states.pop_back();
    naive = states.back();
    check_all(array, naive);
    assert(!array.can_undo());

    std::mt19937 rng(20261116);
    for(int step = 0; step < 400; step++){
        int type = static_cast<int>(rng() % 5);
        if(type < 3){
            int k = static_cast<int>(rng() % array.size());
            long long x = static_cast<int>(rng() % 2001) - 1000;
            array.set(k, x);
            naive[static_cast<std::size_t>(k)] = x;
            states.push_back(naive);
        }else if(type == 3 && states.size() > 1){
            array.undo();
            states.pop_back();
            naive = states.back();
        }else{
            int snap = static_cast<int>(rng() % states.size());
            array.rollback(snap);
            states.resize(static_cast<std::size_t>(snap + 1));
            naive = states.back();
        }
        assert(array.history_size() == static_cast<int>(states.size()) - 1);
        assert(array.can_undo() == (states.size() > 1));
        check_all(array, naive);
    }

    bool thrown = false;
    try{
        (void)array.get(array.size());
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
    RollbackArray<long long, 256, 2048> array(initial);
    while(q--){
        std::string type;
        std::cin >> type;
        if(type == "SET"){
            int k;
            long long x;
            std::cin >> k >> x;
            array.set(k, x);
        }else if(type == "GET"){
            int k;
            std::cin >> k;
            std::cout << array.get(k) << '\n';
        }else if(type == "SNAP"){
            std::cout << array.snapshot() << '\n';
        }else if(type == "UNDO"){
            array.undo();
        }else if(type == "ROLLBACK"){
            int snap;
            std::cin >> snap;
            array.rollback(snap);
        }else if(type == "HISTORY"){
            std::cout << array.history_size() << '\n';
        }else if(type == "CAN"){
            std::cout << array.can_undo() << '\n';
        }else if(type == "SIZE"){
            std::cout << array.size() << '\n';
        }
    }
}
