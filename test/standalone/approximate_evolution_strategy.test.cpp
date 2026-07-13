// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/evolutionary/evolution_strategy.hpp"

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
        std::vector<double> initial(dimension);
        for(std::size_t index = 0; index < dimension; ++index){
            lower[index] = target[index] - 10.0;
            upper[index] = target[index] + 10.0;
            initial[index] = upper[index];
        }
        const auto evaluate = [&target](const std::vector<double>& point){
            double score = 0.0;
            for(std::size_t index = 0; index < point.size(); ++index){
                const double difference = point[index] - target[index];
                score += difference * difference;
            }
            return score;
        };
        const double initial_score = evaluate(initial);

        std::mt19937_64 first_random(2026071303ULL);
        std::mt19937_64 second_random(2026071303ULL);
        const auto one = approximate::evolutionary::one_plus_one_es(
            initial, lower, upper, evaluate, 400, 1.0,
            first_random, 1.2
        );
        const auto repeated = approximate::evolutionary::one_plus_one_es(
            initial, lower, upper, evaluate, 400, 1.0,
            second_random, 1.2
        );
        assert(one.individual == repeated.individual);
        assert(one.score == repeated.score);
        assert(one.score <= initial_score);
        assert(inside(one.individual, lower, upper));
        assert(one.evaluations == 401U);

        std::vector<std::vector<double>> population;
        for(std::size_t member = 0; member < 6; ++member){
            std::vector<double> point(dimension);
            for(std::size_t index = 0; index < dimension; ++index){
                point[index] = lower[index]
                    + (upper[index] - lower[index])
                        * static_cast<double>(member)
                        / 5.0;
            }
            population.push_back(std::move(point));
        }
        double population_best = evaluate(population.front());
        for(const auto& point: population){
            population_best = std::min(population_best, evaluate(point));
        }
        std::mt19937_64 plus_random(2026071304ULL);
        const auto plus = approximate::evolutionary::mu_plus_lambda_es(
            population, lower, upper, evaluate, 40, 18, 0.8, plus_random
        );
        assert(plus.score <= population_best);
        assert(inside(plus.individual, lower, upper));
        assert(plus.evaluations == 6U + 40U * 18U);

        std::mt19937_64 comma_random(2026071305ULL);
        const auto comma = approximate::evolutionary::mu_comma_lambda_es(
            population, lower, upper, evaluate, 40, 18, 0.8, comma_random
        );
        assert(inside(comma.individual, lower, upper));
        assert(std::isfinite(comma.score));
        assert(comma.evaluations == 6U + 40U * 18U);
    }

    bool thrown = false;
    try{
        std::mt19937_64 random(1);
        std::vector<std::vector<double>> population{{0.0}, {1.0}, {2.0}};
        static_cast<void>(approximate::evolutionary::mu_comma_lambda_es(
            population, std::vector<double>{-3.0}, std::vector<double>{3.0},
            [](const auto& point){ return point.front() * point.front(); },
            1, 2, 1.0, random
        ));
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    assert(thrown);
    std::cout << "OK\n";
}
