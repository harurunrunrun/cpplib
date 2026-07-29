#ifndef CPPLIB_SRC_APPROXIMATE_EVOLUTIONARY_NATURE_INSPIRED_OPTIMIZATION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_EVOLUTIONARY_NATURE_INSPIRED_OPTIMIZATION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <numeric>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "evolution_result.hpp"
#include "detail/box_constraints.hpp"
#include "detail/objective.hpp"
#include "detail/safe_count.hpp"

namespace approximate::evolutionary {

namespace nature_inspired_detail {

template<class Real, class URBG>
std::vector<Real> random_point(
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    URBG& random
){
    std::vector<Real> point(lower.size());
    for(std::size_t i = 0; i < point.size(); ++i){
        std::uniform_real_distribution<Real> coordinate(lower[i], upper[i]);
        point[i] = coordinate(random);
    }
    return point;
}

template<class Score, class Compare>
std::vector<std::size_t> order(
    const std::vector<Score>& scores,
    Compare& compare
){
    std::vector<std::size_t> result(scores.size());
    std::iota(result.begin(), result.end(), std::size_t{0});
    std::stable_sort(
        result.begin(), result.end(),
        [&](std::size_t lhs, std::size_t rhs){
            return std::invoke(compare, scores[lhs], scores[rhs]);
        }
    );
    return result;
}

template<class Real>
Real squared_distance(
    const std::vector<Real>& lhs,
    const std::vector<Real>& rhs
){
    Real result{};
    for(std::size_t i = 0; i < lhs.size(); ++i){
        const Real difference = lhs[i] - rhs[i];
        result += difference * difference;
    }
    return result;
}

}  // namespace nature_inspired_detail

template<class Real, class Evaluate, class URBG, class Compare = std::less<>>
auto artificial_bee_colony(
    std::vector<std::vector<Real>> food_sources,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Evaluate evaluate,
    std::size_t iterations,
    std::size_t abandonment_limit,
    URBG& random,
    Compare compare = {}
){
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_population_box(food_sources, lower, upper, 2);
    if(abandonment_limit == 0)[[unlikely]]{
        throw std::invalid_argument("ABC requires positive abandonment limit");
    }
    using Individual = std::vector<Real>;
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const Individual&>
    >;
    const std::size_t population_size = food_sources.size();
    std::size_t evaluations = population_size;
    std::vector<Score> scores;
    scores.reserve(population_size);
    for(const auto& source : food_sources){
        scores.push_back(detail::evaluate_objective(evaluate, source));
    }
    std::vector<std::size_t> trials(population_size);
    std::uniform_int_distribution<std::size_t> choose_dimension(
        0, lower.size() - 1
    );
    std::uniform_real_distribution<Real> phi(Real{-1}, Real{1});
    const auto attempt = [&](std::size_t source){
        std::uniform_int_distribution<std::size_t> choose_other(
            0, population_size - 2
        );
        std::size_t other = choose_other(random);
        if(other >= source) ++other;
        const std::size_t dimension = choose_dimension(random);
        Individual candidate = food_sources[source];
        candidate[dimension] += phi(random)
            * (candidate[dimension] - food_sources[other][dimension]);
        candidate[dimension] = std::clamp(
            candidate[dimension], lower[dimension], upper[dimension]
        );
        Score candidate_score =
            detail::evaluate_objective(evaluate, candidate);
        ++evaluations;
        if(std::invoke(compare, candidate_score, scores[source])){
            food_sources[source] = std::move(candidate);
            scores[source] = std::move(candidate_score);
            trials[source] = 0;
        }else{
            ++trials[source];
        }
    };
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        for(std::size_t source = 0; source < population_size; ++source){
            attempt(source);
        }
        const auto ranked =
            nature_inspired_detail::order(scores, compare);
        std::vector<double> weights(population_size);
        for(std::size_t rank = 0; rank < population_size; ++rank){
            weights[ranked[rank]] =
                static_cast<double>(population_size - rank);
        }
        std::discrete_distribution<std::size_t> choose_source(
            weights.begin(), weights.end()
        );
        for(std::size_t bee = 0; bee < population_size; ++bee){
            attempt(choose_source(random));
        }
        for(std::size_t source = 0; source < population_size; ++source){
            if(trials[source] < abandonment_limit) continue;
            food_sources[source] = nature_inspired_detail::random_point(
                lower, upper, random
            );
            scores[source] = detail::evaluate_objective(
                evaluate, food_sources[source]
            );
            ++evaluations;
            trials[source] = 0;
        }
    }
    const auto ranked = nature_inspired_detail::order(scores, compare);
    return EvolutionResult<Individual, Score>{
        food_sources[ranked.front()], scores[ranked.front()],
        iterations, evaluations
    };
}

template<class Real, class Evaluate, class URBG, class Compare = std::less<>>
auto bees_algorithm(
    std::vector<std::vector<Real>> scouts,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Evaluate evaluate,
    std::size_t iterations,
    std::size_t selected_sites,
    std::size_t elite_sites,
    std::size_t recruits_per_selected,
    std::size_t recruits_per_elite,
    Real initial_patch_size,
    Real patch_decay,
    URBG& random,
    Compare compare = {}
){
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_population_box(scouts, lower, upper, 1);
    if(selected_sites == 0 || scouts.size() < selected_sites
       || selected_sites < elite_sites
       || recruits_per_selected == 0 || recruits_per_elite == 0
       || !(initial_patch_size > Real{})
       || !(patch_decay > Real{} && patch_decay <= Real{1}))[[unlikely]]{
        throw std::invalid_argument("bees_algorithm has invalid parameters");
    }
    using Individual = std::vector<Real>;
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const Individual&>
    >;
    std::vector<Score> scores;
    scores.reserve(scouts.size());
    for(const auto& scout : scouts){
        scores.push_back(detail::evaluate_objective(evaluate, scout));
    }
    std::size_t evaluations = scouts.size();
    Real patch_size = initial_patch_size;
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        const auto ranked =
            nature_inspired_detail::order(scores, compare);
        std::vector<Individual> next;
        std::vector<Score> next_scores;
        next.reserve(scouts.size());
        next_scores.reserve(scouts.size());
        for(std::size_t site = 0; site < selected_sites; ++site){
            Individual site_best = scouts[ranked[site]];
            Score site_score = scores[ranked[site]];
            const std::size_t recruits = site < elite_sites
                ? recruits_per_elite : recruits_per_selected;
            for(std::size_t recruit = 0; recruit < recruits; ++recruit){
                Individual candidate = scouts[ranked[site]];
                for(std::size_t dimension = 0;
                    dimension < candidate.size();
                    ++dimension){
                    std::uniform_real_distribution<Real> offset(
                        -patch_size, patch_size
                    );
                    candidate[dimension] = std::clamp(
                        candidate[dimension] + offset(random),
                        lower[dimension], upper[dimension]
                    );
                }
                Score candidate_score =
                    detail::evaluate_objective(evaluate, candidate);
                ++evaluations;
                if(std::invoke(compare, candidate_score, site_score)){
                    site_best = std::move(candidate);
                    site_score = std::move(candidate_score);
                }
            }
            next.push_back(std::move(site_best));
            next_scores.push_back(std::move(site_score));
        }
        while(next.size() < scouts.size()){
            Individual scout = nature_inspired_detail::random_point(
                lower, upper, random
            );
            next_scores.push_back(
                detail::evaluate_objective(evaluate, scout)
            );
            ++evaluations;
            next.push_back(std::move(scout));
        }
        scouts = std::move(next);
        scores = std::move(next_scores);
        patch_size *= patch_decay;
    }
    const auto ranked = nature_inspired_detail::order(scores, compare);
    return EvolutionResult<Individual, Score>{
        scouts[ranked.front()], scores[ranked.front()],
        iterations, evaluations
    };
}

template<class Real, class Evaluate, class URBG, class Compare = std::less<>>
auto harmony_search(
    std::vector<std::vector<Real>> harmony_memory,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Evaluate evaluate,
    std::size_t iterations,
    Real memory_consideration_rate,
    Real pitch_adjustment_rate,
    Real bandwidth,
    URBG& random,
    Compare compare = {}
){
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_population_box(harmony_memory, lower, upper, 1);
    if(memory_consideration_rate < Real{}
       || Real{1} < memory_consideration_rate
       || pitch_adjustment_rate < Real{}
       || Real{1} < pitch_adjustment_rate
       || bandwidth < Real{})[[unlikely]]{
        throw std::invalid_argument("harmony_search has invalid parameters");
    }
    using Individual = std::vector<Real>;
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const Individual&>
    >;
    std::vector<Score> scores;
    scores.reserve(harmony_memory.size());
    for(const auto& harmony : harmony_memory){
        scores.push_back(detail::evaluate_objective(evaluate, harmony));
    }
    std::bernoulli_distribution use_memory(memory_consideration_rate);
    std::bernoulli_distribution adjust_pitch(pitch_adjustment_rate);
    std::uniform_int_distribution<std::size_t> choose_harmony(
        0, harmony_memory.size() - 1
    );
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        Individual candidate(lower.size());
        for(std::size_t dimension = 0;
            dimension < candidate.size();
            ++dimension){
            if(use_memory(random)){
                candidate[dimension] =
                    harmony_memory[choose_harmony(random)][dimension];
                if(adjust_pitch(random)){
                    std::uniform_real_distribution<Real> offset(
                        -bandwidth, bandwidth
                    );
                    candidate[dimension] += offset(random);
                }
            }else{
                std::uniform_real_distribution<Real> coordinate(
                    lower[dimension], upper[dimension]
                );
                candidate[dimension] = coordinate(random);
            }
            candidate[dimension] = std::clamp(
                candidate[dimension], lower[dimension], upper[dimension]
            );
        }
        Score candidate_score =
            detail::evaluate_objective(evaluate, candidate);
        const auto ranked =
            nature_inspired_detail::order(scores, compare);
        const std::size_t worst = ranked.back();
        if(std::invoke(compare, candidate_score, scores[worst])){
            harmony_memory[worst] = std::move(candidate);
            scores[worst] = std::move(candidate_score);
        }
    }
    const auto ranked = nature_inspired_detail::order(scores, compare);
    return EvolutionResult<Individual, Score>{
        harmony_memory[ranked.front()], scores[ranked.front()],
        iterations, harmony_memory.size() + iterations
    };
}

template<class Real, class Evaluate, class URBG, class Compare = std::less<>>
auto firefly_algorithm(
    std::vector<std::vector<Real>> population,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Evaluate evaluate,
    std::size_t iterations,
    Real attractiveness,
    Real absorption,
    Real randomness,
    URBG& random,
    Compare compare = {}
){
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_population_box(population, lower, upper, 1);
    if(attractiveness < Real{} || absorption < Real{}
       || randomness < Real{})[[unlikely]]{
        throw std::invalid_argument("firefly_algorithm has negative parameters");
    }
    using Individual = std::vector<Real>;
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const Individual&>
    >;
    std::vector<Score> scores;
    scores.reserve(population.size());
    for(const auto& individual : population){
        scores.push_back(detail::evaluate_objective(evaluate, individual));
    }
    std::size_t evaluations = population.size();
    std::uniform_real_distribution<Real> noise(Real{-0.5}, Real{0.5});
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        for(std::size_t i = 0; i < population.size(); ++i){
            for(std::size_t j = 0; j < population.size(); ++j){
                if(!std::invoke(compare, scores[j], scores[i])) continue;
                const Real beta = attractiveness * std::exp(
                    -absorption * nature_inspired_detail::squared_distance(
                        population[i], population[j]
                    )
                );
                Individual candidate = population[i];
                for(std::size_t dimension = 0;
                    dimension < candidate.size();
                    ++dimension){
                    candidate[dimension] += beta
                        * (population[j][dimension]
                           - candidate[dimension])
                        + randomness * noise(random);
                    candidate[dimension] = std::clamp(
                        candidate[dimension],
                        lower[dimension], upper[dimension]
                    );
                }
                Score candidate_score =
                    detail::evaluate_objective(evaluate, candidate);
                ++evaluations;
                if(std::invoke(compare, candidate_score, scores[i])){
                    population[i] = std::move(candidate);
                    scores[i] = std::move(candidate_score);
                }
            }
        }
    }
    const auto ranked = nature_inspired_detail::order(scores, compare);
    return EvolutionResult<Individual, Score>{
        population[ranked.front()], scores[ranked.front()],
        iterations, evaluations
    };
}

template<class Real, class Evaluate, class URBG, class Compare = std::less<>>
auto cuckoo_search(
    std::vector<std::vector<Real>> nests,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Evaluate evaluate,
    std::size_t iterations,
    Real levy_scale,
    Real abandonment_fraction,
    URBG& random,
    Compare compare = {}
){
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_population_box(nests, lower, upper, 1);
    if(!(levy_scale > Real{}) || abandonment_fraction < Real{}
       || Real{1} < abandonment_fraction)[[unlikely]]{
        throw std::invalid_argument("cuckoo_search has invalid parameters");
    }
    using Individual = std::vector<Real>;
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const Individual&>
    >;
    std::vector<Score> scores;
    scores.reserve(nests.size());
    for(const auto& nest : nests){
        scores.push_back(detail::evaluate_objective(evaluate, nest));
    }
    std::size_t evaluations = nests.size();
    std::cauchy_distribution<Real> levy(Real{}, levy_scale);
    std::uniform_int_distribution<std::size_t> choose_nest(
        0, nests.size() - 1
    );
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        const auto ranked =
            nature_inspired_detail::order(scores, compare);
        const Individual best = nests[ranked.front()];
        for(std::size_t source = 0; source < nests.size(); ++source){
            Individual candidate = nests[source];
            for(std::size_t dimension = 0;
                dimension < candidate.size();
                ++dimension){
                candidate[dimension] += levy(random)
                    * (candidate[dimension] - best[dimension]);
                candidate[dimension] = std::clamp(
                    candidate[dimension],
                    lower[dimension], upper[dimension]
                );
            }
            Score candidate_score =
                detail::evaluate_objective(evaluate, candidate);
            ++evaluations;
            const std::size_t target = choose_nest(random);
            if(std::invoke(compare, candidate_score, scores[target])){
                nests[target] = std::move(candidate);
                scores[target] = std::move(candidate_score);
            }
        }
        const auto after = nature_inspired_detail::order(scores, compare);
        const std::size_t abandon = static_cast<std::size_t>(
            abandonment_fraction * static_cast<Real>(nests.size())
        );
        for(std::size_t offset = 0; offset < abandon; ++offset){
            const std::size_t index = after[after.size() - 1 - offset];
            nests[index] = nature_inspired_detail::random_point(
                lower, upper, random
            );
            scores[index] = detail::evaluate_objective(
                evaluate, nests[index]
            );
            ++evaluations;
        }
    }
    const auto ranked = nature_inspired_detail::order(scores, compare);
    return EvolutionResult<Individual, Score>{
        nests[ranked.front()], scores[ranked.front()],
        iterations, evaluations
    };
}

template<class Real, class Evaluate, class URBG, class Compare = std::less<>>
auto bat_algorithm(
    std::vector<std::vector<Real>> bats,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Evaluate evaluate,
    std::size_t iterations,
    Real minimum_frequency,
    Real maximum_frequency,
    Real initial_loudness,
    Real initial_pulse_rate,
    Real loudness_decay,
    Real pulse_growth,
    URBG& random,
    Compare compare = {}
){
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_population_box(bats, lower, upper, 1);
    if(minimum_frequency < Real{}
       || maximum_frequency < minimum_frequency
       || initial_loudness < Real{} || initial_pulse_rate < Real{}
       || Real{1} < initial_pulse_rate
       || loudness_decay < Real{} || Real{1} < loudness_decay
       || pulse_growth < Real{})[[unlikely]]{
        throw std::invalid_argument("bat_algorithm has invalid parameters");
    }
    using Individual = std::vector<Real>;
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const Individual&>
    >;
    std::vector<Score> scores;
    scores.reserve(bats.size());
    for(const auto& bat : bats){
        scores.push_back(detail::evaluate_objective(evaluate, bat));
    }
    std::vector<Individual> velocity(
        bats.size(), Individual(lower.size())
    );
    std::vector<Real> loudness(bats.size(), initial_loudness);
    std::vector<Real> pulse_rate(bats.size(), initial_pulse_rate);
    std::uniform_real_distribution<Real> unit(Real{}, Real{1});
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        const auto ranked =
            nature_inspired_detail::order(scores, compare);
        const Individual best = bats[ranked.front()];
        for(std::size_t i = 0; i < bats.size(); ++i){
            const Real frequency = minimum_frequency
                + (maximum_frequency - minimum_frequency) * unit(random);
            Individual candidate = bats[i];
            for(std::size_t dimension = 0;
                dimension < candidate.size();
                ++dimension){
                velocity[i][dimension] +=
                    (bats[i][dimension] - best[dimension]) * frequency;
                candidate[dimension] += velocity[i][dimension];
            }
            if(unit(random) > pulse_rate[i]){
                for(std::size_t dimension = 0;
                    dimension < candidate.size();
                    ++dimension){
                    candidate[dimension] = best[dimension]
                        + (Real{2} * unit(random) - Real{1})
                            * loudness[i];
                }
            }
            detail::clamp_to_box(candidate, lower, upper);
            Score candidate_score =
                detail::evaluate_objective(evaluate, candidate);
            if(unit(random) < loudness[i]
               && std::invoke(compare, candidate_score, scores[i])){
                bats[i] = std::move(candidate);
                scores[i] = std::move(candidate_score);
                loudness[i] *= loudness_decay;
                pulse_rate[i] = initial_pulse_rate * (
                    Real{1} - std::exp(
                        -pulse_growth
                            * static_cast<Real>(iteration + 1)
                    )
                );
            }
        }
    }
    const auto ranked = nature_inspired_detail::order(scores, compare);
    return EvolutionResult<Individual, Score>{
        bats[ranked.front()], scores[ranked.front()],
        iterations, bats.size() * (iterations + 1)
    };
}

template<class Real, class Evaluate, class URBG, class Compare = std::less<>>
auto grey_wolf_optimizer(
    std::vector<std::vector<Real>> wolves,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    Evaluate evaluate,
    std::size_t iterations,
    URBG& random,
    Compare compare = {}
){
    static_assert(std::is_floating_point_v<Real>);
    detail::validate_population_box(wolves, lower, upper, 3);
    using Individual = std::vector<Real>;
    using Score = std::decay_t<
        std::invoke_result_t<Evaluate&, const Individual&>
    >;
    std::vector<Score> scores;
    scores.reserve(wolves.size());
    for(const auto& wolf : wolves){
        scores.push_back(detail::evaluate_objective(evaluate, wolf));
    }
    std::uniform_real_distribution<Real> unit(Real{}, Real{1});
    for(std::size_t iteration = 0; iteration < iterations; ++iteration){
        const auto ranked =
            nature_inspired_detail::order(scores, compare);
        const Individual alpha = wolves[ranked[0]];
        const Individual beta = wolves[ranked[1]];
        const Individual delta = wolves[ranked[2]];
        const Real a = Real{2} * (
            Real{1} - static_cast<Real>(iteration)
                / static_cast<Real>(std::max(iterations, std::size_t{1}))
        );
        std::vector<Individual> next = wolves;
        for(std::size_t i = 0; i < wolves.size(); ++i){
            for(std::size_t dimension = 0;
                dimension < wolves[i].size();
                ++dimension){
                Real sum{};
                for(const Individual* leader : {&alpha, &beta, &delta}){
                    const Real coefficient_a =
                        Real{2} * a * unit(random) - a;
                    const Real coefficient_c = Real{2} * unit(random);
                    const Real distance = std::fabs(
                        coefficient_c * (*leader)[dimension]
                        - wolves[i][dimension]
                    );
                    sum += (*leader)[dimension]
                        - coefficient_a * distance;
                }
                next[i][dimension] = std::clamp(
                    sum / Real{3}, lower[dimension], upper[dimension]
                );
            }
        }
        wolves = std::move(next);
        for(std::size_t i = 0; i < wolves.size(); ++i){
            scores[i] = detail::evaluate_objective(evaluate, wolves[i]);
        }
    }
    const auto ranked = nature_inspired_detail::order(scores, compare);
    return EvolutionResult<Individual, Score>{
        wolves[ranked.front()], scores[ranked.front()],
        iterations, wolves.size() * (iterations + 1)
    };
}

}  // namespace approximate::evolutionary

#endif  // CPPLIB_SRC_APPROXIMATE_EVOLUTIONARY_NATURE_INSPIRED_OPTIMIZATION_HPP_INCLUDED
