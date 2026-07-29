#ifndef CPPLIB_SRC_APPROXIMATE_EVOLUTIONARY_CMA_ES_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_EVOLUTIONARY_CMA_ES_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <numeric>
#include <optional>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "detail/box_constraints.hpp"
#include "detail/objective.hpp"
#include "detail/safe_count.hpp"

namespace approximate::evolutionary {

template<class Real, class Score>
struct CmaEsResult{
    std::vector<Real> individual;
    Score score;
    std::vector<Real> mean;
    std::vector<std::vector<Real>> covariance;
    Real step_size;
    std::size_t iterations = 0;
    std::size_t evaluations = 0;
};

namespace cma_es_detail {

template<class Real>
std::vector<std::vector<Real>> cholesky(
    std::vector<std::vector<Real>> covariance
){
    const std::size_t dimension = covariance.size();
    std::vector<std::vector<Real>> factor(
        dimension, std::vector<Real>(dimension)
    );
    for(std::size_t row = 0; row < dimension; ++row){
        for(std::size_t column = 0; column <= row; ++column){
            Real value = covariance[row][column];
            for(std::size_t k = 0; k < column; ++k){
                value -= factor[row][k] * factor[column][k];
            }
            if(row == column){
                const Real floor = std::numeric_limits<Real>::epsilon();
                factor[row][column] = std::sqrt(std::max(value, floor));
            }else{
                factor[row][column] =
                    value / factor[column][column];
            }
        }
    }
    return factor;
}

template<class Real>
std::vector<Real> lower_solve(
    const std::vector<std::vector<Real>>& factor,
    const std::vector<Real>& value
){
    std::vector<Real> result(value.size());
    for(std::size_t row = 0; row < value.size(); ++row){
        Real current = value[row];
        for(std::size_t column = 0; column < row; ++column){
            current -= factor[row][column] * result[column];
        }
        result[row] = current / factor[row][row];
    }
    return result;
}

template<class Real>
Real norm(const std::vector<Real>& vector){
    Real squared{};
    for(const Real value : vector) squared += value * value;
    return std::sqrt(squared);
}

}  // namespace cma_es_detail

template<class Real, class Evaluate, class URBG, class Compare = std::less<>>
auto cma_es(
    std::vector<Real> initial_mean,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Evaluate evaluate,
    std::size_t generations,
    std::size_t population_size,
    Real initial_step_size,
    URBG& random,
    Compare compare = {}
){
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_population_box(
        std::vector<std::vector<Real>>{initial_mean}, lower, upper, 1
    );
    if(generations == 0 || population_size < 2
       || !(initial_step_size > Real{})
       || !std::isfinite(initial_step_size))[[unlikely]]{
        throw std::invalid_argument("cma_es has invalid parameters");
    }
    using Individual = std::vector<Real>;
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const Individual&>
    >;
    const std::size_t evaluations = detail::checked_multiply(
        generations, population_size,
        "CMA-ES evaluation count overflows size_t"
    );
    const std::size_t dimension = initial_mean.size();
    const std::size_t mu = population_size / 2;
    std::vector<Real> weights(mu);
    Real weight_sum{};
    Real squared_weight_sum{};
    for(std::size_t i = 0; i < mu; ++i){
        weights[i] = std::log(static_cast<Real>(mu) + Real{0.5})
            - std::log(static_cast<Real>(i + 1));
        weight_sum += weights[i];
    }
    for(Real& weight : weights){
        weight /= weight_sum;
        squared_weight_sum += weight * weight;
    }
    const Real mu_effective = Real{1} / squared_weight_sum;
    const Real n = static_cast<Real>(dimension);
    const Real cc = (Real{4} + mu_effective / n)
        / (n + Real{4} + Real{2} * mu_effective / n);
    const Real cs = (mu_effective + Real{2})
        / (n + mu_effective + Real{5});
    const Real c1 = Real{2}
        / ((n + Real{1.3}) * (n + Real{1.3}) + mu_effective);
    const Real cmu = std::min(
        Real{1} - c1,
        Real{2} * (mu_effective - Real{2} + Real{1} / mu_effective)
            / ((n + Real{2}) * (n + Real{2}) + mu_effective)
    );
    const Real damping = Real{1}
        + Real{2} * std::max(
            Real{}, std::sqrt((mu_effective - Real{1}) / (n + Real{1}))
                - Real{1}
        ) + cs;
    const Real expected_norm = std::sqrt(n)
        * (Real{1} - Real{1} / (Real{4} * n)
           + Real{1} / (Real{21} * n * n));

    std::vector<Real> mean = std::move(initial_mean);
    std::vector<std::vector<Real>> covariance(
        dimension, std::vector<Real>(dimension)
    );
    for(std::size_t i = 0; i < dimension; ++i){
        covariance[i][i] = Real{1};
    }
    std::vector<Real> evolution_path_c(dimension);
    std::vector<Real> evolution_path_sigma(dimension);
    Real step_size = initial_step_size;
    std::optional<Individual> best;
    std::optional<Score> best_score;
    std::normal_distribution<Real> normal(Real{}, Real{1});

    for(std::size_t generation = 0; generation < generations; ++generation){
        const auto factor = cma_es_detail::cholesky(covariance);
        std::vector<Individual> population(
            population_size, Individual(dimension)
        );
        std::vector<Individual> normalized_steps(
            population_size, Individual(dimension)
        );
        std::vector<Score> scores;
        scores.reserve(population_size);
        for(std::size_t sample = 0; sample < population_size; ++sample){
            Individual z(dimension);
            for(Real& value : z) value = normal(random);
            for(std::size_t row = 0; row < dimension; ++row){
                Real step{};
                for(std::size_t column = 0; column <= row; ++column){
                    step += factor[row][column] * z[column];
                }
                population[sample][row] =
                    mean[row] + step_size * step;
                population[sample][row] = std::clamp(
                    population[sample][row], lower[row], upper[row]
                );
                normalized_steps[sample][row] =
                    (population[sample][row] - mean[row]) / step_size;
            }
            Score score = detail::evaluate_objective(
                evaluate, population[sample]
            );
            if(!best_score || std::invoke(compare, score, *best_score)){
                best = population[sample];
                best_score = score;
            }
            scores.push_back(std::move(score));
        }
        std::vector<std::size_t> order(population_size);
        std::iota(order.begin(), order.end(), std::size_t{0});
        std::stable_sort(
            order.begin(), order.end(),
            [&](std::size_t lhs, std::size_t rhs){
                return std::invoke(compare, scores[lhs], scores[rhs]);
            }
        );
        Individual next_mean(dimension);
        Individual weighted_step(dimension);
        for(std::size_t rank = 0; rank < mu; ++rank){
            for(std::size_t coordinate = 0;
                coordinate < dimension;
                ++coordinate){
                next_mean[coordinate] += weights[rank]
                    * population[order[rank]][coordinate];
                weighted_step[coordinate] += weights[rank]
                    * normalized_steps[order[rank]][coordinate];
            }
        }
        const Individual whitened =
            cma_es_detail::lower_solve(factor, weighted_step);
        const Real sigma_path_scale =
            std::sqrt(cs * (Real{2} - cs) * mu_effective);
        const Real covariance_path_scale =
            std::sqrt(cc * (Real{2} - cc) * mu_effective);
        for(std::size_t coordinate = 0;
            coordinate < dimension;
            ++coordinate){
            evolution_path_sigma[coordinate] =
                (Real{1} - cs) * evolution_path_sigma[coordinate]
                + sigma_path_scale * whitened[coordinate];
        }
        const Real path_norm =
            cma_es_detail::norm(evolution_path_sigma);
        const Real normalization = std::sqrt(
            Real{1} - std::pow(
                Real{1} - cs,
                Real{2} * static_cast<Real>(generation + 1)
            )
        );
        const bool h_sigma =
            path_norm / normalization
            < (Real{1.4} + Real{2} / (n + Real{1}))
                * expected_norm;
        for(std::size_t coordinate = 0;
            coordinate < dimension;
            ++coordinate){
            evolution_path_c[coordinate] =
                (Real{1} - cc) * evolution_path_c[coordinate]
                + (h_sigma ? covariance_path_scale : Real{})
                    * weighted_step[coordinate];
        }
        const Real old_scale =
            Real{1} - c1 - cmu
            + (h_sigma ? Real{} : c1 * cc * (Real{2} - cc));
        std::vector<std::vector<Real>> next_covariance(
            dimension, std::vector<Real>(dimension)
        );
        for(std::size_t row = 0; row < dimension; ++row){
            for(std::size_t column = 0; column < dimension; ++column){
                Real value = old_scale * covariance[row][column]
                    + c1 * evolution_path_c[row]
                        * evolution_path_c[column];
                for(std::size_t rank = 0; rank < mu; ++rank){
                    value += cmu * weights[rank]
                        * normalized_steps[order[rank]][row]
                        * normalized_steps[order[rank]][column];
                }
                next_covariance[row][column] = value;
            }
        }
        covariance = std::move(next_covariance);
        mean = std::move(next_mean);
        step_size *= std::exp(
            (cs / damping) * (path_norm / expected_norm - Real{1})
        );
        if(!std::isfinite(step_size)){
            step_size = std::numeric_limits<Real>::max();
        }
        step_size = std::max(
            step_size, std::numeric_limits<Real>::min()
        );
    }
    return CmaEsResult<Real, Score>{
        std::move(*best), std::move(*best_score), std::move(mean),
        std::move(covariance), step_size, generations, evaluations
    };
}

}  // namespace approximate::evolutionary

#endif  // CPPLIB_SRC_APPROXIMATE_EVOLUTIONARY_CMA_ES_HPP_INCLUDED
