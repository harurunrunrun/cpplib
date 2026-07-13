// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "../../src/approximate/optimization/adagrad.hpp"
#include "../../src/approximate/optimization/adam.hpp"
#include "../../src/approximate/optimization/adamw.hpp"
#include "../../src/approximate/optimization/coordinate_descent.hpp"
#include "../../src/approximate/optimization/gradient_descent.hpp"
#include "../../src/approximate/optimization/momentum.hpp"
#include "../../src/approximate/optimization/nesterov.hpp"
#include "../../src/approximate/optimization/rmsprop.hpp"

namespace {

template<class Exception, class Function>
void expect_throw(Function&& function){
    bool thrown = false;
    try{
        std::forward<Function>(function)();
    }catch(const Exception&){
        thrown = true;
    }
    assert(thrown);
}

double squared_distance(
    const std::vector<double>& point,
    const std::vector<double>& target
){
    double value = 0.0;
    for(std::size_t index = 0; index < point.size(); ++index){
        const double difference = point[index] - target[index];
        value += difference * difference;
    }
    return value;
}

bool inside(
    const std::vector<double>& point,
    const std::vector<double>& lower,
    const std::vector<double>& upper
){
    for(std::size_t index = 0; index < point.size(); ++index){
        if(point[index] < lower[index] || upper[index] < point[index]) return false;
    }
    return true;
}

template<class Result>
void check_result(
    const Result& result,
    double initial_value,
    const std::vector<double>& lower,
    const std::vector<double>& upper
){
    assert(std::isfinite(result.value));
    assert(result.value < initial_value || initial_value == 0.0);
    assert(result.evaluations == result.iterations + 1);
    assert(result.gradient_evaluations == result.iterations
           || result.gradient_evaluations == result.iterations + 1);
    assert(inside(result.point, lower, upper));
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t case_count;
    if(!(std::cin >> case_count)) return 0;
    while(case_count-- > 0){
        std::size_t dimension;
        std::cin >> dimension;
        std::vector<double> target(dimension);
        std::vector<double> initial(dimension);
        for(auto& value: target) std::cin >> value;
        for(auto& value: initial) std::cin >> value;
        std::vector<double> lower(dimension);
        std::vector<double> upper(dimension);
        for(std::size_t index = 0; index < dimension; ++index){
            lower[index] = target[index] - 20.0;
            upper[index] = target[index] + 20.0;
        }
        const auto objective = [&target](const std::vector<double>& point){
            return squared_distance(point, target);
        };
        const auto gradient = [&target](const std::vector<double>& point){
            std::vector<double> result(point.size());
            for(std::size_t index = 0; index < point.size(); ++index){
                result[index] = 2.0 * (point[index] - target[index]);
            }
            return result;
        };
        const double initial_value = objective(initial);

        const auto gd = approximate::optimization::gradient_descent(
            initial, objective, gradient, 200, 0.2, 1e-10, lower, upper
        );
        check_result(gd, initial_value, lower, upper);
        assert(gd.value < 1e-14);

        const auto momentum = approximate::optimization::momentum_descent(
            initial, objective, gradient, 500, 0.08, 0.8,
            1e-9, lower, upper
        );
        check_result(momentum, initial_value, lower, upper);
        assert(momentum.value < 1e-12);

        const auto nesterov =
            approximate::optimization::nesterov_accelerated_gradient(
                initial, objective, gradient, 500, 0.08, 0.8,
                1e-9, lower, upper
            );
        check_result(nesterov, initial_value, lower, upper);
        assert(nesterov.value < 1e-12);

        const auto adagrad = approximate::optimization::adagrad(
            initial, objective, gradient, 800, 0.8, 1e-8,
            1e-8, lower, upper
        );
        check_result(adagrad, initial_value, lower, upper);

        const auto rmsprop = approximate::optimization::rmsprop(
            initial, objective, gradient, 1000, 0.03, 0.9, 1e-8,
            0.0, lower, upper
        );
        check_result(rmsprop, initial_value, lower, upper);
        assert(rmsprop.value < initial_value);

        const auto adam = approximate::optimization::adam(
            initial, objective, gradient, 800, 0.08, 0.9, 0.999,
            1e-8, 1e-8, lower, upper
        );
        check_result(adam, initial_value, lower, upper);
        assert(adam.value < 1e-10);

        const auto adamw = approximate::optimization::adamw(
            initial, objective, gradient, 800, 0.08, 0.0, 0.9, 0.999,
            1e-8, 1e-8, lower, upper
        );
        check_result(adamw, initial_value, lower, upper);
        assert(adamw.value < 1e-10);

        const auto coordinate_gradient = [&target](
            const std::vector<double>& point, std::size_t coordinate
        ){
            return 2.0 * (point[coordinate] - target[coordinate]);
        };
        const auto coordinate = approximate::optimization::coordinate_descent(
            initial, objective, coordinate_gradient, 200, 0.2,
            1e-10, lower, upper
        );
        assert(coordinate.value < 1e-14);
        assert(coordinate.value < initial_value);
        assert(coordinate.gradient_evaluations >= coordinate.iterations * dimension);
        assert(inside(coordinate.point, lower, upper));
    }

    const auto objective = [](const std::vector<double>& point){
        return point.front() * point.front();
    };
    const auto gradient = [](const std::vector<double>& point){
        return std::vector<double>{2.0 * point.front()};
    };
    const auto no_iteration = approximate::optimization::gradient_descent(
        std::vector<double>{3.0}, objective, gradient, 0, 0.1
    );
    assert(no_iteration.point == std::vector<double>{3.0});
    assert(no_iteration.value == 9.0);
    assert(no_iteration.evaluations == 1);
    assert(no_iteration.gradient_evaluations == 0);
    assert(!no_iteration.converged);

    const auto fixed = approximate::optimization::gradient_descent(
        std::vector<double>{0.0}, objective, gradient, 10, 0.1, 0.0,
        std::vector<double>{0.0}, std::vector<double>{0.0}
    );
    assert(fixed.converged);
    assert(fixed.iterations == 0);
    assert(fixed.gradient_evaluations == 1);

    expect_throw<std::invalid_argument>([&]{
        static_cast<void>(approximate::optimization::gradient_descent(
            std::vector<double>{}, objective, gradient, 1, 0.1
        ));
    });
    expect_throw<std::invalid_argument>([&]{
        static_cast<void>(approximate::optimization::gradient_descent(
            std::vector<double>{std::numeric_limits<double>::quiet_NaN()},
            objective, gradient, 1, 0.1
        ));
    });
    expect_throw<std::invalid_argument>([&]{
        static_cast<void>(approximate::optimization::gradient_descent(
            std::vector<double>{1.0}, objective, gradient, 1, 0.1, 0.0,
            std::vector<double>{0.0, 0.0}, std::vector<double>{2.0, 2.0}
        ));
    });
    expect_throw<std::domain_error>([&]{
        static_cast<void>(approximate::optimization::gradient_descent(
            std::vector<double>{1.0},
            [](const auto&){ return std::numeric_limits<double>::infinity(); },
            gradient, 1, 0.1
        ));
    });
    expect_throw<std::invalid_argument>([&]{
        static_cast<void>(approximate::optimization::gradient_descent(
            std::vector<double>{1.0}, objective,
            [](const auto&){ return std::vector<double>{}; }, 1, 0.1
        ));
    });
    expect_throw<std::domain_error>([&]{
        static_cast<void>(approximate::optimization::gradient_descent(
            std::vector<double>{1.0}, objective,
            [](const auto&){
                return std::vector<double>{
                    std::numeric_limits<double>::quiet_NaN()
                };
            }, 1, 0.1
        ));
    });
    expect_throw<std::overflow_error>([&]{
        static_cast<void>(approximate::optimization::gradient_descent(
            std::vector<double>{1.0}, objective,
            [](const auto&){
                return std::vector<double>{std::numeric_limits<double>::max()};
            }, 1, std::numeric_limits<double>::max()
        ));
    });
    expect_throw<std::invalid_argument>([&]{
        static_cast<void>(approximate::optimization::momentum_descent(
            std::vector<double>{1.0}, objective, gradient, 1, 0.1, 1.0
        ));
    });
    expect_throw<std::invalid_argument>([&]{
        static_cast<void>(approximate::optimization::coordinate_descent(
            std::vector<double>{1.0}, objective,
            [](const auto&, std::size_t){ return 1.0; }, 1, 0.0
        ));
    });
    std::cout << "OK\n";
}
