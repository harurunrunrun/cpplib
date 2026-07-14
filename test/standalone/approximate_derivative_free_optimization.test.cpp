// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/optimization/compass_search.hpp"
#include "../../src/approximate/optimization/nelder_mead.hpp"
#include "../../src/approximate/optimization/detail/common.hpp"

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
        const double initial_value = objective(initial);

        const auto compass = approximate::optimization::compass_search(
            initial, objective, 400, 8.0, 0.5, 1e-7, lower, upper
        );
        assert(compass.value < initial_value);
        assert(compass.value < 1e-10);
        assert(compass.gradient_evaluations == 0);
        assert(inside(compass.point, lower, upper));

        const auto simplex = approximate::optimization::nelder_mead(
            initial, objective, 2000, 1.0, 1e-8,
            1.0, 2.0, 0.5, 0.5, lower, upper
        );
        assert(simplex.value < initial_value);
        assert(simplex.value < 1e-10);
        assert(simplex.gradient_evaluations == 0);
        assert(inside(simplex.point, lower, upper));
    }

    const auto objective = [](const std::vector<double>& point){
        return point.front() * point.front();
    };
    const auto no_iteration = approximate::optimization::nelder_mead(
        std::vector<double>{2.0}, objective, 0, 1.0
    );
    assert(no_iteration.iterations == 0);
    assert(no_iteration.evaluations == 2);
    assert(no_iteration.value == 4.0);

    // Expansion is measured from the reflected point.  A non-default
    // reflection coefficient distinguishes it from a direct centroid-to-worst
    // expansion.
    const auto nondefault_reflection = approximate::optimization::nelder_mead(
        std::vector<double>{0.0},
        [](const std::vector<double>& point){
            const double difference = point.front() - 4.0;
            return difference * difference;
        },
        1, 1.0, 0.0, 0.5, 2.0
    );
    assert(nondefault_reflection.point == std::vector<double>{2.0});
    assert(nondefault_reflection.value == 4.0);

    const auto fixed = approximate::optimization::compass_search(
        std::vector<double>{0.0}, objective, 30, 1.0, 0.5, 1e-8,
        std::vector<double>{0.0}, std::vector<double>{0.0}
    );
    assert(fixed.converged);
    assert(fixed.value == 0.0);

    expect_throw<std::invalid_argument>([&]{
        static_cast<void>(approximate::optimization::compass_search(
            std::vector<double>{}, objective, 1, 1.0
        ));
    });
    expect_throw<std::invalid_argument>([&]{
        static_cast<void>(approximate::optimization::nelder_mead(
            std::vector<double>{0.0}, objective, 1, 0.0
        ));
    });
    expect_throw<std::domain_error>([&]{
        static_cast<void>(approximate::optimization::compass_search(
            std::vector<double>{0.0},
            [](const auto&){ return std::numeric_limits<double>::quiet_NaN(); },
            1, 1.0
        ));
    });
    expect_throw<std::overflow_error>([&]{
        static_cast<void>(approximate::optimization::compass_search(
            std::vector<double>{std::numeric_limits<double>::max()},
            [](const auto&){ return 0.0; }, 1,
            std::numeric_limits<double>::max()
        ));
    });
    expect_throw<std::overflow_error>([&]{
        static_cast<void>(approximate::optimization::nelder_mead(
            std::vector<double>{std::numeric_limits<double>::max()},
            [](const auto&){ return 0.0; }, 1,
            std::numeric_limits<double>::max()
        ));
    });
    std::cout << "OK\n";
}
