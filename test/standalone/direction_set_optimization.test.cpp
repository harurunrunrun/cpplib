// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/optimization/hooke_jeeves.hpp"
#include "../../src/approximate/optimization/powell.hpp"
#include "../../src/approximate/optimization/detail/direct_search_common.hpp"

namespace {

template<class Exception, class Function>
void expect_throw(Function&& function){
    bool thrown = false;
    try{ std::forward<Function>(function)(); }
    catch(const Exception&){ thrown = true; }
    assert(thrown);
}

struct CountedQuadratic{
    const std::vector<double>* target{};
    const std::vector<double>* weight{};
    std::size_t* calls{};

    double operator()(const std::vector<double>& point) const{
        ++*calls;
        double value = 0;
        for(std::size_t index = 0; index < point.size(); ++index){
            const double difference = point[index] - (*target)[index];
            value += (*weight)[index] * difference * difference;
        }
        return value;
    }
};

template<class Result>
void verify(
    const Result& result,
    const std::vector<double>& target,
    std::size_t calls
){
    assert(result.value < 1e-7);
    assert(result.evaluations == calls);
    assert(result.evaluations <= 100000);
    assert(result.gradient_evaluations == 0);
    for(std::size_t index = 0; index < target.size(); ++index){
        assert(std::abs(result.point[index] - target[index]) < 5e-4);
    }
}

void run_case(
    const std::vector<double>& target,
    const std::vector<double>& initial,
    const std::vector<double>& weight
){
    std::vector<double> lower(target.size());
    std::vector<double> upper(target.size());
    for(std::size_t index = 0; index < target.size(); ++index){
        lower[index] = target[index] - 20;
        upper[index] = target[index] + 20;
    }
    std::size_t calls = 0;
    auto objective = CountedQuadratic{&target, &weight, &calls};
    const auto powell_result = approximate::optimization::powell(
        initial, objective, 50, 100000,
        4.0, 0.5, 1e-5, 1e-7, lower, upper
    );
    verify(powell_result, target, calls);

    calls = 0;
    const auto hooke_result = approximate::optimization::hooke_jeeves(
        initial, objective, 500, 100000,
        4.0, 0.5, 1.0, 1e-6, lower, upper
    );
    verify(hooke_result, target, calls);

    std::size_t repeat_calls = 0;
    const auto repeat = approximate::optimization::powell(
        initial, CountedQuadratic{&target, &weight, &repeat_calls},
        50, 100000, 4.0, 0.5, 1e-5, 1e-7, lower, upper
    );
    assert(repeat.point == powell_result.point);
    assert(repeat.value == powell_result.value);
    assert(repeat.evaluations == powell_result.evaluations);
}

void boundary_tests(){
    using namespace approximate::optimization;
    const auto square = [](const std::vector<double>& point){
        return point.front() * point.front();
    };
    const auto no_iteration = powell(
        std::vector<double>{3}, square, 0, 1
    );
    assert(no_iteration.point == std::vector<double>({3}));
    assert(no_iteration.value == 9);
    assert(no_iteration.evaluations == 1);
    assert(no_iteration.iterations == 0);

    const auto no_budget = hooke_jeeves(
        std::vector<double>{3}, square, 10, 1
    );
    assert(no_budget.point == std::vector<double>({3}));
    assert(no_budget.evaluations == 1);
    assert(no_budget.iterations == 0);

    const auto two_minima = [](const std::vector<double>& point){
        const double distance = std::abs(point.front()) - 1;
        return distance * distance;
    };
    const auto powell_tie = powell(
        std::vector<double>{0}, two_minima, 1, 100,
        1.0, 0.5, 0.01, 0.0
    );
    const auto hooke_tie = hooke_jeeves(
        std::vector<double>{0}, two_minima, 1, 100,
        1.0, 0.5, 1.0, 0.0
    );
    assert(powell_tie.point.front() == 1);
    assert(hooke_tie.point.front() == 1);

    std::size_t fixed_calls = 0;
    const auto fixed_objective = [&fixed_calls](const auto&){
        ++fixed_calls;
        return 0.0;
    };
    const auto fixed_powell = powell(
        std::vector<double>{0}, fixed_objective, 10, 2,
        1.0, 0.5, 1e-4, 0.0,
        std::vector<double>{0}, std::vector<double>{0}
    );
    assert(fixed_powell.converged && fixed_powell.evaluations == 1);
    fixed_calls = 0;
    const auto fixed_hooke = hooke_jeeves(
        std::vector<double>{0}, fixed_objective, 100, 2,
        1.0, 0.5, 1.0, 1e-4,
        std::vector<double>{0}, std::vector<double>{0}
    );
    assert(fixed_hooke.converged && fixed_hooke.evaluations == 1);

    expect_throw<std::invalid_argument>([&]{
        (void)powell(std::vector<double>{}, square, 1, 10);
    });
    expect_throw<std::invalid_argument>([&]{
        (void)hooke_jeeves(std::vector<double>{0}, square, 1, 0);
    });
    expect_throw<std::invalid_argument>([&]{
        (void)powell(std::vector<double>{0}, square, 1, 10, 0.0);
    });
    expect_throw<std::invalid_argument>([&]{
        (void)powell(
            std::vector<double>{0}, square, 1, 10, 1.0, 1.0
        );
    });
    expect_throw<std::invalid_argument>([&]{
        (void)hooke_jeeves(
            std::vector<double>{0}, square, 1, 10,
            1.0, 0.5, 0.0
        );
    });
    expect_throw<std::domain_error>([&]{
        (void)powell(
            std::vector<double>{0},
            [](const auto&){ return std::numeric_limits<double>::infinity(); },
            1, 10
        );
    });
    expect_throw<std::overflow_error>([&]{
        (void)powell(
            std::vector<double>{std::numeric_limits<double>::max()},
            [](const auto&){ return 0.0; }, 1, 10,
            std::numeric_limits<double>::max(), 0.5, 1.0
        );
    });
    expect_throw<std::overflow_error>([&]{
        (void)hooke_jeeves(
            std::vector<double>{std::numeric_limits<double>::max()},
            [](const auto&){ return 0.0; }, 1, 10,
            std::numeric_limits<double>::max()
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
        std::cin >> dimension;
        std::vector<double> target(dimension);
        std::vector<double> initial(dimension);
        std::vector<double> weight(dimension);
        for(double& value: target) std::cin >> value;
        for(double& value: initial) std::cin >> value;
        for(double& value: weight) std::cin >> value;
        run_case(target, initial, weight);
    }
    boundary_tests();
    std::cout << "OK\n";
}
