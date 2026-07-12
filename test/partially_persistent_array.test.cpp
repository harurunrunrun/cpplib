// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <vector>
#include "../src/structure/array/partially_persistent_array.hpp"

struct Box{
    int value;

    Box() = delete;
    explicit Box(int value): value(value){}
};

int main(){
    constexpr int N = 37;
    constexpr int MAX_UPDATE = 5000;

    std::mt19937_64 rng(0x5eed1234ULL);
    std::vector<std::int64_t> initial(N);
    for(auto& x: initial){
        x = (std::int64_t)(rng() % 2000001) - 1000000;
    }

    PartiallyPersistentArray<std::int64_t, 64, MAX_UPDATE> a(initial);
    std::vector<std::vector<std::int64_t>> expected(1, initial);

    for(int q = 0; q < MAX_UPDATE; q++){
        int k = (int)(rng() % N);
        std::int64_t x = (std::int64_t)(rng() % 2000001) - 1000000;

        auto next = expected.back();
        next[k] = x;
        int version = a.set(k, x);
        expected.push_back(std::move(next));

        assert(version == q + 1);
        assert(a.latest_version() == q + 1);
        assert(a.versions() == q + 2);

        for(int rep = 0; rep < 4; rep++){
            int v = (int)(rng() % expected.size());
            int i = (int)(rng() % N);
            assert(a.get(v, i) == expected[v][i]);
        }
        assert(a.get(k) == x);
    }

    std::array<Box, 3> boxes{Box(1), Box(2), Box(3)};
    PartiallyPersistentArray<Box, 3, 2> non_default(boxes);
    int version = non_default.set(1, Box(7));
    assert(non_default.get(0, 1).value == 2);
    assert(non_default.get(version, 1).value == 7);

    PartiallyPersistentArray<int, 1, 1> capacity(1, 0);
    capacity.set(0, 1);
    bool thrown = false;
    try{
        capacity.set(0, 2);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}
