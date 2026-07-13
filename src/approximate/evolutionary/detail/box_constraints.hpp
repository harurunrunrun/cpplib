#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace approximate::evolutionary::detail {

template<class Real>
void validate_box(
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    std::size_t dimension
){
    if(lower.size() != dimension || upper.size() != dimension)[[unlikely]]{
        throw std::invalid_argument("bound dimensions do not match the individual");
    }
    for(std::size_t index = 0; index < dimension; ++index){
        if(!std::isfinite(lower[index]) || !std::isfinite(upper[index])
           || upper[index] < lower[index])[[unlikely]]{
            throw std::invalid_argument("box bounds must be finite and ordered");
        }
    }
}

template<class Real>
void clamp_to_box(
    std::vector<Real>& point,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper
){
    for(std::size_t index = 0; index < point.size(); ++index){
        if(std::isnan(point[index])){
            point[index] = lower[index];
            continue;
        }
        point[index] = std::clamp(point[index], lower[index], upper[index]);
    }
}

template<class Real>
void validate_population_box(
    const std::vector<std::vector<Real>>& population,
    const std::vector<Real>& lower,
    const std::vector<Real>& upper,
    std::size_t minimum_population
){
    if(population.size() < minimum_population)[[unlikely]]{
        throw std::invalid_argument("the initial population is too small");
    }
    if(population.front().empty())[[unlikely]]{
        throw std::invalid_argument("continuous individuals must be non-empty");
    }
    validate_box(lower, upper, population.front().size());
    for(const auto& point: population){
        if(point.size() != population.front().size())[[unlikely]]{
            throw std::invalid_argument("population dimensions are inconsistent");
        }
        for(std::size_t index = 0; index < point.size(); ++index){
            if(!std::isfinite(point[index]) || point[index] < lower[index]
               || upper[index] < point[index])[[unlikely]]{
                throw std::invalid_argument("an initial individual is outside the bounds");
            }
        }
    }
}

}  // namespace approximate::evolutionary::detail
