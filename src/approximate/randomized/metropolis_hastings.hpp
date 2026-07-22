#ifndef CPPLIB_SRC_APPROXIMATE_RANDOMIZED_METROPOLIS_HASTINGS_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_RANDOMIZED_METROPOLIS_HASTINGS_HPP_INCLUDED

#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace approximate::randomized {

template<class State>
struct MetropolisHastingsResult {
    std::vector<State> samples;
    std::size_t accepted = 0;
    std::size_t proposals = 0;

    double acceptance_rate() const noexcept {
        if(proposals == 0) return 0.0;
        return static_cast<double>(accepted) / static_cast<double>(proposals);
    }
};

namespace detail {

inline bool valid_log_density(double value) noexcept {
    return std::isfinite(value) || value == -std::numeric_limits<double>::infinity();
}

template<class State, class LogDensity, class Proposal, class LogProposalRatio, class RandomEngine>
MetropolisHastingsResult<State> metropolis_hastings_impl(
    State initial_state,
    std::size_t sample_count,
    std::size_t burn_in,
    std::size_t thinning,
    LogDensity log_density,
    Proposal proposal,
    LogProposalRatio log_proposal_ratio,
    RandomEngine& random
) {
    if(sample_count == 0) throw std::invalid_argument("sample_count must be positive");
    if(thinning == 0) throw std::invalid_argument("thinning must be positive");
    if(sample_count > (std::numeric_limits<std::size_t>::max() - burn_in) / thinning){
        throw std::overflow_error("number of Markov-chain steps overflows size_t");
    }

    State current = std::move(initial_state);
    double current_log_density = static_cast<double>(log_density(current));
    if(!std::isfinite(current_log_density)){
        throw std::domain_error("initial log density must be finite");
    }

    MetropolisHastingsResult<State> result;
    result.samples.reserve(sample_count);
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    const auto step = [&]() {
        State candidate = proposal(current, random);
        const double candidate_log_density = static_cast<double>(log_density(candidate));
        if(!valid_log_density(candidate_log_density)){
            throw std::domain_error("log density must be finite or negative infinity");
        }
        bool accept = false;
        if(candidate_log_density != -std::numeric_limits<double>::infinity()){
            const double proposal_ratio = static_cast<double>(log_proposal_ratio(current, candidate));
            if(!std::isfinite(proposal_ratio)){
                throw std::domain_error("log proposal ratio must be finite");
            }
            const long double log_acceptance =
                static_cast<long double>(candidate_log_density)
                - static_cast<long double>(current_log_density)
                + static_cast<long double>(proposal_ratio);
            const double draw = uniform(random);
            const long double log_draw = draw == 0.0
                ? -std::numeric_limits<long double>::infinity()
                : std::log(static_cast<long double>(draw));
            accept = log_acceptance >= 0.0L || log_draw < log_acceptance;
        }
        ++result.proposals;
        if(accept){
            current = std::move(candidate);
            current_log_density = candidate_log_density;
            ++result.accepted;
        }
    };

    for(std::size_t index = 0; index < burn_in; ++index) step();
    for(std::size_t sample = 0; sample < sample_count; ++sample){
        for(std::size_t index = 0; index < thinning; ++index) step();
        result.samples.push_back(current);
    }
    return result;
}

}  // namespace detail

template<class State, class LogDensity, class Proposal, class LogProposalRatio, class RandomEngine>
MetropolisHastingsResult<State> metropolis_hastings(
    State initial_state,
    std::size_t sample_count,
    std::size_t burn_in,
    std::size_t thinning,
    LogDensity log_density,
    Proposal proposal,
    LogProposalRatio log_proposal_ratio,
    RandomEngine& random
) {
    return detail::metropolis_hastings_impl(
        std::move(initial_state), sample_count, burn_in, thinning,
        std::move(log_density), std::move(proposal), std::move(log_proposal_ratio), random
    );
}

template<class State, class LogDensity, class Proposal, class RandomEngine>
MetropolisHastingsResult<State> metropolis_hastings_symmetric(
    State initial_state,
    std::size_t sample_count,
    std::size_t burn_in,
    std::size_t thinning,
    LogDensity log_density,
    Proposal proposal,
    RandomEngine& random
) {
    return detail::metropolis_hastings_impl(
        std::move(initial_state), sample_count, burn_in, thinning,
        std::move(log_density), std::move(proposal),
        [](const State&, const State&){ return 0.0; }, random
    );
}

}  // namespace approximate::randomized

#endif  // CPPLIB_SRC_APPROXIMATE_RANDOMIZED_METROPOLIS_HASTINGS_HPP_INCLUDED
