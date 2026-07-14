// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/other/partially_persistent_storage.hpp"
#include "../../src/structure/array/partially_persistent_array.hpp"

struct Box{
    int value;

    Box() = delete;
    explicit Box(int value): value(value){}
};

int main(){
    int input_n, input_q;
    if(std::cin >> input_n >> input_q){
        std::vector<std::int64_t> initial(static_cast<std::size_t>(input_n));
        for(auto& value: initial) std::cin >> value;
        PartiallyPersistentArray<std::int64_t, 128, 2048> array(initial);
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "SET"){
                int k;
                std::int64_t value;
                std::cin >> k >> value;
                std::cout << array.set(k, value) << '\n';
            }else if(type == "GET"){
                int version, k;
                std::cin >> version >> k;
                std::cout << array.get(version, k) << '\n';
            }else if(type == "CURRENT"){
                int k;
                std::cin >> k;
                std::cout << array.get(k) << '\n';
            }else if(type == "VERSIONS"){
                std::cout << array.versions() << '\n';
            }else if(type == "LATEST"){
                std::cout << array.latest_version() << '\n';
            }
        }
        return 0;
    }

    constexpr int N = 37;
    constexpr int MAX_UPDATE = 5000;

    std::mt19937_64 rng(0x5eed1234ULL);
    std::vector<std::int64_t> initial(N);
    for(auto& x: initial){
        x = (std::int64_t)(rng() % 2000001) - 1000000;
    }

    PartiallyPersistentArray<std::int64_t, 64, MAX_UPDATE> a(initial);
    std::vector<std::vector<std::int64_t>> expected(1, initial);
    assert(a.size() == N);
    assert(a.versions() == 1);
    assert(a.latest_version() == 0);

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
    assert(a.size() == N);
    assert(a.versions() == static_cast<int>(expected.size()));
    assert(a.latest_version() + 1 == a.versions());

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
