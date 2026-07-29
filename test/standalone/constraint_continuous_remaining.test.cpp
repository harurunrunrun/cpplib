// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
#include <span>
#include <vector>

#include "../../src/approximate/constraint/advanced_sat_local_search.hpp"
#include "../../src/approximate/constraint/weighted_csp_search.hpp"
#include "../../src/approximate/optimization/global_optimization.hpp"
#include "../../src/approximate/optimization/second_order_and_pattern.hpp"
#include "../../src/approximate/optimization/stochastic_descent.hpp"

namespace {

void sat_and_csp_tests(std::mt19937& random) {
    const approximate::constraint::CnfFormula formula(
        3, {{1}, {2}, {3}, {-1, 2}, {-2, 3}}
    );
    const auto novelty = approximate::constraint::novelty(
        formula, random, 100, 10
    );
    const auto novelty_plus = approximate::constraint::novelty_plus(
        formula, random, 100, 10
    );
    const auto random_walk = approximate::constraint::random_walk_sat(
        formula, random, 100, 10
    );
    const auto breakout = approximate::constraint::breakout_method(
        formula, random, 100, 10
    );
    const auto paws = approximate::constraint::paws(
        formula, random, 100, 10
    );
    const auto saps = approximate::constraint::saps(
        formula, random, 100, 10
    );
    const auto clause_weighting =
        approximate::constraint::clause_weighting_sat(
            formula, random, 100, 10
        );
    for(const auto* result : {
        &novelty, &novelty_plus, &random_walk, &breakout,
        &paws, &saps, &clause_weighting
    }){
        approximate::constraint::validate_sat_search_result(
            formula, *result
        );
        assert(result->found);
    }

    const std::vector<std::size_t> domains{3, 3, 3};
    const std::vector<std::vector<std::size_t>> scopes{
        {0, 1}, {1, 2}, {0, 2}
    };
    const auto violated = [](
        std::size_t constraint,
        std::span<const std::size_t> assignment
    ){
        constexpr std::size_t first[] = {0, 1, 0};
        constexpr std::size_t second[] = {1, 2, 2};
        return assignment[first[constraint]]
            == assignment[second[constraint]];
    };
    const auto weighted = approximate::constraint::constraint_weighting(
        domains, scopes, violated, random, 100, 10
    );
    const auto csp_walk = approximate::constraint::random_walk_csp(
        domains, scopes, violated, random, 100, 20
    );
    assert(weighted.found);
    assert(csp_walk.found);
}

auto quadratic_objective() {
    return [](const std::vector<double>& point){
        double value = 0.0;
        for(const double coordinate : point){
            const double difference = coordinate - 1.0;
            value += difference * difference;
        }
        return value;
    };
}

void continuous_tests(std::mt19937& random) {
    const auto objective = quadratic_objective();
    const auto sample_gradient = [](
        const std::vector<double>& point, std::size_t sample
    ){
        const double target = sample == 0 ? 0.5 : 1.5;
        std::vector<double> gradient(point.size());
        for(std::size_t coordinate = 0;
            coordinate < point.size(); ++coordinate){
            gradient[coordinate] =
                2.0 * (point[coordinate] - target);
        }
        return gradient;
    };
    const auto stochastic =
        approximate::optimization::stochastic_gradient_descent(
            std::vector<double>{4.0, -2.0}, objective,
            sample_gradient, 2, random, 100, 0.05
        );
    const auto mini_batch =
        approximate::optimization::mini_batch_gradient_descent(
            std::vector<double>{4.0, -2.0}, objective,
            sample_gradient, 2, 8, random, 100, 0.05
        );
    const auto stochastic_coordinate =
        approximate::optimization::stochastic_coordinate_descent(
            std::vector<double>{4.0, -2.0}, objective,
            [](const std::vector<double>& point, std::size_t coordinate){
                return 2.0 * (point[coordinate] - 1.0);
            },
            random, 100, 0.1
        );
    assert(stochastic.value < 18.0);
    assert(mini_batch.value < 18.0);
    assert(stochastic_coordinate.value < 18.0);

    const auto newton = approximate::optimization::newton_method(
        std::vector<double>{4.0, -2.0}, objective,
        [](const std::vector<double>& point){
            std::vector<double> result(point.size());
            for(std::size_t index = 0; index < point.size(); ++index){
                result[index] = 2.0 * (point[index] - 1.0);
            }
            return result;
        },
        [](const std::vector<double>& point){
            std::vector<std::vector<double>> result(
                point.size(), std::vector<double>(point.size(), 0.0)
            );
            for(std::size_t index = 0; index < point.size(); ++index){
                result[index][index] = 2.0;
            }
            return result;
        },
        20, 100
    );
    assert(newton.optimization.value < 1.0e-12);
    assert(newton.hessian_evaluations > 0);

    const auto conjugate =
        approximate::optimization::conjugate_gradient<double>(
            [](const std::vector<double>& point){
                return std::vector<double>{
                    4.0 * point[0], 9.0 * point[1]
                };
            },
            std::vector<double>{8.0, 27.0},
            std::vector<double>{0.0, 0.0}, 10
        );
    assert(conjugate.converged);
    assert(std::abs(conjugate.point[0] - 2.0) < 1.0e-10);
    assert(std::abs(conjugate.point[1] - 3.0) < 1.0e-10);

    const std::vector<std::vector<double>> directions{
        {1.0, 0.0}, {-1.0, 0.0},
        {0.0, 1.0}, {0.0, -1.0}
    };
    const auto pattern = approximate::optimization::pattern_search(
        std::vector<double>{4.0, -2.0}, objective, directions,
        100, 1000, 1.0, 0.5, 1.0e-7
    );
    assert(pattern.value < 1.0e-8);

    assert(approximate::optimization::expected_improvement(
        0.0L, 1.0L, 1.0L
    ) > 0.0L);
    assert(approximate::optimization::probability_of_improvement(
        0.0L, 1.0L, 1.0L
    ) > 0.5L);
    assert(approximate::optimization::upper_confidence_bound(
        1.0L, 2.0L, 3.0L
    ) == 7.0L);
    const std::vector<double> lower{-3.0, -3.0};
    const std::vector<double> upper{3.0, 3.0};
    const auto bayesian =
        approximate::optimization::bayesian_optimization(
            objective, lower, upper, random, 4, 4, 20
        );
    const auto ei =
        approximate::optimization::expected_improvement_optimization(
            objective, lower, upper, random, 4, 3, 15
        );
    const auto pi =
        approximate::optimization::probability_of_improvement_optimization(
            objective, lower, upper, random, 4, 3, 15
        );
    const auto ucb =
        approximate::optimization::upper_confidence_bound_optimization(
            objective, lower, upper, random, 4, 3, 15
        );
    for(const auto* result : {&bayesian, &ei, &pi, &ucb}){
        assert(result->point.size() == 2);
        assert(std::isfinite(result->value));
    }
    const auto direct = approximate::optimization::direct_optimization(
        objective, lower, upper, 20, 100
    );
    assert(direct.value <= objective({0.0, 0.0}));
    const auto basin = approximate::optimization::basin_hopping(
        std::vector<double>{2.5, -2.5}, objective, random,
        10, 10, 1000, 0.5, 0.1, 0.5
    );
    assert(basin.value < 1.0e-6);
}

}  // namespace

int main() {
    std::mt19937 random(20260729U);
    sat_and_csp_tests(random);
    continuous_tests(random);

    std::size_t size;
    if(!(std::cin >> size)) return 0;
    std::vector<double> diagonal(size);
    std::vector<double> right_hand_side(size);
    for(double& value : diagonal) std::cin >> value;
    for(double& value : right_hand_side) std::cin >> value;
    const auto result =
        approximate::optimization::conjugate_gradient<double>(
            [&](const std::vector<double>& point){
                std::vector<double> product(size);
                for(std::size_t index = 0; index < size; ++index){
                    product[index] = diagonal[index] * point[index];
                }
                return product;
            },
            right_hand_side, std::vector<double>(size, 0.0),
            size + 2, 1.0e-10
        );
    for(std::size_t index = 0; index < size; ++index){
        if(index != 0) std::cout << ' ';
        std::cout << std::llround(result.point[index]);
    }
    std::cout << '\n';
}
