// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/structure/array/partially_persistent_array.hpp"

struct ThrowOnCopy{
    static inline bool enabled = false;
    int value = 0;

    ThrowOnCopy() = default;
    explicit ThrowOnCopy(int value): value(value){}
    ThrowOnCopy(const ThrowOnCopy& other): value(other.value){
        if(enabled) throw 1;
    }
};

void test_storage_guarantees(){
    PartiallyPersistentStorage<ThrowOnCopy, 2, 4> storage;
    storage.initialize(0, ThrowOnCopy(1));
    storage.initialize(1, ThrowOnCopy(2));
    storage.write(0, 1, ThrowOnCopy(3));
    const ThrowOnCopy* stable = &storage.current(0);
    storage.write(1, 2, ThrowOnCopy(4));
    assert(&storage.current(0) == stable);
    assert(storage.get(0, 0).value == 1);
    assert(storage.get(0, 1).value == 3);

    const int snapshot = storage.changes();
    ThrowOnCopy::enabled = true;
    try{
        storage.write(0, 3, ThrowOnCopy(5));
        assert(false);
    }catch(int){}
    ThrowOnCopy::enabled = false;
    assert(storage.changes() == snapshot);
    assert(&storage.current(0) == stable);
    storage.write(0, 3, ThrowOnCopy(6));
    storage.rollback(snapshot);
    assert(&storage.current(0) == stable);
}

int main(){
    test_storage_guarantees();
    int n, q;
    if(!(std::cin >> n >> q)) return 0;
    std::vector<std::int64_t> initial(static_cast<std::size_t>(n));
    for(auto& value: initial) std::cin >> value;

    PartiallyPersistentArray<std::int64_t, 32768, 131072> array(initial);
    while(q--){
        std::string operation;
        std::cin >> operation;
        if(operation == "SET"){
            int index;
            std::int64_t value;
            std::cin >> index >> value;
            std::cout << array.set(index, value) << '\n';
        }else if(operation == "GET"){
            int version, index;
            std::cin >> version >> index;
            std::cout << array.get(version, index) << '\n';
        }else{
            int index;
            std::cin >> index;
            std::cout << array.get(index) << '\n';
        }
    }
}
