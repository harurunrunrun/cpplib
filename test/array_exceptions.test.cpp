// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <vector>
#include "../src/structure/array/partially_persistent_array.hpp"
#include "../src/structure/array/rollback_array.hpp"

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
