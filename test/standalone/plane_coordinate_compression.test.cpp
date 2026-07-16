// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "../../src/algorithm/other/plane_coordinate_compression.hpp"

template<class Function>
bool throws_runtime_error(Function&& function){
    try{
        function();
    }catch(const std::runtime_error&){
        return true;
    }
    return false;
}

template<class T>
std::vector<T> naive_axis(
    const std::vector<T>& values,
    CoordinateNeighborPolicy neighbors
){
    std::vector<T> result;
    for(const T value: values){
        if(neighbors.predecessor && value != std::numeric_limits<T>::lowest()){
            result.push_back(static_cast<T>(value - T{1}));
        }
        result.push_back(value);
        if(neighbors.successor && value != std::numeric_limits<T>::max()){
            result.push_back(static_cast<T>(value + T{1}));
        }
    }
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

void self_test(){
    using Compression = PlaneCoordinateCompression<long long, int, 64, 64>;
    Compression empty;
    assert(empty.empty());
    assert(empty.size() == (std::array<std::size_t, 2>{0, 0}));

    Compression compression(
        {10, 0, 10},
        {5, -2},
        {true, true},
        {}
    );
    const std::vector<long long> expected_x{-1, 0, 1, 9, 10, 11};
    const std::vector<int> expected_y{-2, 5};
    assert(std::vector<long long>(compression.x_values().begin(), compression.x_values().end())
        == expected_x);
    assert(std::vector<int>(compression.y_values().begin(), compression.y_values().end())
        == expected_y);
    for(std::size_t index = 0; index < expected_x.size(); ++index){
        assert(compression.contains_x(expected_x[index]));
        assert(compression.x_index(expected_x[index]) == index);
        assert(compression.x_value(index) == expected_x[index]);
    }
    assert(!compression.contains_x(100));
    assert(throws_runtime_error([&]{ (void)compression.x_index(100); }));
    assert(throws_runtime_error([&]{ (void)compression.y_index(0); }));
    assert(throws_runtime_error([&]{ (void)compression.x_value(expected_x.size()); }));
    assert(throws_runtime_error([&]{ (void)compression.y_value(expected_y.size()); }));

    using UnsignedCompression = PlaneCoordinateCompression<
        unsigned long long,
        unsigned int,
        8,
        8
    >;
    UnsignedCompression extremes(
        {0, std::numeric_limits<unsigned long long>::max()},
        {0, std::numeric_limits<unsigned int>::max()},
        {true, true},
        {true, true}
    );
    assert((std::vector<unsigned long long>(
        extremes.x_values().begin(), extremes.x_values().end()
    ) == std::vector<unsigned long long>{
        0,
        1,
        std::numeric_limits<unsigned long long>::max() - 1,
        std::numeric_limits<unsigned long long>::max()
    }));
    assert((std::vector<unsigned int>(
        extremes.y_values().begin(), extremes.y_values().end()
    ) == std::vector<unsigned int>{
        0,
        1,
        std::numeric_limits<unsigned int>::max() - 1,
        std::numeric_limits<unsigned int>::max()
    }));

    PlaneCoordinateCompression<int, int, 2, 2> small({9}, {8});
    assert(throws_runtime_error([&]{ small.build({1}, {2}, {true, true}, {}); }));
    assert(small.x_size() == 1 && small.x_value(0) == 9);
    assert(small.y_size() == 1 && small.y_value(0) == 8);

    PlaneCoordinateCompression<int, int, 0, 0> zero_capacity;
    zero_capacity.build({}, {});
    assert(zero_capacity.empty());
    assert(zero_capacity.x_values().empty());
    assert(zero_capacity.y_values().empty());
    assert(!zero_capacity.contains_x(0));
    assert(!zero_capacity.contains_y(0));
    assert(throws_runtime_error([&]{ (void)zero_capacity.x_index(0); }));
    assert(throws_runtime_error([&]{ zero_capacity.build({0}, {}); }));

    std::mt19937_64 random(2026071601);
    for(int iteration = 0; iteration < 3000; ++iteration){
        const int nx = static_cast<int>(random() % 40);
        const int ny = static_cast<int>(random() % 40);
        std::vector<long long> xs(static_cast<std::size_t>(nx));
        std::vector<long long> ys(static_cast<std::size_t>(ny));
        for(long long& value: xs) value = static_cast<long long>(random() % 101) - 50;
        for(long long& value: ys) value = static_cast<long long>(random() % 101) - 50;
        if(iteration % 7 == 0 && !xs.empty()){
            xs.front() = std::numeric_limits<long long>::lowest();
        }
        if(iteration % 11 == 0 && !ys.empty()){
            ys.back() = std::numeric_limits<long long>::max();
        }
        const CoordinateNeighborPolicy x_neighbors{
            (random() & 1U) != 0,
            (random() & 1U) != 0
        };
        const CoordinateNeighborPolicy y_neighbors{
            (random() & 1U) != 0,
            (random() & 1U) != 0
        };
        PlaneCoordinateCompression<long long, long long, 128, 128> current(
            xs,
            ys,
            x_neighbors,
            y_neighbors
        );
        const auto want_x = naive_axis(xs, x_neighbors);
        const auto want_y = naive_axis(ys, y_neighbors);
        assert(std::vector<long long>(current.x_values().begin(), current.x_values().end())
            == want_x);
        assert(std::vector<long long>(current.y_values().begin(), current.y_values().end())
            == want_y);
        for(std::size_t index = 0; index < want_x.size(); ++index){
            assert(current.x_index(want_x[index]) == index);
            assert(current.x_value(index) == want_x[index]);
        }
        for(std::size_t index = 0; index < want_y.size(); ++index){
            assert(current.y_index(want_y[index]) == index);
            assert(current.y_value(index) == want_y[index]);
        }
    }
}

template<class T>
void print_line(std::span<const T> values){
    for(std::size_t index = 0; index < values.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << values[index];
    }
    std::cout << '\n';
}

int main(){
    int nx;
    int ny;
    int include_x_predecessor;
    int include_x_successor;
    int include_y_predecessor;
    int include_y_successor;
    if(!(std::cin >> nx >> ny >> include_x_predecessor >> include_x_successor
        >> include_y_predecessor >> include_y_successor)){
        self_test();
        return 0;
    }

    std::vector<long long> xs(static_cast<std::size_t>(nx));
    std::vector<long long> ys(static_cast<std::size_t>(ny));
    for(long long& value: xs) std::cin >> value;
    for(long long& value: ys) std::cin >> value;

    PlaneCoordinateCompression<long long, long long, 4096, 4096> compression(
        xs,
        ys,
        {include_x_predecessor != 0, include_x_successor != 0},
        {include_y_predecessor != 0, include_y_successor != 0}
    );
    std::cout << compression.x_size() << ' ' << compression.y_size() << '\n';
    print_line(compression.x_values());
    print_line(compression.y_values());
    for(std::size_t index = 0; index < xs.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << compression.x_index(xs[index]);
    }
    std::cout << '\n';
    for(std::size_t index = 0; index < ys.size(); ++index){
        if(index != 0) std::cout << ' ';
        std::cout << compression.y_index(ys[index]);
    }
    std::cout << '\n';
}
