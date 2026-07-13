// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/optimization/spsa.hpp"

namespace {

template<class Exception, class Function>
void expect_throw(Function&& function){
    bool thrown = false;
    try{ std::forward<Function>(function)(); }
    catch(const Exception&){ thrown = true; }
    assert(thrown);
}

struct CountingBit{
    using result_type = unsigned;
    std::size_t calls = 0;
    static constexpr result_type min(){ return 0; }
    static constexpr result_type max(){ return 1; }
    result_type operator()(){
        const result_type result = static_cast<result_type>(calls & 1U);
        ++calls;
        return result;
    }
};

struct CountedQuadratic{
    const std::vector<double>* target{};
    std::size_t* calls{};

    double operator()(const std::vector<double>& point) const{
        ++*calls;
        double value = 0;
        for(std::size_t index = 0; index < point.size(); ++index){
            const double difference = point[index] - (*target)[index];
            value += difference * difference;
        }
        return value;
    }
};

void run_case(
    const std::vector<double>& target,
    const std::vector<double>& initial,
    std::uint32_t seed
){
    std::vector<double> lower(target.size());
    std::vector<double> upper(target.size());
    for(std::size_t index = 0; index < target.size(); ++index){
        lower[index] = target[index] - 20;
        upper[index] = target[index] + 20;
    }
    std::size_t calls = 0;
    std::mt19937 random(seed);
    const auto result = approximate::optimization::spsa(
        initial, CountedQuadratic{&target, &calls}, random,
        4000, 12001, 0.12, 0.1, 0.602, 0.101, 10.0, 0.0,
        lower, upper
    );
    double initial_value = 0;
    for(std::size_t index = 0; index < target.size(); ++index){
        const double difference = initial[index] - target[index];
        initial_value += difference * difference;
        assert(std::abs(result.point[index] - target[index]) < 0.25);
    }
    assert(result.value < std::max(1e-8, initial_value * 0.01));
    assert(result.evaluations == calls);
    assert(result.evaluations <= 12001);
    assert(result.gradient_evaluations == result.iterations);
    assert(result.iterations <= 4000);

    std::size_t repeat_calls = 0;
    std::mt19937 repeat_random(seed);
    const auto repeat = approximate::optimization::spsa(
        initial, CountedQuadratic{&target, &repeat_calls}, repeat_random,
        4000, 12001, 0.12, 0.1, 0.602, 0.101, 10.0, 0.0,
        lower, upper
    );
    assert(repeat.point == result.point);
    assert(repeat.value == result.value);
    assert(repeat.evaluations == result.evaluations);
}

void boundary_tests(){
    using approximate::optimization::spsa;
    const auto objective = [](const std::vector<double>& point){
        const double difference = point.front() - 1;
        return difference * difference;
    };
    CountingBit no_iteration_random;
    const auto no_iteration = spsa(
        std::vector<double>{0}, objective, no_iteration_random,
        0, 1, 0.1, 0.1
    );
    assert(no_iteration.evaluations == 1);
    assert(no_iteration.gradient_evaluations == 0);
    assert(no_iteration_random.calls == 0);

    CountingBit no_budget_random;
    const auto no_budget = spsa(
        std::vector<double>{0}, objective, no_budget_random,
        10, 3, 0.1, 0.1
    );
    assert(no_budget.iterations == 0);
    assert(no_budget.evaluations == 1);
    assert(no_budget_random.calls == 0);

    std::size_t exact_calls = 0;
    const std::vector<double> target{1};
    CountingBit exact_random;
    const auto exact = spsa(
        std::vector<double>{0}, CountedQuadratic{&target, &exact_calls},
        exact_random, 5, 20, 0.5, 0.1,
        1.0, 1.0, 0.0, 0.0
    );
    assert(std::abs(exact.point.front() - 1) < 1e-14);
    assert(exact.value < 1e-28);
    assert(exact.iterations == 2);
    assert(exact.gradient_evaluations == 2);
    assert(exact.evaluations == 6 && exact_calls == 6);
    assert(exact_random.calls == exact.iterations);

    std::size_t fixed_calls = 0;
    CountingBit fixed_random;
    const auto fixed = spsa(
        std::vector<double>{0},
        [&fixed_calls](const auto&){ ++fixed_calls; return 0.0; },
        fixed_random, 10, 4, 0.1, 0.1,
        0.602, 0.101, 10.0, 0.0,
        std::vector<double>{0}, std::vector<double>{0}
    );
    assert(fixed.converged);
    assert(fixed.evaluations == 1 && fixed_calls == 1);
    assert(fixed.gradient_evaluations == 1);

    expect_throw<std::invalid_argument>([&]{
        CountingBit random;
        (void)spsa(std::vector<double>{}, objective, random, 1, 10, 0.1, 0.1);
    });
    expect_throw<std::invalid_argument>([&]{
        CountingBit random;
        (void)spsa(std::vector<double>{0}, objective, random, 1, 0, 0.1, 0.1);
    });
    expect_throw<std::invalid_argument>([&]{
        CountingBit random;
        (void)spsa(std::vector<double>{0}, objective, random, 1, 10, 0.0, 0.1);
    });
    expect_throw<std::invalid_argument>([&]{
        CountingBit random;
        (void)spsa(std::vector<double>{0}, objective, random, 1, 10, 0.1, -1.0);
    });
    expect_throw<std::domain_error>([&]{
        CountingBit random;
        (void)spsa(
            std::vector<double>{0},
            [](const auto&){ return std::numeric_limits<double>::infinity(); },
            random, 1, 10, 0.1, 0.1
        );
    });
    expect_throw<std::overflow_error>([&]{
        CountingBit random;
        (void)spsa(
            std::vector<double>{std::numeric_limits<double>::max()},
            [](const auto&){ return 0.0; }, random, 1, 10,
            0.1, std::numeric_limits<double>::max()
        );
    });
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t case_count = 0;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        std::size_t dimension = 0;
        std::uint32_t seed = 0;
        std::cin >> dimension >> seed;
        std::vector<double> target(dimension);
        std::vector<double> initial(dimension);
        for(double& value: target) std::cin >> value;
        for(double& value: initial) std::cin >> value;
        run_case(target, initial, seed);
    }
    boundary_tests();
    std::cout << "OK\n";
}
