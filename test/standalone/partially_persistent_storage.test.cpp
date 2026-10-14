// competitive-verifier: STANDALONE

#define PROBLEM "https://example.com/standalone/partially_persistent_storage"

#include <cassert>
#include <iostream>
#include "../../src/structure/array/static_or_vector.hpp"
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/persistence/partially_persistent_storage.hpp"

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
    {
        using cpplib_detail::checked_int_size;
        const std::size_t maximum = static_cast<std::size_t>(std::numeric_limits<int>::max());
        assert(checked_int_size(maximum) == std::numeric_limits<int>::max());
        bool thrown = false;
        try{ (void)checked_int_size(maximum + 1); }catch(const std::length_error&){ thrown = true; }
        assert(thrown);
        PartiallyPersistentStorage<bool> storage;
        storage.initialize(0, true);
        for(int i = 0; i < 100; ++i) storage.write(0, i, storage.current(0));
        const bool& retained = storage.current(0);
        assert(retained && storage.get(0, 0));
        storage.write(0, 101, false);
        assert(!storage.current(0)); storage.rollback(0); assert(storage.current(0));
    }

    {
        cpplib_detail::StaticOrVector<int, -1> values;
        values.ensure(3); values[1] = 7; values.ensure(129);
        assert(values[1] == 7 && values.size() == 129);
        values.resize(2); assert(values.at(1) == 7);
        cpplib_detail::StaticOrVector<int, 3> fixed;
        fixed.ensure(3); bool threw = false;
        try{fixed.ensure(4);}catch(const std::length_error&){threw = true;}
        assert(threw && fixed.size() == 3);
    }
    {PartiallyPersistentStorage<std::string> s; s.initialize(19,"initial"); for(int i=0;i<40;++i)s.write(19,i,s.current(19)); assert(s.get(19,0)=="initial"); s.write(19,41,"new"); assert(s.get(19,40)=="initial"); assert(s.current(19)=="new"); s.rollback(0); assert(s.current(19)=="initial");}

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
