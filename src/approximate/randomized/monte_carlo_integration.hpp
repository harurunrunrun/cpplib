#ifndef CPPLIB_SRC_APPROXIMATE_RANDOMIZED_MONTE_CARLO_INTEGRATION_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_RANDOMIZED_MONTE_CARLO_INTEGRATION_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace approximate::randomized {

struct MonteCarloEstimate {
    double estimate = 0.0;
    double standard_error = 0.0;
    std::size_t samples = 0;
};

namespace detail {

class RunningMoments {
public:
    void add(long double value) {
        ++count_;
        const long double difference = value - mean_;
        mean_ += difference / static_cast<long double>(count_);
        squared_deviation_ += difference * (value - mean_);
        if(!std::isfinite(mean_) || !std::isfinite(squared_deviation_)){
            throw std::overflow_error("sample moments overflow");
        }
    }

    long double mean() const noexcept { return mean_; }

    long double standard_error() const {
        if(count_ <= 1) return 0.0L;
        const long double variance = squared_deviation_ / static_cast<long double>(count_ - 1);
        return std::sqrt(std::max(0.0L, variance) / static_cast<long double>(count_));
    }

private:
    std::size_t count_ = 0;
    long double mean_ = 0.0L;
    long double squared_deviation_ = 0.0L;
};

inline double checked_double(long double value, const char* message) {
    if(!std::isfinite(value)
       || value > static_cast<long double>(std::numeric_limits<double>::max())
       || value < -static_cast<long double>(std::numeric_limits<double>::max())){
        throw std::overflow_error(message);
    }
    return static_cast<double>(value);
}

template<class Function, class Point, class RandomEngine>
MonteCarloEstimate integrate_box(
    const Point& lower,
    const Point& upper,
    std::size_t sample_count,
    Function function,
    RandomEngine& random
) {
    if(sample_count == 0) throw std::invalid_argument("sample_count must be positive");
    if(lower.size() != upper.size()) throw std::invalid_argument("bound dimensions differ");
    const std::size_t dimension = lower.size();
    long double volume = 1.0L;
    std::vector<std::uniform_real_distribution<long double>> distributions;
    distributions.reserve(dimension);
    for(std::size_t index = 0; index < dimension; ++index){
        const double low = static_cast<double>(lower[index]);
        const double high = static_cast<double>(upper[index]);
        if(!std::isfinite(low) || !std::isfinite(high) || low > high){
            throw std::invalid_argument("bounds must be finite and ordered");
        }
        volume *= static_cast<long double>(high) - static_cast<long double>(low);
        if(!std::isfinite(volume)) throw std::overflow_error("integration volume overflow");
        distributions.emplace_back(low, high);
    }

    Point point{};
    if constexpr(requires(Point value) { value.resize(std::size_t{}); }) point.resize(dimension);
    RunningMoments moments;
    for(std::size_t sample = 0; sample < sample_count; ++sample){
        for(std::size_t index = 0; index < dimension; ++index){
            point[index] = static_cast<double>(distributions[index](random));
        }
        const long double value = static_cast<long double>(function(point));
        if(!std::isfinite(value)) throw std::domain_error("integrand returned a non-finite value");
        moments.add(value);
    }
    const long double estimate = volume * moments.mean();
    const long double standard_error = std::fabs(volume) * moments.standard_error();
    return MonteCarloEstimate{
        checked_double(estimate, "integral estimate overflow"),
        checked_double(standard_error, "standard error overflow"),
        sample_count
    };
}

}  // namespace detail

template<class Function, class RandomEngine>
MonteCarloEstimate monte_carlo_integrate(
    Function function,
    double lower,
    double upper,
    std::size_t sample_count,
    RandomEngine& random
) {
    if(!std::isfinite(lower) || !std::isfinite(upper) || lower > upper){
        throw std::invalid_argument("bounds must be finite and ordered");
    }
    const std::array<double, 1> low{lower};
    const std::array<double, 1> high{upper};
    return detail::integrate_box(
        low, high, sample_count,
        [&function](const std::array<double, 1>& point){ return function(point[0]); }, random
    );
}

template<class Function, class RandomEngine>
MonteCarloEstimate monte_carlo_integrate_hyperrectangle(
    const std::vector<double>& lower,
    const std::vector<double>& upper,
    std::size_t sample_count,
    Function function,
    RandomEngine& random
) {
    return detail::integrate_box(lower, upper, sample_count, std::move(function), random);
}

template<std::size_t Dimension, class Function, class RandomEngine>
MonteCarloEstimate monte_carlo_integrate_hyperrectangle(
    const std::array<double, Dimension>& lower,
    const std::array<double, Dimension>& upper,
    std::size_t sample_count,
    Function function,
    RandomEngine& random
) {
    return detail::integrate_box(lower, upper, sample_count, std::move(function), random);
}

}  // namespace approximate::randomized

#endif  // CPPLIB_SRC_APPROXIMATE_RANDOMIZED_MONTE_CARLO_INTEGRATION_HPP_INCLUDED
