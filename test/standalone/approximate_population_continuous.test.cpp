// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/evolutionary/differential_evolution.hpp"
#include "../../src/approximate/evolutionary/particle_swarm_optimization.hpp"

namespace {

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
        for(auto& value: target) std::cin >> value;
        std::vector<double> lower(dimension);
        std::vector<double> upper(dimension);
        for(std::size_t index = 0; index < dimension; ++index){
            lower[index] = target[index] - 20.0;
            upper[index] = target[index] + 20.0;
        }
        std::vector<std::vector<double>> population(12, target);
        for(std::size_t member = 0; member < population.size(); ++member){
            for(std::size_t index = 0; index < dimension; ++index){
                const double ratio = static_cast<double>(member + 1)
                    / static_cast<double>(population.size() + 1);
                population[member][index] = lower[index]
                    + ratio * (upper[index] - lower[index]);
            }
        }
        const auto evaluate = [&target](const std::vector<double>& point){
            double score = 0.0;
            for(std::size_t index = 0; index < point.size(); ++index){
                const double difference = point[index] - target[index];
                score += difference * difference;
            }
            return score;
        };
        double initial_best = evaluate(population.front());
        for(const auto& point: population){
            initial_best = std::min(initial_best, evaluate(point));
        }

        std::mt19937_64 first_random(2026071306ULL);
        std::mt19937_64 second_random(2026071306ULL);
        const auto differential = approximate::evolutionary::
            differential_evolution(
                population, lower, upper, evaluate, 100,
                0.8, 0.9, first_random
            );
        const auto differential_repeated = approximate::evolutionary::
            differential_evolution(
                population, lower, upper, evaluate, 100,
                0.8, 0.9, second_random
            );
        assert(differential.individual == differential_repeated.individual);
        assert(differential.score == differential_repeated.score);
        assert(differential.score <= initial_best);
        assert(inside(differential.individual, lower, upper));
        assert(differential.evaluations == 12U * 101U);

        std::mt19937_64 third_random(2026071307ULL);
        std::mt19937_64 fourth_random(2026071307ULL);
        const auto swarm = approximate::evolutionary::
            particle_swarm_optimization(
                population, lower, upper, evaluate, 120,
                0.72, 1.45, 1.45, third_random
            );
        const auto swarm_repeated = approximate::evolutionary::
            particle_swarm_optimization(
                population, lower, upper, evaluate, 120,
                0.72, 1.45, 1.45, fourth_random
            );
        assert(swarm.individual == swarm_repeated.individual);
        assert(swarm.score == swarm_repeated.score);
        assert(swarm.score <= initial_best);
        assert(inside(swarm.individual, lower, upper));
        assert(swarm.evaluations == 12U * 121U);
    }

    bool thrown = false;
    try{
        std::mt19937_64 random(1);
        std::vector<std::vector<double>> too_small{{0.0}, {1.0}, {2.0}};
        static_cast<void>(approximate::evolutionary::differential_evolution(
            too_small, std::vector<double>{-2.0}, std::vector<double>{2.0},
            [](const auto& point){ return point.front(); }, 1,
            0.5, 0.5, random
        ));
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    std::cout << "OK\n";
}
