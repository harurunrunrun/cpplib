// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/other/offline_query/multidimensional_prefix_sum.hpp"

template<class Function>
bool throws_runtime_error(Function&& function){
    try{
        function();
    }catch(const std::runtime_error&){
        return true;
    }
    return false;
}

using Coordinate3 = std::array<std::size_t, 3>;

std::size_t naive_index(const Coordinate3& shape, const Coordinate3& coordinate){
    return (coordinate[0] * shape[1] + coordinate[1]) * shape[2] + coordinate[2];
}

long long naive_range_sum(
    const Coordinate3& shape,
    const std::vector<long long>& values,
    const Coordinate3& begin,
    const Coordinate3& end
){
    long long result = 0;
    for(std::size_t x = begin[0]; x < end[0]; ++x){
        for(std::size_t y = begin[1]; y < end[1]; ++y){
            for(std::size_t z = begin[2]; z < end[2]; ++z){
                result += values[naive_index(shape, {x, y, z})];
            }
        }
    }
    return result;
}

void self_test(){
    const Coordinate3 shape{3, 4, 5};
    MultidimensionalPrefixSum<long long, 3, 256> prefix(shape);
    std::vector<long long> values(prefix.size(), 0);
    for(std::size_t x = 0; x < shape[0]; ++x){
        for(std::size_t y = 0; y < shape[1]; ++y){
            for(std::size_t z = 0; z < shape[2]; ++z){
                const Coordinate3 coordinate{x, y, z};
                const long long value = static_cast<long long>(100 * x + 10 * y + z) - 50;
                prefix.set(coordinate, value);
                prefix.add(coordinate, 3);
                values[naive_index(shape, coordinate)] = value + 3;
                assert(prefix.get(coordinate) == value + 3);
            }
        }
    }
    assert(!prefix.built());
    assert(prefix.shape() == shape);
    assert(prefix.size() == 60);
    assert(prefix.storage_size() == 120);
    assert(prefix.capacity() == 256);
    assert(throws_runtime_error([&]{ (void)prefix.prefix_sum({1, 1, 1}); }));
    prefix.build();
    assert(prefix.built());
    for(std::size_t bx = 0; bx <= shape[0]; ++bx){
        for(std::size_t by = 0; by <= shape[1]; ++by){
            for(std::size_t bz = 0; bz <= shape[2]; ++bz){
                const Coordinate3 begin{bx, by, bz};
                for(std::size_t ex = bx; ex <= shape[0]; ++ex){
                    for(std::size_t ey = by; ey <= shape[1]; ++ey){
                        for(std::size_t ez = bz; ez <= shape[2]; ++ez){
                            const Coordinate3 end{ex, ey, ez};
                            assert(prefix.range_sum(begin, end)
                                == naive_range_sum(shape, values, begin, end));
                        }
                    }
                }
            }
        }
    }
    for(std::size_t x = 0; x < shape[0]; ++x){
        for(std::size_t y = 0; y < shape[1]; ++y){
            for(std::size_t z = 0; z < shape[2]; ++z){
                const Coordinate3 coordinate{x, y, z};
                assert(prefix.get(coordinate) == values[naive_index(shape, coordinate)]);
            }
        }
    }
    assert(prefix.prefix_sum(shape) == naive_range_sum(shape, values, {0, 0, 0}, shape));
    assert(throws_runtime_error([&]{ prefix.set({0, 0, 0}, 0); }));
    assert(throws_runtime_error([&]{ prefix.add({0, 0, 0}, 1); }));
    assert(throws_runtime_error([&]{ prefix.build(); }));
    assert(throws_runtime_error([&]{ (void)prefix.get({3, 0, 0}); }));
    assert(throws_runtime_error([&]{ (void)prefix.prefix_sum({4, 0, 0}); }));
    assert(throws_runtime_error([&]{ (void)prefix.range_sum({2, 0, 0}, {1, 0, 0}); }));
    assert(throws_runtime_error([&]{ (void)prefix.range_sum({0, 2, 0}, {0, 1, 0}); }));

    MultidimensionalPrefixSum<int, 3, 32> empty({0, 5, 2});
    assert(empty.empty());
    assert(empty.size() == 0);
    empty.build();
    assert(empty.prefix_sum({0, 5, 2}) == 0);
    assert(empty.range_sum({0, 1, 1}, {0, 5, 2}) == 0);

    assert(throws_runtime_error([]{
        (void)MultidimensionalPrefixSum<int, 3, 20>({2, 2, 2});
    }));
    assert(throws_runtime_error([]{
        (void)MultidimensionalPrefixSum<int, 3, 64>({
            std::numeric_limits<std::size_t>::max(), 0, 0
        });
    }));

    MultidimensionalPrefixSum<int, 1, 8> one_dimension({4});
    for(std::size_t index = 0; index < 4; ++index){
        one_dimension.set({index}, static_cast<int>(index + 1));
    }
    one_dimension.build();
    assert(one_dimension.prefix_sum({4}) == 10);
    assert(one_dimension.range_sum({1}, {4}) == 9);

    MultidimensionalPrefixSum<int, 4, 81> four_dimensions({2, 2, 2, 2});
    for(std::size_t mask = 0; mask < 16; ++mask){
        const std::array<std::size_t, 4> coordinate{
            mask & 1U,
            (mask >> 1) & 1U,
            (mask >> 2) & 1U,
            (mask >> 3) & 1U
        };
        four_dimensions.set(coordinate, static_cast<int>(mask) - 7);
    }
    four_dimensions.build();
    assert(four_dimensions.prefix_sum({2, 2, 2, 2}) == 8);
    assert(four_dimensions.prefix_sum({1, 2, 1, 2}) == -8);
    assert(four_dimensions.range_sum({1, 0, 1, 0}, {2, 2, 2, 2}) == 12);

    std::mt19937_64 random(2026071602);
    for(int iteration = 0; iteration < 1000; ++iteration){
        const Coordinate3 current_shape{
            1 + random() % 7,
            1 + random() % 7,
            1 + random() % 7
        };
        MultidimensionalPrefixSum<long long, 3, 512> current(current_shape);
        std::vector<long long> current_values(current.size(), 0);
        for(std::size_t x = 0; x < current_shape[0]; ++x){
            for(std::size_t y = 0; y < current_shape[1]; ++y){
                for(std::size_t z = 0; z < current_shape[2]; ++z){
                    const Coordinate3 coordinate{x, y, z};
                    const long long value = static_cast<long long>(random() % 2001) - 1000;
                    current.set(coordinate, value);
                    current_values[naive_index(current_shape, coordinate)] = value;
                }
            }
        }
        current.build();
        for(int query = 0; query < 50; ++query){
            Coordinate3 begin{};
            Coordinate3 end{};
            for(std::size_t axis = 0; axis < 3; ++axis){
                begin[axis] = random() % (current_shape[axis] + 1);
                end[axis] = begin[axis] + random() % (current_shape[axis] - begin[axis] + 1);
            }
            assert(current.range_sum(begin, end)
                == naive_range_sum(current_shape, current_values, begin, end));
        }
    }
}

int main(){
    Coordinate3 shape{};
    if(!(std::cin >> shape[0] >> shape[1] >> shape[2])){
        self_test();
        return 0;
    }

    MultidimensionalPrefixSum<long long, 3, 4096> prefix(shape);
    int update_count;
    std::cin >> update_count;
    for(int update = 0; update < update_count; ++update){
        char type;
        Coordinate3 coordinate{};
        long long value;
        std::cin >> type >> coordinate[0] >> coordinate[1] >> coordinate[2] >> value;
        if(type == 'S') prefix.set(coordinate, value);
        else prefix.add(coordinate, value);
    }
    prefix.build();

    int query_count;
    std::cin >> query_count;
    for(int query = 0; query < query_count; ++query){
        char type;
        std::cin >> type;
        if(type == 'G'){
            Coordinate3 coordinate{};
            std::cin >> coordinate[0] >> coordinate[1] >> coordinate[2];
            std::cout << prefix.get(coordinate) << '\n';
        }else if(type == 'P'){
            Coordinate3 end{};
            std::cin >> end[0] >> end[1] >> end[2];
            std::cout << prefix.prefix_sum(end) << '\n';
        }else{
            Coordinate3 begin{};
            Coordinate3 end{};
            std::cin >> begin[0] >> begin[1] >> begin[2]
                >> end[0] >> end[1] >> end[2];
            std::cout << prefix.range_sum(begin, end) << '\n';
        }
    }
}
