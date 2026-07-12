// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../src/structure/array/persistent_array.hpp"
#include "../src/structure/array/rollback_array.hpp"

struct NonDefaultValue{
    int value;

    NonDefaultValue() = delete;
    explicit NonDefaultValue(int value): value(value){}
};

int main(){
    constexpr int N = 37;
    constexpr int MAX_VERSION = 3000;

    std::mt19937_64 rng(0x3141592653589793ULL);

    PersistentArray<std::int64_t, 64, MAX_VERSION> persistent(N);
    std::vector<std::vector<std::int64_t>> versions(
        1, std::vector<std::int64_t>(N)
    );

    for(int q = 0; q < MAX_VERSION; q++){
        int base = (int)(rng() % versions.size());
        int version;

        if(rng() % 4 != 0){
            int k = (int)(rng() % N);
            std::int64_t x = (std::int64_t)(rng() % 2000001) - 1000000;
            auto next = versions[base];
            next[k] = x;
            version = persistent.set(base, k, x);
            versions.push_back(std::move(next));
        }else{
            version = persistent.fork(base);
            versions.push_back(versions[base]);
        }

        assert(version == q + 1);
        assert(persistent.versions() == q + 2);
        assert(persistent.latest_version() == q + 1);

        for(int rep = 0; rep < 4; rep++){
            int v = (int)(rng() % versions.size());
            int k = (int)(rng() % N);
            assert(persistent.get(v, k) == versions[v][k]);
        }
    }

    bool thrown = false;
    try{
        persistent.fork(0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    std::array<NonDefaultValue, 3> initial{
        NonDefaultValue(1), NonDefaultValue(2), NonDefaultValue(3)
    };
    PersistentArray<NonDefaultValue, 3, 3> non_default(initial);
    int branch = non_default.set(0, 1, NonDefaultValue(8));
    int copy = non_default.fork(0);
    assert(non_default.get(0, 1).value == 2);
    assert(non_default.get(branch, 1).value == 8);
    assert(non_default.get(copy, 1).value == 2);

    constexpr int MAX_HISTORY = 3000;
    RollbackArray<std::int64_t, 64, MAX_HISTORY> rollback(N);
    std::vector<std::vector<std::int64_t>> states(
        1, std::vector<std::int64_t>(N)
    );

    for(int q = 0; q < 10000; q++){
        int operation = (int)(rng() % 4);
        if(operation < 2 && rollback.history_size() < MAX_HISTORY){
            int k = (int)(rng() % N);
            std::int64_t x = (std::int64_t)(rng() % 2000001) - 1000000;
            auto next = states.back();
            next[k] = x;
            rollback.set(k, x);
            states.push_back(std::move(next));
        }else if(operation == 2 && rollback.can_undo()){
            rollback.undo();
            states.pop_back();
        }else{
            int snapshot = (int)(rng() % states.size());
            rollback.rollback(snapshot);
            states.resize(snapshot + 1);
        }

        assert(rollback.snapshot() == (int)states.size() - 1);
        assert(rollback.can_undo() == (states.size() != 1));
        for(int rep = 0; rep < 4; rep++){
            int k = (int)(rng() % N);
            assert(rollback.get(k) == states.back()[k]);
        }
    }

    RollbackArray<NonDefaultValue, 3, 2> rollback_non_default(initial);
    auto snapshot = rollback_non_default.snapshot();
    rollback_non_default.set(0, NonDefaultValue(9));
    assert(rollback_non_default.get(0).value == 9);
    rollback_non_default.rollback(snapshot);
    assert(rollback_non_default.get(0).value == 1);

    RollbackArray<int, 1, 1> capacity(1, 0);
    capacity.set(0, 1);
    thrown = false;
    try{
        capacity.set(0, 2);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    capacity.undo();
    assert(capacity.get(0) == 0);
}
