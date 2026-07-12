// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include "../../src/structure/array/persistent_array.hpp"
#include "../../src/structure/array/rollback_array.hpp"

struct ThrowingCopy{
    static inline int copies_before_throw = -1;

    int value;

    explicit ThrowingCopy(int value): value(value){}

    ThrowingCopy(const ThrowingCopy& other): value(other.value){
        if(copies_before_throw == 0){
            throw std::runtime_error("copy failed");
        }
        if(copies_before_throw > 0){
            copies_before_throw--;
        }
    }

    ThrowingCopy& operator=(const ThrowingCopy&) = delete;
};

int main(){
    int q;
    if(std::cin >> q){
        ThrowingCopy::copies_before_throw = -1;
        PersistentArray<ThrowingCopy, 1, 64> persistent(1, ThrowingCopy(1));
        RollbackArray<ThrowingCopy, 1, 64> rollback(1, ThrowingCopy(1));
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "PSET"){
                int version, value, budget;
                std::cin >> version >> value >> budget;
                ThrowingCopy::copies_before_throw = budget;
                try{
                    std::cout << persistent.set(version, 0, ThrowingCopy(value)) << '\n';
                }catch(const std::runtime_error&){
                    std::cout << "THROW\n";
                }
            }else if(type == "RSET"){
                int value, budget;
                std::cin >> value >> budget;
                ThrowingCopy::copies_before_throw = budget;
                try{
                    rollback.set(0, ThrowingCopy(value));
                    std::cout << "OK\n";
                }catch(const std::runtime_error&){
                    std::cout << "THROW\n";
                }
            }else if(type == "PGET"){
                int version;
                std::cin >> version;
                std::cout << persistent.get(version, 0).value << '\n';
            }else if(type == "RGET"){
                std::cout << rollback.get(0).value << '\n';
            }else if(type == "PVERSIONS"){
                std::cout << persistent.versions() << '\n';
            }else if(type == "RHISTORY"){
                std::cout << rollback.history_size() << '\n';
            }else if(type == "RROLLBACK"){
                int snapshot;
                std::cin >> snapshot;
                rollback.rollback(snapshot);
                std::cout << "OK\n";
            }
        }
        return 0;
    }

    ThrowingCopy::copies_before_throw = -1;
    PersistentArray<ThrowingCopy, 1, 2> persistent(1, ThrowingCopy(1));

    ThrowingCopy::copies_before_throw = 0;
    bool thrown = false;
    try{
        persistent.set(0, 0, ThrowingCopy(2));
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    assert(persistent.versions() == 1);
    assert(persistent.get(0, 0).value == 1);

    ThrowingCopy::copies_before_throw = -1;
    RollbackArray<ThrowingCopy, 1, 2> rollback(1, ThrowingCopy(1));

    ThrowingCopy::copies_before_throw = 0;
    thrown = false;
    try{
        rollback.set(0, ThrowingCopy(2));
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    assert(rollback.history_size() == 0);
    assert(rollback.get(0).value == 1);

    ThrowingCopy::copies_before_throw = -1;
    rollback.set(0, ThrowingCopy(2));
    assert(rollback.history_size() == 1);
    assert(rollback.get(0).value == 2);

    ThrowingCopy::copies_before_throw = 0;
    thrown = false;
    try{
        rollback.set(0, ThrowingCopy(3));
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    assert(rollback.history_size() == 1);
    assert(rollback.get(0).value == 2);

    rollback.rollback(0);
    assert(rollback.get(0).value == 1);
}
