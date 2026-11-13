// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <random>
#include <utility>
#include <vector>

#include "../../src/algorithm/other/offline_query/range_count/offline_dynamic_range_count_less_equal.hpp"

template<class T>
void random_test(unsigned seed){
    std::mt19937 rng(seed);
    for(int iteration = 0; iteration < 100; ++iteration){
        const int n = static_cast<int>(rng() % 8) + 1;
        std::vector<T> values(static_cast<std::size_t>(n));
        for(T& value: values) value = static_cast<T>(rng() % 6);
        OfflineDynamicRangeCountLessEqual<T> solver(values);
        std::vector<int> expected;
        for(int step = 0; step < 20; ++step){
            if(rng() % 2 == 0){
                const int position = static_cast<int>(rng() % static_cast<unsigned>(n));
                const T value = static_cast<T>(rng() % 6);
                solver.add_set(position, value);
                values[static_cast<std::size_t>(position)] = value;
            }else{
                int left = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
                int right = static_cast<int>(rng() % static_cast<unsigned>(n + 1));
                if(left > right) std::swap(left, right);
                const T upper = static_cast<T>(rng() % 6);
                solver.add_count_less_equal(left, right, upper);
                int count = 0;
                for(int index = left; index < right; ++index){
                    if(values[static_cast<std::size_t>(index)] <= upper) ++count;
                }
                expected.push_back(count);
            }
        }
        assert(solver.solve() == expected);
    }
}

int main(){
    OfflineDynamicRangeCountLessEqual<unsigned> solver({1u, 2u, 3u});
    solver.add_count_less_equal(0, 3, 5u);
    solver.add_count_less_equal(0, 0, 5u);
    solver.add_count_less_equal(1, 3, 2u);
    assert((solver.solve() == std::vector<int>{3, 0, 1}));
    random_test<int>(1);
    random_test<unsigned>(2);
    random_test<unsigned long long>(3);
}
