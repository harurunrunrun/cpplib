// competitive-verifier: STANDALONE

#include "../../src/structure/array/persistent_array.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include "../../src/structure/array/partially_persistent_array.hpp"
#include "../../src/structure/array/rollback_array.hpp"

int main(){
    int n, q;
    if(std::cin >> n >> q){
        PersistentArray<long long, 8, 128> persistent(n, -1);
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "SET"){
                int version, k;
                long long value;
                std::cin >> version >> k >> value;
                std::cout << persistent.set(version, k, value) << '\n';
            }else if(type == "FORK"){
                int version;
                std::cin >> version;
                std::cout << persistent.fork(version) << '\n';
            }else if(type == "GET"){
                int version, k;
                std::cin >> version >> k;
                std::cout << persistent.get(version, k) << '\n';
            }else if(type == "VERSIONS"){
                std::cout << persistent.versions() << '\n';
            }
        }
        return 0;
    }

    PersistentArray<int, 1, 0> persistent(1);
    assert(persistent.size() == 1);
    assert(persistent.versions() == 1);
    assert(persistent.get(0, 0) == 0);

    bool thrown = false;
    try{
        persistent.set(0, 0, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        persistent.fork(0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    PersistentArray<int, 5, 10> non_power_of_two(5, -1);
    int version = 0;
    for(int k = 0; k < 5; k++){
        version = non_power_of_two.set(version, k, k * k);
    }
    for(int k = 0; k < 5; k++){
        assert(non_power_of_two.get(0, k) == -1);
        assert(non_power_of_two.get(version, k) == k * k);
    }

    PartiallyPersistentArray<int, 1, 0> partial(1);
    assert(partial.get(0) == 0);

    RollbackArray<int, 1, 0> rollback(1);
    assert(rollback.get(0) == 0);
    rollback.rollback(rollback.snapshot());

    thrown = false;
    try{
        rollback.set(0, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    PersistentArray<int, 4, 1> empty_persistent(0);
    RollbackArray<int, 4, 1> empty_rollback(0);
    assert(empty_persistent.size() == 0);
    assert(empty_rollback.size() == 0);
}
