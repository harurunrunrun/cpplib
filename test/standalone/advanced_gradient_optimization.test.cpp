// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/optimization/nonlinear_conjugate_gradient.hpp"
#include "../../src/approximate/optimization/quasi_newton.hpp"

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

struct CountedGradient{
    const std::vector<double>* target{};
    const std::vector<double>* weight{};
    std::size_t* calls{};

    std::vector<double> operator()(const std::vector<double>& point) const{
        ++*calls;
        std::vector<double> result(point.size());
        for(std::size_t index = 0; index < point.size(); ++index){
            result[index] = 2 * (*weight)[index]
                * (point[index] - (*target)[index]);
        }
        return result;
    }
};

template<class Result>
void verify_result(
    const Result& result,
    const std::vector<double>& target,
    std::size_t objective_calls,
    std::size_t gradient_calls
){
    assert(result.value < 1e-12);
    assert(result.evaluations == objective_calls);
    assert(result.gradient_evaluations == gradient_calls);
    assert(result.evaluations <= 5000);
    assert(result.iterations <= 500);
    for(std::size_t index = 0; index < target.size(); ++index){
        assert(std::abs(result.point[index] - target[index]) < 1e-5);
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
    std::size_t objective_calls = 0;
    std::size_t gradient_calls = 0;
    auto objective = CountedQuadratic{&target, &weight, &objective_calls};
    auto gradient = CountedGradient{&target, &weight, &gradient_calls};
    const auto bfgs = approximate::optimization::bfgs(
        initial, objective, gradient, 500, 5000,
        1.0, 1e-9, 1e-4, 0.5, 1e-12, lower, upper
    );
    verify_result(bfgs, target, objective_calls, gradient_calls);

    objective_calls = 0;
    gradient_calls = 0;
    const auto lbfgs = approximate::optimization::lbfgs(
        initial, objective, gradient, 500, 5000, 7,
        1.0, 1e-9, 1e-4, 0.5, 1e-12, lower, upper
    );
    verify_result(lbfgs, target, objective_calls, gradient_calls);

    objective_calls = 0;
    gradient_calls = 0;
    const auto conjugate = approximate::optimization::nonlinear_conjugate_gradient(
        initial, objective, gradient, 500, 5000,
        1.0, 1e-9, 1e-4, 0.5, 1e-12, lower, upper
    );
    verify_result(conjugate, target, objective_calls, gradient_calls);

    std::size_t repeat_objective_calls = 0;
    std::size_t repeat_gradient_calls = 0;
    const auto repeat = approximate::optimization::bfgs(
        initial,
        CountedQuadratic{&target, &weight, &repeat_objective_calls},
        CountedGradient{&target, &weight, &repeat_gradient_calls},
        500, 5000, 1.0, 1e-9, 1e-4, 0.5, 1e-12, lower, upper
    );
    assert(repeat.point == bfgs.point);
    assert(repeat.value == bfgs.value);
    assert(repeat.evaluations == bfgs.evaluations);
    assert(repeat.gradient_evaluations == bfgs.gradient_evaluations);
}

void boundary_tests(){
    using namespace approximate::optimization;
    const auto objective = [](const std::vector<double>& point){
        const double difference = point.front() - 1;
        return difference * difference;
    };
    const auto gradient = [](const std::vector<double>& point){
        return std::vector<double>{2 * (point.front() - 1)};
    };
    const auto no_iteration = bfgs(
        std::vector<double>{4}, objective, gradient, 0, 1
    );
    assert(no_iteration.point == std::vector<double>({4}));
    assert(no_iteration.value == 9);
    assert(no_iteration.evaluations == 1);
    assert(no_iteration.gradient_evaluations == 0);
    assert(!no_iteration.converged);

    const auto no_budget = lbfgs(
        std::vector<double>{4}, objective, gradient, 10, 1
    );
    assert(no_budget.point == std::vector<double>({4}));
    assert(no_budget.evaluations == 1);
    assert(no_budget.gradient_evaluations == 1);
    assert(no_budget.iterations == 0);

    for(const int method: {0, 1, 2}){
        std::size_t objective_calls = 0;
        std::size_t gradient_calls = 0;
        const std::vector<double> target{1};
        const std::vector<double> weight{1};
        auto counted_objective = CountedQuadratic{
            &target, &weight, &objective_calls
        };
        auto counted_gradient = CountedGradient{
            &target, &weight, &gradient_calls
        };
        OptimizationResult<double> result;
        if(method == 0){
            result = bfgs(
                std::vector<double>{0}, counted_objective, counted_gradient,
                5, 10, 0.5
            );
        }else if(method == 1){
            result = lbfgs(
                std::vector<double>{0}, counted_objective, counted_gradient,
                5, 10, 3, 0.5
            );
        }else{
            result = nonlinear_conjugate_gradient(
                std::vector<double>{0}, counted_objective, counted_gradient,
                5, 10, 0.5
            );
        }
        assert(result.point == std::vector<double>({1}));
        assert(result.value == 0);
        assert(result.iterations == 1);
        assert(result.evaluations == 2 && objective_calls == 2);
        assert(result.gradient_evaluations == 2 && gradient_calls == 2);
        assert(result.converged);
    }

    const auto fixed = bfgs(
        std::vector<double>{0}, objective, gradient, 10, 100,
        1.0, 0.0, 1e-4, 0.5, 1e-12,
        std::vector<double>{0}, std::vector<double>{0}
    );
    assert(fixed.converged);
    assert(fixed.evaluations == 1);

    const auto constrained_objective = [](const std::vector<double>& point){
        return 0.5 * point[0] * point[0] - point[0] * point[1]
            + point[1] * point[1] - 8 * point[0] - 3 * point[1];
    };
    const auto constrained_gradient = [](const std::vector<double>& point){
        return std::vector<double>{
            point[0] - point[1] - 8,
            -point[0] + 2 * point[1] - 3,
        };
    };
    const std::vector<double> constrained_initial{0, 0};
    const std::vector<double> constrained_lower{0, 0};
    const std::vector<double> constrained_upper{4, 4};
    const auto constrained_bfgs = bfgs(
        constrained_initial, constrained_objective, constrained_gradient,
        100, 1000, 1.0, 1e-10, 1e-4, 0.5, 1e-12,
        constrained_lower, constrained_upper
    );
    const auto constrained_lbfgs = lbfgs(
        constrained_initial, constrained_objective, constrained_gradient,
        100, 1000, 5, 1.0, 1e-10, 1e-4, 0.5, 1e-12,
        constrained_lower, constrained_upper
    );
    const auto constrained_cg = nonlinear_conjugate_gradient(
        constrained_initial, constrained_objective, constrained_gradient,
        100, 1000, 1.0, 1e-10, 1e-4, 0.5, 1e-12,
        constrained_lower, constrained_upper
    );
    for(const OptimizationResult<double>* result: {
        &constrained_bfgs, &constrained_lbfgs, &constrained_cg,
    }){
        assert(std::abs(result->point[0] - 4) < 1e-9);
        assert(std::abs(result->point[1] - 3.5) < 1e-7);
        assert(result->value < -36.249999999);
    }

    expect_throw<std::invalid_argument>([&]{
        (void)bfgs(std::vector<double>{}, objective, gradient, 1, 10);
    });
    expect_throw<std::invalid_argument>([&]{
        (void)lbfgs(std::vector<double>{0}, objective, gradient, 1, 0);
    });
    expect_throw<std::invalid_argument>([&]{
        (void)lbfgs(std::vector<double>{0}, objective, gradient, 1, 10, 0);
    });
    expect_throw<std::invalid_argument>([&]{
        (void)nonlinear_conjugate_gradient(
            std::vector<double>{0}, objective, gradient, 1, 10, 0.0
        );
    });
    expect_throw<std::invalid_argument>([&]{
        (void)bfgs(
            std::vector<double>{0}, objective, gradient, 1, 10,
            1.0, 1e-8, 1.0
        );
    });
    expect_throw<std::domain_error>([&]{
        (void)bfgs(
            std::vector<double>{0},
            [](const auto&){ return std::numeric_limits<double>::quiet_NaN(); },
            gradient, 1, 10
        );
    });
    expect_throw<std::invalid_argument>([&]{
        (void)bfgs(
            std::vector<double>{0}, objective,
            [](const auto&){ return std::vector<double>{}; }, 1, 10
        );
    });
    expect_throw<std::domain_error>([&]{
        (void)bfgs(
            std::vector<double>{0}, objective,
            [](const auto&){
                return std::vector<double>{
                    std::numeric_limits<double>::infinity()
                };
            }, 1, 10
        );
    });
    expect_throw<std::runtime_error>([&]{
        (void)bfgs(
            std::vector<double>{0}, [](const auto&){ return 0.0; },
            [](const auto&){ return std::vector<double>{1.0}; },
            10, 100, 1.0, 0.0, 1e-4, 0.5, 0.01
        );
    });
    expect_throw<std::overflow_error>([&]{
        (void)bfgs(
            std::vector<double>{std::numeric_limits<double>::max()},
            [](const auto&){ return 0.0; },
            [](const auto&){
                return std::vector<double>{std::numeric_limits<double>::max()};
            },
            1, 10, std::numeric_limits<double>::max(), 0.0,
            1e-4, 0.5, 1.0
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
