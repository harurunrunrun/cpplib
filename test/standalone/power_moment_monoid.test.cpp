// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

#include "../../src/structure/bbst/reversible_splay_tree.hpp"
#include "../../src/structure/types/power_moment_monoid.hpp"

namespace{

constexpr std::size_t max_power = 10;
constexpr PowerMomentMonoid<std::uint32_t, max_power> power_moment_monoid{};
using Aggregate = PowerMomentAggregate<std::uint32_t, max_power>;

constexpr auto compile_time_left =
    power_moment_singleton<std::uint32_t, max_power>(3);
constexpr auto compile_time_right =
    power_moment_singleton<std::uint32_t, max_power>(5);
constexpr auto compile_time_combined =
    power_moment_monoid.op(compile_time_left, compile_time_right);
constexpr auto compile_time_third =
    power_moment_singleton<std::uint32_t, max_power>(
        std::numeric_limits<std::uint32_t>::max()
    );

static_assert(compile_time_combined.length == 2);
static_assert(compile_time_combined.moment[0] == 8);
static_assert(compile_time_combined.moment[1] == 13);
static_assert(compile_time_combined.moment[2] == 23);
static_assert(
    power_moment_monoid.op(
        power_moment_monoid.op(compile_time_left, compile_time_right),
        compile_time_third
    ) == power_moment_monoid.op(
        compile_time_left,
        power_moment_monoid.op(compile_time_right, compile_time_third)
    )
);
static_assert(
    power_moment_monoid.op(power_moment_monoid.e(), compile_time_combined)
    == compile_time_combined
);
static_assert(
    power_moment_monoid.op(compile_time_combined, power_moment_monoid.e())
    == compile_time_combined
);

Aggregate naive_aggregate(
    const std::vector<std::uint32_t>& values,
    std::size_t left,
    std::size_t right
){
    Aggregate result;
    result.length = right - left;
    for(std::size_t index = left; index < right; ++index){
        const std::uint32_t position =
            static_cast<std::uint32_t>(index - left + 1);
        std::uint32_t position_power = 1;
        for(std::size_t power = 0; power <= max_power; ++power){
            result.moment[power] += values[index] * position_power;
            position_power *= position;
        }
    }
    return result;
}

Aggregate fold_aggregate(
    const std::vector<std::uint32_t>& values,
    std::size_t left,
    std::size_t right
){
    Aggregate result = power_moment_monoid.e();
    for(std::size_t index = left; index < right; ++index){
        result = power_moment_monoid.op(
            result,
            power_moment_singleton<std::uint32_t, max_power>(values[index])
        );
    }
    return result;
}

void check_direct_contract(){
    const auto maximum = std::numeric_limits<std::uint32_t>::max();
    const std::vector<std::uint32_t> boundary = {
        0, 1, maximum, std::uint32_t{1} << 31, maximum - 1
    };
    assert(fold_aggregate(boundary, 0, boundary.size())
        == naive_aggregate(boundary, 0, boundary.size()));
    assert(power_moment_monoid.op(
        power_moment_singleton<std::uint32_t, max_power>(maximum),
        power_moment_singleton<std::uint32_t, max_power>(1)
    ).moment[0] == 0);

    std::mt19937 random(2026071401);
    for(int trial = 0; trial < 500; ++trial){
        const std::size_t first_size = random() % 8;
        const std::size_t second_size = random() % 8;
        const std::size_t third_size = random() % 8;
        std::vector<std::uint32_t> values(
            first_size + second_size + third_size
        );
        for(auto& value: values){
            value = static_cast<std::uint32_t>(random());
        }
        if(!values.empty()) values[0] = maximum;

        const auto first = fold_aggregate(values, 0, first_size);
        const auto second = fold_aggregate(
            values,
            first_size,
            first_size + second_size
        );
        const auto third = fold_aggregate(
            values,
            first_size + second_size,
            values.size()
        );
        const auto left_grouped = power_moment_monoid.op(
            power_moment_monoid.op(first, second),
            third
        );
        const auto right_grouped = power_moment_monoid.op(
            first,
            power_moment_monoid.op(second, third)
        );
        assert(left_grouped == right_grouped);
        assert(left_grouped == naive_aggregate(values, 0, values.size()));
        assert(power_moment_monoid.op(power_moment_monoid.e(), first) == first);
        assert(power_moment_monoid.op(first, power_moment_monoid.e()) == first);
    }
}

std::uint32_t read_value(){
    std::uint64_t value;
    std::cin >> value;
    assert(value <= std::numeric_limits<std::uint32_t>::max());
    return static_cast<std::uint32_t>(value);
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    check_direct_contract();

    int size, query_count;
    if(!(std::cin >> size >> query_count)) return 0;
    std::vector<Aggregate> initial(static_cast<std::size_t>(size));
    for(auto& value: initial){
        value = power_moment_singleton<std::uint32_t, max_power>(read_value());
    }

    ReversibleSplayTree<power_moment_monoid, 200005> sequence(initial);
    while(query_count-- != 0){
        std::string operation;
        std::cin >> operation;
        if(operation == "INSERT"){
            int position;
            std::cin >> position;
            sequence.insert(
                position,
                power_moment_singleton<std::uint32_t, max_power>(read_value())
            );
        }else if(operation == "ERASE"){
            int position;
            std::cin >> position;
            sequence.erase(position);
        }else if(operation == "SET"){
            int position;
            std::cin >> position;
            sequence.set(
                position,
                power_moment_singleton<std::uint32_t, max_power>(read_value())
            );
        }else if(operation == "REVERSE"){
            int left, right;
            std::cin >> left >> right;
            sequence.reverse(left, right);
        }else if(operation == "QUERY"){
            int left, right;
            std::size_t power;
            std::cin >> left >> right >> power;
            assert(power <= max_power);
            std::cout << sequence.prod(left, right).moment[power] << '\n';
        }else if(operation == "ALL"){
            std::size_t power;
            std::cin >> power;
            assert(power <= max_power);
            std::cout << sequence.all_prod().moment[power] << '\n';
        }else if(operation == "SIZE"){
            std::cout << sequence.size() << '\n';
        }else{
            assert(false);
        }
    }
}
