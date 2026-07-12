// competitive-verifier: STANDALONE

#include "../../src/structure/array/persistent_array.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include "../../src/structure/array/partially_persistent_array.hpp"
#include "../../src/structure/array/rollback_array.hpp"

using Persistent = PersistentArray<int, 2, 2>;
using Partial = PartiallyPersistentArray<int, 2, 2>;
using Rollback = RollbackArray<int, 2, 2>;

static_assert(!std::is_copy_constructible_v<Persistent>);
static_assert(!std::is_copy_assignable_v<Persistent>);
static_assert(!std::is_move_constructible_v<Persistent>);
static_assert(!std::is_move_assignable_v<Persistent>);
static_assert(!std::is_copy_constructible_v<Partial>);
static_assert(!std::is_move_constructible_v<Partial>);
static_assert(!std::is_copy_constructible_v<Rollback>);
static_assert(!std::is_move_constructible_v<Rollback>);

template<class F>
bool throws_runtime_error(F&& f){
    try{
        f();
    }catch(const std::runtime_error&){
        return true;
    }
    return false;
}

int main(){
    int q;
    if(std::cin >> q){
        PersistentArray<int, 4, 8> persistent(2, 0);
        PartiallyPersistentArray<int, 4, 8> partial(2, 0);
        RollbackArray<int, 4, 8> rollback(2, 0);
        while(q--){
            std::string type;
            std::cin >> type;
            try{
                if(type == "PGET"){
                    int version, k;
                    std::cin >> version >> k;
                    std::cout << persistent.get(version, k) << '\n';
                }else if(type == "PSET"){
                    int version, k, value;
                    std::cin >> version >> k >> value;
                    std::cout << persistent.set(version, k, value) << '\n';
                }else if(type == "PFORK"){
                    int version;
                    std::cin >> version;
                    std::cout << persistent.fork(version) << '\n';
                }else if(type == "TGET"){
                    int version, k;
                    std::cin >> version >> k;
                    std::cout << partial.get(version, k) << '\n';
                }else if(type == "TSET"){
                    int k, value;
                    std::cin >> k >> value;
                    std::cout << partial.set(k, value) << '\n';
                }else if(type == "RGET"){
                    int k;
                    std::cin >> k;
                    std::cout << rollback.get(k) << '\n';
                }else if(type == "RSET"){
                    int k, value;
                    std::cin >> k >> value;
                    rollback.set(k, value);
                    std::cout << "OK\n";
                }else if(type == "RUNDO"){
                    rollback.undo();
                    std::cout << "OK\n";
                }else if(type == "RROLLBACK"){
                    int snapshot;
                    std::cin >> snapshot;
                    rollback.rollback(snapshot);
                    std::cout << "OK\n";
                }
            }catch(const std::runtime_error&){
                std::cout << "THROW\n";
            }
        }
        return 0;
    }

    assert(throws_runtime_error([]{ Persistent a(-1); }));
    assert(throws_runtime_error([]{ Persistent a(3); }));
    assert(throws_runtime_error([]{ Partial a(-1); }));
    assert(throws_runtime_error([]{ Rollback a(3); }));

    std::vector<int> oversized_vector(3);
    std::array<int, 3> oversized_array{};
    assert(throws_runtime_error([&]{ Persistent a(oversized_vector); }));
    assert(throws_runtime_error([&]{ Persistent a(oversized_array); }));
    assert(throws_runtime_error([&]{ Rollback a(oversized_vector); }));
    assert(throws_runtime_error([&]{ Rollback a(oversized_array); }));

    Persistent persistent(2, 0);
    assert(throws_runtime_error([&]{ persistent.get(-1, 0); }));
    assert(throws_runtime_error([&]{ persistent.get(1, 0); }));
    assert(throws_runtime_error([&]{ persistent.get(0, -1); }));
    assert(throws_runtime_error([&]{ persistent.get(0, 2); }));
    assert(throws_runtime_error([&]{ persistent.set(-1, 0, 1); }));
    assert(throws_runtime_error([&]{ persistent.set(0, 2, 1); }));
    assert(throws_runtime_error([&]{ persistent.fork(1); }));

    Partial partial(2, 0);
    assert(throws_runtime_error([&]{ partial.get(-1, 0); }));
    assert(throws_runtime_error([&]{ partial.get(0, 2); }));
    assert(throws_runtime_error([&]{ partial.set(-1, 1); }));

    Rollback rollback(2, 0);
    assert(throws_runtime_error([&]{ rollback.get(-1); }));
    assert(throws_runtime_error([&]{ rollback.get(2); }));
    assert(throws_runtime_error([&]{ rollback.set(-1, 1); }));
    assert(throws_runtime_error([&]{ rollback.set(2, 1); }));
    assert(throws_runtime_error([&]{ rollback.undo(); }));
    assert(throws_runtime_error([&]{ rollback.rollback(-1); }));
    assert(throws_runtime_error([&]{ rollback.rollback(1); }));

    rollback.set(0, 1);
    auto discarded = rollback.snapshot();
    rollback.rollback(0);
    assert(throws_runtime_error([&]{ rollback.rollback(discarded); }));
}
