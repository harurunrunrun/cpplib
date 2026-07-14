// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/evolutionary/compact_genetic_algorithm.hpp"
#include "../../src/approximate/evolutionary/differential_evolution.hpp"
#include "../../src/approximate/evolutionary/evolution_strategy.hpp"
#include "../../src/approximate/evolutionary/genetic_algorithm.hpp"
#include "../../src/approximate/evolutionary/hybrid_genetic_algorithm.hpp"
#include "../../src/approximate/evolutionary/particle_swarm_optimization.hpp"
#include "../../src/approximate/evolutionary/detail/box_constraints.hpp"
#include "../../src/approximate/evolutionary/detail/objective.hpp"
#include "../../src/approximate/evolutionary/detail/safe_count.hpp"
#include "../../src/approximate/evolutionary/evolution_result.hpp"

template<class Exception, class Callable>
bool expect(Callable callable){
    try{ callable(); }
    catch(const Exception&){ return true; }
    catch(...){ return false; }
    return false;
}

int main(){
    std::uint64_t seed;
    if(!(std::cin >> seed)) return 0;
    std::mt19937_64 random(seed);
    const std::size_t maximum = std::numeric_limits<std::size_t>::max();
    const auto select = [](const auto&, const auto&, auto&) -> int{ return 0; };
    const auto crossover = [](const int& lhs, const int&, auto&){ return lhs; };
    const auto mutate = [](int&, auto&){};
    const auto int_score = [](const int& value){ return value; };
    const std::vector<double> clamp_lower{0.0, -1.0, -1.0};
    const std::vector<double> clamp_upper{1.0, 1.0, 1.0};
    approximate::evolutionary::detail::validate_box(
        clamp_lower, clamp_upper, 3
    );
    std::vector<double> clamped{
        std::numeric_limits<double>::quiet_NaN(), -2.0, 2.0
    };
    approximate::evolutionary::detail::clamp_to_box(
        clamped, clamp_lower, clamp_upper
    );
    assert((clamped == std::vector<double>{0.0, -1.0, 1.0}));
    assert(approximate::evolutionary::detail::checked_add(2, 3, "overflow") == 5);
    assert(approximate::evolutionary::detail::checked_multiply_add(
        2, 3, 4, "overflow"
    ) == 10);
    auto doubled_score = [](const int& value){ return value * 2; };
    assert(approximate::evolutionary::detail::evaluate_objective(
        doubled_score, 3
    ) == 6);
    const approximate::evolutionary::EvolutionResult<int, int> invariant_result{
        3, 6, 4, 5
    };
    assert(invariant_result.score == doubled_score(invariant_result.individual));
    assert(invariant_result.iterations == 4 && invariant_result.evaluations == 5);

    std::size_t calls = 0;
    assert(expect<std::length_error>([&]{
        static_cast<void>(approximate::evolutionary::generational_genetic_algorithm(
            std::vector<int>{1, 2},
            [&calls](const int& value){ ++calls; return value; },
            select, crossover, mutate, maximum, random
        ));
    }));
    assert(calls == 0U);
    assert(expect<std::length_error>([&]{
        static_cast<void>(approximate::evolutionary::steady_state_genetic_algorithm(
            std::vector<int>{1}, int_score, select, crossover, mutate,
            maximum, random
        ));
    }));
    assert(expect<std::out_of_range>([&]{
        const auto negative = [](const auto&, const auto&, auto&){ return -1; };
        static_cast<void>(approximate::evolutionary::steady_state_genetic_algorithm(
            std::vector<int>{1}, int_score, negative, crossover, mutate, 1, random
        ));
    }));
    assert(expect<std::domain_error>([&]{
        static_cast<void>(approximate::evolutionary::generational_genetic_algorithm(
            std::vector<int>{1}, [](const int&){
                return std::numeric_limits<double>::quiet_NaN();
            }, select, crossover, mutate, 0, random
        ));
    }));

    const std::vector<double> lower{-1.0}, upper{1.0};
    const auto square = [](const std::vector<double>& point){
        return point.front() * point.front();
    };
    assert(expect<std::length_error>([&]{
        static_cast<void>(approximate::evolutionary::one_plus_one_es(
            std::vector<double>{0.0}, lower, upper, square,
            maximum, 1.0, random
        ));
    }));
    const auto no_generation = approximate::evolutionary::mu_plus_lambda_es(
        std::vector<std::vector<double>>{{0.0}}, lower, upper,
        square, 0, maximum, 1.0, random
    );
    assert(no_generation.evaluations == 1U);
    assert(expect<std::length_error>([&]{
        static_cast<void>(approximate::evolutionary::mu_plus_lambda_es(
            std::vector<std::vector<double>>{{0.0}}, lower, upper,
            square, maximum, 2, 1.0, random
        ));
    }));
    assert(expect<std::invalid_argument>([&]{
        static_cast<void>(approximate::evolutionary::one_plus_one_es(
            std::vector<double>{0.0},
            std::vector<double>{-std::numeric_limits<double>::infinity()},
            upper, square, 0, 1.0, random
        ));
    }));

    const std::vector<std::vector<double>> population{
        {-1.0}, {-0.5}, {0.5}, {1.0}
    };
    assert(expect<std::length_error>([&]{
        static_cast<void>(approximate::evolutionary::differential_evolution(
            population, lower, upper, square, maximum, 0.5, 0.5, random
        ));
    }));
    assert(expect<std::length_error>([&]{
        static_cast<void>(approximate::evolutionary::particle_swarm_optimization(
            std::vector<std::vector<double>>{{0.0}}, lower, upper, square,
            maximum, 0.0, 0.0, 0.0, random
        ));
    }));
    const double largest = std::numeric_limits<double>::max();
    const auto sign_score = [](const std::vector<double>& point){
        return point.front() < 0.0 ? 1.0 : 0.0;
    };
    const auto extreme = approximate::evolutionary::particle_swarm_optimization(
        std::vector<std::vector<double>>{{largest}, {-largest}},
        std::vector<double>{-largest}, std::vector<double>{largest},
        sign_score, 2, 0.0, 0.0, 1.0, random
    );
    assert(std::isfinite(extreme.individual.front()));

    const auto ones = [](const std::vector<std::uint8_t>& bits){
        std::size_t result = 0;
        for(const auto bit: bits) result += bit;
        return result;
    };
    const auto compact = approximate::evolutionary::compact_genetic_algorithm<double>(
        4, ones, 1, 10, random, std::greater<>{}, 0.0
    );
    for(const double probability: compact.probabilities){
        assert(0.1 <= probability && probability <= 0.9);
    }
    assert(expect<std::length_error>([&]{
        static_cast<void>(approximate::evolutionary::compact_genetic_algorithm<double>(
            1, ones, maximum, 10, random
        ));
    }));

    assert(expect<std::length_error>([&]{
        static_cast<void>(approximate::evolutionary::memetic_genetic_algorithm(
            std::vector<int>{1, 2}, int_score, select, crossover,
            mutate, mutate, maximum, random
        ));
    }));
    assert(expect<std::length_error>([&]{
        static_cast<void>(approximate::evolutionary::island_model_genetic_algorithm(
            std::vector<std::vector<int>>{{0}}, int_score, select,
            crossover, mutate, maximum, 2, 0, random
        ));
    }));
    const auto self = approximate::evolutionary::island_model_genetic_algorithm(
        std::vector<std::vector<int>>{{0, 10, 20}}, int_score, select,
        crossover, mutate, 2, 0, 3, random
    );
    assert(self.score == 0 && self.iterations == 0U && self.evaluations == 3U);
    std::cout << "OK\n";
}
