// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/randomized/metropolis_hastings.hpp"
#include "../../src/approximate/randomized/monte_carlo_integration.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t sample_count;
    std::uint64_t seed;
    if(!(std::cin >> sample_count >> seed)) return 0;

    std::mt19937_64 first_random(seed);
    std::mt19937_64 second_random(seed);
    const auto integral = approximate::randomized::monte_carlo_integrate(
        [](double value){ return value * value; }, 0.0, 1.0, sample_count, first_random
    );
    const auto repeated = approximate::randomized::monte_carlo_integrate(
        [](double value){ return value * value; }, 0.0, 1.0, sample_count, second_random
    );
    assert(integral.estimate == repeated.estimate);
    assert(integral.standard_error == repeated.standard_error);
    assert(integral.samples == sample_count);
    assert(std::abs(integral.estimate - 1.0 / 3.0) < 0.015);
    assert(integral.standard_error > 0.0 && integral.standard_error < 0.01);

    std::mt19937_64 box_random(seed + 1U);
    const std::array<double, 2> lower{-1.0, 2.0};
    const std::array<double, 2> upper{3.0, 5.0};
    const auto box = approximate::randomized::monte_carlo_integrate_hyperrectangle(
        lower, upper, sample_count,
        [](const std::array<double, 2>& point){ return point[0] + point[1]; }, box_random
    );
    assert(std::abs(box.estimate - 54.0) < 1.0);
    assert(box.standard_error > 0.0);

    std::mt19937_64 zero_random(seed + 2U);
    const auto zero = approximate::randomized::monte_carlo_integrate_hyperrectangle(
        std::vector<double>{0.0, 1.0}, std::vector<double>{0.0, 4.0}, 10,
        [](const std::vector<double>& point){ return point[0] + point[1]; }, zero_random
    );
    assert(zero.estimate == 0.0);
    assert(zero.standard_error == 0.0);

    std::mt19937_64 empty_random(seed + 30U);
    const auto empty_box = approximate::randomized::monte_carlo_integrate_hyperrectangle(
        std::vector<double>{}, std::vector<double>{}, 5,
        [](const std::vector<double>& point){ return point.empty() ? 2.0 : 0.0; }, empty_random
    );
    assert(empty_box.estimate == 2.0);
    assert(empty_box.standard_error == 0.0);

    const auto log_density = [](double value){ return -0.5 * value * value; };
    const auto proposal = [](double current, std::mt19937_64& random){
        std::uniform_real_distribution<double> step(-1.5, 1.5);
        return current + step(random);
    };
    std::mt19937_64 chain_random(seed + 3U);
    std::mt19937_64 chain_repeated_random(seed + 3U);
    const auto chain = approximate::randomized::metropolis_hastings_symmetric(
        0.0, 20000, 2500, 2, log_density, proposal, chain_random
    );
    const auto chain_repeated = approximate::randomized::metropolis_hastings_symmetric(
        0.0, 20000, 2500, 2, log_density, proposal, chain_repeated_random
    );
    assert(chain.samples == chain_repeated.samples);
    assert(chain.accepted == chain_repeated.accepted);
    assert(chain.proposals == 42500);
    assert(chain.acceptance_rate() > 0.3 && chain.acceptance_rate() < 0.9);
    double mean = 0.0;
    double second_moment = 0.0;
    for(double value: chain.samples){
        mean += value;
        second_moment += value * value;
    }
    mean /= static_cast<double>(chain.samples.size());
    second_moment /= static_cast<double>(chain.samples.size());
    assert(std::abs(mean) < 0.15);
    assert(second_moment > 0.75 && second_moment < 1.25);

    std::mt19937_64 stationary_random(seed + 4U);
    const auto stationary = approximate::randomized::metropolis_hastings(
        2, 4, 3, 2, [](int){ return 0.0; },
        [](int value, std::mt19937_64&){ return value; },
        [](int, int){ return 0.0; }, stationary_random
    );
    assert(stationary.samples == std::vector<int>({2, 2, 2, 2}));
    assert(stationary.accepted == 11);

    bool nonfinite_thrown = false;
    try{
        std::mt19937_64 random(1);
        static_cast<void>(approximate::randomized::monte_carlo_integrate(
            [](double){ return std::numeric_limits<double>::infinity(); },
            0.0, 1.0, 2, random
        ));
    }catch(const std::domain_error&){
        nonfinite_thrown = true;
    }
    assert(nonfinite_thrown);

    bool thinning_thrown = false;
    try{
        std::mt19937_64 random(1);
        static_cast<void>(approximate::randomized::metropolis_hastings_symmetric(
            0, 1, 0, 0, [](int){ return 0.0; },
            [](int value, std::mt19937_64&){ return value; }, random
        ));
    }catch(const std::invalid_argument&){
        thinning_thrown = true;
    }
    assert(thinning_thrown);

    bool dimensions_thrown = false;
    try{
        std::mt19937_64 random(1);
        static_cast<void>(approximate::randomized::monte_carlo_integrate_hyperrectangle(
            std::vector<double>{0.0}, std::vector<double>{1.0, 2.0}, 2,
            [](const std::vector<double>&){ return 0.0; }, random
        ));
    }catch(const std::invalid_argument&){
        dimensions_thrown = true;
    }
    assert(dimensions_thrown);

    bool initial_density_thrown = false;
    try{
        std::mt19937_64 random(1);
        static_cast<void>(approximate::randomized::metropolis_hastings_symmetric(
            0, 1, 0, 1, [](int){ return std::numeric_limits<double>::quiet_NaN(); },
            [](int value, std::mt19937_64&){ return value; }, random
        ));
    }catch(const std::domain_error&){
        initial_density_thrown = true;
    }
    assert(initial_density_thrown);

    bool steps_overflow_thrown = false;
    try{
        std::mt19937_64 random(1);
        static_cast<void>(approximate::randomized::metropolis_hastings_symmetric(
            0, std::numeric_limits<std::size_t>::max(), 1, 2,
            [](int){ return 0.0; },
            [](int value, std::mt19937_64&){ return value; }, random
        ));
    }catch(const std::overflow_error&){
        steps_overflow_thrown = true;
    }
    assert(steps_overflow_thrown);

    std::cout << "OK\n";
}
