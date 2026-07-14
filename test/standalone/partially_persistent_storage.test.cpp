// competitive-verifier: STANDALONE

#define PROBLEM "https://example.com/standalone/partially_persistent_storage"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/other/partially_persistent_storage.hpp"

void self_test(){
    PartiallyPersistentStorage<std::string, 3, 8> storage;
    storage.initialize(0, "zero");
    storage.initialize(1, "one");
    storage.initialize(2, "two");
    assert(storage.changes() == 0);
    assert(storage.current(0) == "zero");
    assert(storage.get(1, -100) == "one");

    storage.write(0, 2, "a");
    storage.write(1, 3, "b");
    storage.write(0, 5, "c");
    assert(storage.changes() == 3);
    assert(storage.current(0) == "c");
    assert(storage.current(1) == "b");
    assert(storage.get(0, 1) == "zero");
    assert(storage.get(0, 2) == "a");
    assert(storage.get(0, 4) == "a");
    assert(storage.get(0, 5) == "c");

    storage.rollback(1);
    assert(storage.changes() == 1);
    assert(storage.current(0) == "a");
    assert(storage.current(1) == "one");
    storage.write(2, 7, "tail");
    assert(storage.current(2) == "tail");
    storage.rollback(0);
    assert(storage.current(0) == "zero");
    assert(storage.current(2) == "two");

    bool thrown = false;
    try{ storage.initialize(-1, "bad"); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ storage.rollback(1); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);

    PartiallyPersistentStorage<int, 1, 2> full;
    full.initialize(0, 0);
    full.write(0, 1, 1);
    full.write(0, 2, 2);
    thrown = false;
    try{ full.write(0, 3, 3); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    assert(full.changes() == 2);
    assert(full.current(0) == 2);
}

int main(){
    self_test();

    int size;
    int query_count;
    if(!(std::cin >> size >> query_count)) return 0;
    PartiallyPersistentStorage<long long, 16, 4096> storage;
    for(int index = 0; index < size; ++index){
        long long value;
        std::cin >> value;
        storage.initialize(index, value);
    }
    while(query_count-- > 0){
        std::string operation;
        std::cin >> operation;
        if(operation == "WRITE"){
            int index;
            int version;
            long long value;
            std::cin >> index >> version >> value;
            storage.write(index, version, value);
        }else if(operation == "ROLLBACK"){
            int snapshot;
            std::cin >> snapshot;
            storage.rollback(snapshot);
        }else if(operation == "CURRENT"){
            int index;
            std::cin >> index;
            std::cout << storage.current(index) << '\n';
        }else if(operation == "GET"){
            int index;
            int version;
            std::cin >> index >> version;
            std::cout << storage.get(index, version) << '\n';
        }else if(operation == "CHANGES"){
            std::cout << storage.changes() << '\n';
        }
    }
}
