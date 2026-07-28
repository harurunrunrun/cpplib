// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <optional>
#include <random>
#include <vector>

#include "../../src/algorithm/other/dynamic_programming/bitset/four_russians_boolean_matrix.hpp"
#include "../../src/algorithm/other/dynamic_programming/digit/digit_dp.hpp"
#include "../../src/algorithm/other/dynamic_programming/profile_grid/broken_profile_dp.hpp"
#include "../../src/algorithm/other/dynamic_programming/profile_grid/plug_dp.hpp"

namespace {

std::vector<int> digits(std::uint64_t value) {
    std::vector<int> result(8);
    for(std::size_t offset = 0; offset < result.size(); ++offset){
        result[result.size() - 1 - offset] =
            static_cast<int>(value % 10);
        value /= 10;
    }
    return result;
}

std::uint64_t brute_digit_count(std::uint64_t upper) {
    std::uint64_t result = 0;
    for(std::uint64_t value = 0; value <= upper; ++value){
        std::uint64_t copy = value;
        int sum = 0;
        do{
            sum += static_cast<int>(copy % 10);
            copy /= 10;
        }while(copy != 0);
        if(sum % 7 == 3) ++result;
    }
    return result;
}

dynamic_programming::BooleanMatrix naive_boolean_product(
    const dynamic_programming::BooleanMatrix& first,
    const dynamic_programming::BooleanMatrix& second
) {
    dynamic_programming::BooleanMatrix result(
        first.size(),
        std::vector<unsigned char>(
            second.empty() ? 0 : second.front().size()
        )
    );
    for(std::size_t row = 0; row < first.size(); ++row){
        for(std::size_t middle = 0; middle < second.size(); ++middle){
            for(std::size_t column = 0;
                column < result[row].size();
                ++column){
                result[row][column] |=
                    first[row][middle] & second[middle][column];
            }
        }
    }
    return result;
}

}  // namespace

int main() {
    std::uint64_t seed;
    int rounds;
    if(!(std::cin >> seed >> rounds)) return 0;
    std::mt19937_64 random(seed);

    const auto transition = [](
        int state,
        int digit,
        bool,
        bool,
        std::size_t
    ) -> std::optional<int> {
        return (state + digit) % 7;
    };
    const auto accept = [](int state, bool){ return state == 3; };
    for(int round = 0; round < rounds; ++round){
        const std::uint64_t upper = random() % 100000;
        const auto counted =
            dynamic_programming::digit_dp_count<std::uint64_t>(
                digits(upper), 10, 0, transition, accept
            );
        assert(counted == brute_digit_count(upper));

        const std::size_t rows = 1 + random() % 15;
        const std::size_t shared = 1 + random() % 15;
        const std::size_t columns = 1 + random() % 15;
        dynamic_programming::BooleanMatrix first(
            rows, std::vector<unsigned char>(shared)
        );
        dynamic_programming::BooleanMatrix second(
            shared, std::vector<unsigned char>(columns)
        );
        for(auto& row : first){
            for(auto& value : row) value = random() & 1U;
        }
        for(auto& row : second){
            for(auto& value : row) value = random() & 1U;
        }
        assert(
            dynamic_programming::four_russians_boolean_matrix_multiply(
                first, second
            )
            == naive_boolean_product(first, second)
        );
    }

    assert(
        dynamic_programming::count_domino_tilings_broken_profile<>(
            2, 10
        ) == 89
    );
    assert(
        dynamic_programming::count_domino_tilings_broken_profile<>(
            3, 4
        ) == 11
    );

    dynamic_programming::PlugDpState state({7, 0, 7, 12, 12});
    assert((state.plugs() == std::vector<unsigned int>{1, 0, 1, 2, 2}));
    state.connect(1, 2);
    assert((state.plugs() == std::vector<unsigned int>{1, 0, 1, 1, 1}));
    const auto binary_labelings =
        dynamic_programming::plug_dp_grid<std::uint64_t>(
            3,
            4,
            dynamic_programming::PlugDpState(5),
            [](
                std::size_t,
                std::size_t,
                const dynamic_programming::PlugDpState& current,
                const auto& emit
            ){
                emit(current, std::uint64_t{1});
                emit(current, std::uint64_t{1});
            },
            [](const dynamic_programming::PlugDpState&){ return true; }
        );
    assert(binary_labelings == (std::uint64_t{1} << 12));
    std::cout << "OK\n";
}
