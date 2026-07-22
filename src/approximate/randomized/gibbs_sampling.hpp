#ifndef CPPLIB_SRC_APPROXIMATE_RANDOMIZED_GIBBS_SAMPLING_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_RANDOMIZED_GIBBS_SAMPLING_HPP_INCLUDED

#include <concepts>
#include <cstddef>
#include <functional>
#include <limits>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace approximate::randomized {

template<class Value, class ConditionalSampler, class UniformRandomBitGenerator>
[[nodiscard]] std::vector<std::vector<Value>> gibbs_sample(
    std::vector<Value> state,
    ConditionalSampler conditional_sampler,
    std::size_t burn_in_sweeps,
    std::size_t sweeps_between_samples,
    std::size_t sample_count,
    UniformRandomBitGenerator& generator
){
    static_assert(std::copy_constructible<Value>);
    static_assert(std::is_convertible_v<
        std::invoke_result_t<
            ConditionalSampler&,
            std::size_t,
            std::span<const Value>,
            UniformRandomBitGenerator&
        >,
        Value
    >);
    if(sample_count != 0 && sweeps_between_samples == 0)[[unlikely]]{
        throw std::invalid_argument(
            "sweeps_between_samples must be positive when samples are requested"
        );
    }
    constexpr std::size_t maximum = std::numeric_limits<std::size_t>::max();
    if(sample_count != 0
       && sweeps_between_samples > (maximum - burn_in_sweeps) / sample_count){
        throw std::overflow_error("the total Gibbs sweep count overflows size_t");
    }
    std::vector<std::vector<Value>> samples;
    if(sample_count > samples.max_size())[[unlikely]]{
        throw std::length_error("the Gibbs sample count is too large");
    }
    samples.reserve(sample_count);
    const auto sweep = [&]{
        for(std::size_t coordinate = 0; coordinate < state.size(); ++coordinate){
            state[coordinate] = static_cast<Value>(std::invoke(
                conditional_sampler,
                coordinate,
                std::span<const Value>(state),
                generator
            ));
        }
    };

    std::size_t completed = 0;
    while(completed < burn_in_sweeps){
        sweep();
        ++completed;
    }
    std::size_t produced = 0;
    while(produced < sample_count){
        std::size_t separated = 0;
        while(separated < sweeps_between_samples){
            sweep();
            ++separated;
        }
        samples.push_back(state);
        ++produced;
    }
    return samples;
}

}  // namespace approximate::randomized

#endif  // CPPLIB_SRC_APPROXIMATE_RANDOMIZED_GIBBS_SAMPLING_HPP_INCLUDED
