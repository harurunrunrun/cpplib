// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>
#include "../../src/structure/wavelet_matrix/partially_persistent/partially_persistent_rectangle_sum.hpp"
#include "../../src/structure/wavelet_matrix/partially_persistent/partially_persistent_wavelet_matrix_2d_weighted.hpp"
#include "../../src/structure/wavelet_matrix/persistent/persistent_rectangle_sum.hpp"
#include "../../src/structure/wavelet_matrix/persistent/persistent_wavelet_matrix_2d_weighted.hpp"

namespace{

template<class Operation>
bool throws_runtime_error(Operation&& operation){
    try{
        operation();
    }catch(const std::runtime_error&){
        return true;
    }
    return false;
}

void self_test(){
    using Full = PersistentRectangleSum<
        int, unsigned, long long, 4, 1, 3>;
    Full full(
        std::vector<int>{0, 1},
        std::vector<unsigned>{1, 7},
        std::vector<long long>{4, 5}
    );
    const int version = full.set_weight(0, 1, 9);
    assert(version == 1);
    assert(full.weight(0, 1) == 5);
    assert(full.weight(1, 1) == 9);
    assert(throws_runtime_error([&]{ full.set_y(1, 0, 2); }));
    assert(full.versions() == 2);
    assert(full.weight(1, 1) == 9);

    using Partial = PartiallyPersistentRectangleSum<
        int, unsigned, long long, 4, 2, 3>;
    Partial partial(
        std::vector<int>{0, 1},
        std::vector<unsigned>{1, 7},
        std::vector<long long>{4, 5}
    );
    assert(throws_runtime_error([&]{ partial.set_y(0, 8); }));
    assert(partial.versions() == 1);
    assert(partial.y(0, 0) == 1);
    assert(partial.rectangle_sum(0, -1, 2, 0, 8) == 9);
}

template<class Matrix>
void print_point(const Matrix& matrix, int version, int position){
    std::cout << matrix.x(position) << ' '
              << matrix.y(version, position) << ' '
              << matrix.weight(version, position) << '\n';
}

void run_full(
    const std::vector<int>& xs,
    const std::vector<int>& ys,
    const std::vector<long long>& weights,
    int query_count
){
    PersistentRectangleSum<int, int, long long, 128, 512, 32>
        matrix(xs, ys, weights);
    while(query_count--){
        std::string operation;
        std::cin >> operation;
        if(operation == "SET"){
            int version, position, y;
            long long weight;
            std::cin >> version >> position >> y >> weight;
            std::cout << matrix.set(
                version, position, y, weight
            ) << '\n';
        }else if(operation == "SET_Y"){
            int version, position, y;
            std::cin >> version >> position >> y;
            std::cout << matrix.set_y(version, position, y) << '\n';
        }else if(operation == "SET_WEIGHT"){
            int version, position;
            long long weight;
            std::cin >> version >> position >> weight;
            std::cout << matrix.set_weight(
                version, position, weight
            ) << '\n';
        }else if(operation == "FORK"){
            int version;
            std::cin >> version;
            std::cout << matrix.fork(version) << '\n';
        }else if(operation == "POINT"){
            int version, position;
            std::cin >> version >> position;
            print_point(matrix, version, position);
        }else if(operation == "COUNT"){
            int version, xl, xr, yl, yr;
            std::cin >> version >> xl >> xr >> yl >> yr;
            std::cout << matrix.rectangle_count(
                version, xl, xr, yl, yr
            ) << '\n';
        }else if(operation == "SUM"){
            int version, xl, xr, yl, yr;
            std::cin >> version >> xl >> xr >> yl >> yr;
            std::cout << matrix.range_sum(
                version, xl, xr, yl, yr
            ) << '\n';
        }else if(operation == "VERSIONS"){
            std::cout << matrix.versions() << ' '
                      << matrix.latest_version() << '\n';
        }
    }
}

void run_partial(
    const std::vector<int>& xs,
    const std::vector<int>& ys,
    const std::vector<long long>& weights,
    int query_count
){
    PartiallyPersistentRectangleSum<
        int, int, long long, 128, 512, 32
    > matrix(xs, ys, weights);
    while(query_count--){
        std::string operation;
        std::cin >> operation;
        if(operation == "SET"){
            int position, y;
            long long weight;
            std::cin >> position >> y >> weight;
            std::cout << matrix.set(position, y, weight) << '\n';
        }else if(operation == "SET_Y"){
            int position, y;
            std::cin >> position >> y;
            std::cout << matrix.set_y(position, y) << '\n';
        }else if(operation == "SET_WEIGHT"){
            int position;
            long long weight;
            std::cin >> position >> weight;
            std::cout << matrix.set_weight(position, weight) << '\n';
        }else if(operation == "POINT"){
            int version, position;
            std::cin >> version >> position;
            print_point(matrix, version, position);
        }else if(operation == "COUNT"){
            int version, xl, xr, yl, yr;
            std::cin >> version >> xl >> xr >> yl >> yr;
            std::cout << matrix.range_freq(
                version, xl, xr, yl, yr
            ) << '\n';
        }else if(operation == "SUM"){
            int version, xl, xr, yl, yr;
            std::cin >> version >> xl >> xr >> yl >> yr;
            std::cout << matrix.rectangle_sum(
                version, xl, xr, yl, yr
            ) << '\n';
        }else if(operation == "VERSIONS"){
            std::cout << matrix.versions() << ' '
                      << matrix.latest_version() << '\n';
        }
    }
}

} // namespace

int main(){
    std::string mode;
    int n, query_count;
    if(!(std::cin >> mode >> n >> query_count)){
        self_test();
        return 0;
    }
    std::vector<int> xs(static_cast<std::size_t>(n));
    std::vector<int> ys(static_cast<std::size_t>(n));
    std::vector<long long> weights(static_cast<std::size_t>(n));
    for(int& x: xs) std::cin >> x;
    for(int& y: ys) std::cin >> y;
    for(long long& weight: weights) std::cin >> weight;
    if(mode == "FULL"){
        run_full(xs, ys, weights, query_count);
    }else{
        run_partial(xs, ys, weights, query_count);
    }
}
