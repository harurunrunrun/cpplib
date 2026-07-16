// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/fft/bivariate_formal_power_series.hpp"
#include "../../src/structure/modint/modint.hpp"

using Mint = Modint998244353;
using Series = BivariateFormalPowerSeries<Mint, 4, 4>;

static Series random_series(std::mt19937_64& rng){
    Series result;
    for(std::size_t x = 0; x <= Series::x_degree; ++x){
        for(std::size_t y = 0; y <= Series::y_degree; ++y){
            result(x, y) = Mint(static_cast<long long>(rng() % 100000));
        }
    }
    return result;
}

static void test_random_identities(){
    std::mt19937_64 rng(2026071619);
    const Series one = Series::constant(Mint(1));
    for(int iteration = 0; iteration < 100; ++iteration){
        Series invertible = random_series(rng);
        if(invertible(0, 0) == Mint(0)) invertible(0, 0) = Mint(1);
        assert(invertible * invertible.inverse() == one);
        assert(invertible.pow(0) == one);
        assert(invertible.pow(3) == invertible * invertible * invertible);
        assert(invertible.pow(-1) == invertible.inverse());

        Series zero_constant = random_series(rng);
        zero_constant(0, 0) = Mint(0);
        const Series exponential = zero_constant.exponential();
        assert(exponential.constant_term() == Mint(1));
        assert(exponential.logarithm() == zero_constant);

        Series root = random_series(rng);
        if(root(0, 0) == Mint(0)) root(0, 0) = Mint(7);
        const Series square = root * root;
        assert(square.square_root(root.constant_term()) == root);
    }
}

static void test_edges_and_errors(){
    using Narrow = BivariateFormalPowerSeries<Mint, 0, 0>;
    assert(Narrow::constant(Mint(7)).inverse().constant_term() == Mint(7).inv());
    assert(Narrow::constant(Mint(0)).exponential() == Narrow::constant(Mint(1)));
    assert(Narrow::constant(Mint(1)).logarithm() == Narrow{});

    bool thrown = false;
    try{ (void)Series{}.inverse(); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)Series::constant(Mint(2)).logarithm(); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)Series::constant(Mint(1)).exponential(); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)Series::constant(Mint(4)).square_root(Mint(3)); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ (void)Series{}(5, 0); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
}

static void test_larger(){
    using Large = BivariateFormalPowerSeries<Mint, 8, 8>;
    Large value;
    value(0, 0) = Mint(1);
    for(std::size_t x = 0; x <= 8; ++x){
        for(std::size_t y = 0; y <= 8; ++y){
            if(x + y != 0) value(x, y) = Mint((x + 3) * (y + 5));
        }
    }
    assert(value * value.inverse() == Large::constant(Mint(1)));
    assert(value.logarithm().exponential() == value);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t query_count;
    if(!(std::cin >> query_count)){
        test_random_identities();
        test_edges_and_errors();
        test_larger();
        return 0;
    }
    using InputSeries = BivariateFormalPowerSeries<Mint, 5, 5>;
    while(query_count-- > 0){
        std::string operation;
        std::cin >> operation;
        long long parameter = 0;
        if(operation == "POW" || operation == "SQRT") std::cin >> parameter;
        InputSeries value;
        for(std::size_t x = 0; x <= 5; ++x){
            for(std::size_t y = 0; y <= 5; ++y){
                std::cin >> value(x, y);
            }
        }
        InputSeries result;
        if(operation == "INV") result = value.inverse();
        else if(operation == "LOG") result = value.logarithm();
        else if(operation == "EXP") result = value.exponential();
        else if(operation == "POW") result = value.pow(parameter);
        else if(operation == "SQRT") result = value.square_root(Mint(parameter));
        else throw std::runtime_error("unknown bivariate FPS operation");
        for(std::size_t index = 0; index < result.data().size(); ++index){
            if(index != 0) std::cout << ' ';
            std::cout << result.data()[index];
        }
        std::cout << '\n';
    }
}

