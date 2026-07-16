// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/permutation_group_order.hpp"

static std::vector<int> cycle_permutation(int degree){
    std::vector<int> result(static_cast<std::size_t>(degree));
    for(int point = 0; point < degree; ++point){
        result[static_cast<std::size_t>(point)] = (point + 1) % degree;
    }
    return result;
}

static void test_known_groups(){
    assert(permutation_group_order(0, {}).order == 1);
    assert(permutation_group_order(7, {}).order == 1);

    std::vector<int> transposition{1, 0, 2, 3, 4};
    auto symmetric = permutation_group_order(
        5, {cycle_permutation(5), transposition}
    );
    assert(symmetric.order == 120);
    BigInteger orbit_product = 1;
    for(const std::size_t size: symmetric.stabilizer_orbit_sizes){
        orbit_product *= static_cast<unsigned long long>(size);
    }
    assert(orbit_product == symmetric.order);

    std::vector<int> rotation = cycle_permutation(11);
    std::vector<int> reflection(11);
    for(int point = 0; point < 11; ++point){
        reflection[static_cast<std::size_t>(point)] = (11 - point) % 11;
    }
    assert(permutation_group_order(11, {rotation}).order == 11);
    assert(permutation_group_order(11, {rotation, reflection}).order == 22);

    std::vector<int> three_cycle{1, 2, 0, 3, 4};
    std::vector<int> other_three_cycle{0, 2, 3, 1, 4};
    assert(permutation_group_order(
        5, {three_cycle, other_three_cycle}
    ).order == 12);
}

static void test_large_symmetric(){
    constexpr int degree = 30;
    std::vector<int> transposition(static_cast<std::size_t>(degree));
    std::iota(transposition.begin(), transposition.end(), 0);
    std::swap(transposition[0], transposition[1]);
    const auto result = permutation_group_order(
        degree, {cycle_permutation(degree), transposition}
    );
    BigInteger factorial = 1;
    for(int value = 2; value <= degree; ++value) factorial *= value;
    assert(result.order == factorial);
}

static void test_invalid(){
    bool thrown = false;
    try{ (void)permutation_group_order(3, {{0, 1}}); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)permutation_group_order(3, {{0, 0, 2}}); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t case_count;
    if(!(std::cin >> case_count)){
        test_known_groups();
        test_large_symmetric();
        test_invalid();
        return 0;
    }
    while(case_count-- > 0){
        int degree;
        std::size_t generator_count;
        std::cin >> degree >> generator_count;
        std::vector<std::vector<int>> generators(
            generator_count,
            std::vector<int>(static_cast<std::size_t>(degree))
        );
        for(auto& generator: generators){
            for(int& image: generator) std::cin >> image;
        }
        std::cout << permutation_group_order(degree, generators).order << '\n';
    }
}

