#include <cassert>
#include <stdexcept>
#include "../../../src/structure/array/persistent_array.hpp"
#include "../../../src/structure/array/rollback_array.hpp"

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

inline void run_array_exception_safety_standalone_check(){
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
