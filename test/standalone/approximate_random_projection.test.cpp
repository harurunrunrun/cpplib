// competitive-verifier: STANDALONE

#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/randomized/random_projection.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t point_count;
    std::size_t input_dimension;
    std::size_t output_dimension;
    std::uint64_t seed;
    if(!(std::cin >> point_count >> input_dimension >> output_dimension >> seed)) return 0;
    std::vector<std::vector<double>> points(
        point_count, std::vector<double>(input_dimension)
    );
    for(auto& point: points) for(double& value: point) std::cin >> value;

    std::mt19937_64 first_random(seed);
    std::mt19937_64 second_random(seed);
    const auto projection = approximate::randomized::rademacher_projection_matrix(
        input_dimension, output_dimension, first_random
    );
    const auto repeated_projection = approximate::randomized::rademacher_projection_matrix(
        input_dimension, output_dimension, second_random
    );
    assert(projection == repeated_projection);
    const double expected_absolute = 1.0 / std::sqrt(static_cast<double>(output_dimension));
    for(const auto& row: projection){
        assert(row.size() == input_dimension);
        for(double value: row) assert(std::abs(std::abs(value) - expected_absolute) < 1.0e-15);
    }

    const auto applied = approximate::randomized::apply_random_projection(points, projection);
    std::mt19937_64 transform_random(seed);
    const auto transformed = approximate::randomized::johnson_lindenstrauss_transform(
        points, output_dimension, transform_random
    );
    assert(applied == transformed);
    assert(applied.size() == point_count);
    for(const auto& point: applied) assert(point.size() == output_dimension);

    if(point_count >= 3){
        std::vector<double> sum(input_dimension);
        for(std::size_t coordinate = 0; coordinate < input_dimension; ++coordinate){
            sum[coordinate] = points[0][coordinate] + points[1][coordinate];
        }
        const auto projected_sum = approximate::randomized::apply_random_projection(
            std::vector<std::vector<double>>{sum}, projection
        );
        for(std::size_t coordinate = 0; coordinate < output_dimension; ++coordinate){
            const double expected = applied[0][coordinate] + applied[1][coordinate];
            assert(std::abs(projected_sum[0][coordinate] - expected) < 1.0e-10);
        }
    }

    std::mt19937_64 fixed_random(seed + 1U);
    const auto fixed_projection = approximate::randomized::rademacher_projection_matrix<3, 2>(
        fixed_random
    );
    const std::array<std::array<double, 3>, 2> fixed_points{{{{1.0, 2.0, 3.0}},
                                                            {{-2.0, 0.5, 4.0}}}};
    const auto fixed_result = approximate::randomized::apply_random_projection(
        fixed_points, fixed_projection
    );
    for(std::size_t point = 0; point < fixed_points.size(); ++point){
        for(std::size_t output = 0; output < fixed_projection.size(); ++output){
            double expected = 0.0;
            for(std::size_t input = 0; input < fixed_points[point].size(); ++input){
                expected += fixed_points[point][input] * fixed_projection[output][input];
            }
            assert(std::abs(fixed_result[point][output] - expected) < 1.0e-12);
        }
    }

    std::mt19937_64 empty_random(seed + 2U);
    const auto empty = approximate::randomized::johnson_lindenstrauss_transform(
        {}, 3, empty_random
    );
    assert(empty.empty());
    const auto zero_input_projection = approximate::randomized::rademacher_projection_matrix(
        0, 4, empty_random
    );
    const auto zero_input = approximate::randomized::apply_random_projection(
        std::vector<std::vector<double>>{{}, {}}, zero_input_projection
    );
    assert(zero_input.size() == 2);
    assert(zero_input[0] == std::vector<double>(4, 0.0));
    assert(zero_input[1] == std::vector<double>(4, 0.0));

    const std::size_t dimension = approximate::randomized::johnson_lindenstrauss_dimension(
        100, 0.5, 0.1
    );
    assert(dimension > 0);

    bool shape_thrown = false;
    try{
        static_cast<void>(approximate::randomized::apply_random_projection(
            std::vector<std::vector<double>>{{1.0, 2.0}},
            std::vector<std::vector<double>>{{1.0}, {1.0, 2.0}}
        ));
    }catch(const std::invalid_argument&){
        shape_thrown = true;
    }
    assert(shape_thrown);

    bool nonfinite_thrown = false;
    try{
        static_cast<void>(approximate::randomized::apply_random_projection(
            std::vector<std::vector<double>>{{std::numeric_limits<double>::infinity()}},
            std::vector<std::vector<double>>{{1.0}}
        ));
    }catch(const std::invalid_argument&){
        nonfinite_thrown = true;
    }
    assert(nonfinite_thrown);

    bool overflow_thrown = false;
    try{
        static_cast<void>(approximate::randomized::apply_random_projection(
            std::vector<std::vector<double>>{{std::numeric_limits<double>::max()}},
            std::vector<std::vector<double>>{{2.0}}
        ));
    }catch(const std::overflow_error&){
        overflow_thrown = true;
    }
    assert(overflow_thrown);

    bool output_dimension_thrown = false;
    try{
        std::mt19937_64 random(1);
        static_cast<void>(approximate::randomized::rademacher_projection_matrix(2, 0, random));
    }catch(const std::invalid_argument&){
        output_dimension_thrown = true;
    }
    assert(output_dimension_thrown);

    bool epsilon_thrown = false;
    try{
        static_cast<void>(approximate::randomized::johnson_lindenstrauss_dimension(
            1, std::numeric_limits<double>::quiet_NaN(), 0.1
        ));
    }catch(const std::invalid_argument&){
        epsilon_thrown = true;
    }
    assert(epsilon_thrown);

    std::cout << point_count << ' ' << output_dimension << '\n';
}
