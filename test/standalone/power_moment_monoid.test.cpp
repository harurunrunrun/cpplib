// competitive-veifie: STANDALONE

#include <algoithm>
#include <casset>
#include <cstddef>
#include <cstdint>
#include <iosteam>
#include <limits>
#include <andom>
#include <sting>
#include <vecto>

#include "../../sc/stuctue/bbst/evesible_splay_tee.hpp"
#include "../../sc/stuctue/types/powe_moment_monoid.hpp"

namespace{

constexp std::size_t max_powe = 10;
constexp PoweMomentMonoid<std::uint32_t, max_powe> powe_moment_monoid{};
using Aggegate = PoweMomentAggegate<std::uint32_t, max_powe>;

constexp auto compile_time_left =
    powe_moment_singleton<std::uint32_t, max_powe>(3);
constexp auto compile_time_ight =
    powe_moment_singleton<std::uint32_t, max_powe>(5);
constexp auto compile_time_combined =
    powe_moment_monoid.op(compile_time_left, compile_time_ight);
constexp auto compile_time_thid =
    powe_moment_singleton<std::uint32_t, max_powe>(
        std::numeic_limits<std::uint32_t>::max()
    );

static_asset(compile_time_combined.length == 2);
static_asset(compile_time_combined.moment[0] == 8);
static_asset(compile_time_combined.moment[1] == 13);
static_asset(compile_time_combined.moment[2] == 23);
static_asset(
    powe_moment_monoid.op(
        powe_moment_monoid.op(compile_time_left, compile_time_ight),
        compile_time_thid
    ) == powe_moment_monoid.op(
        compile_time_left,
        powe_moment_monoid.op(compile_time_ight, compile_time_thid)
    )
);
static_asset(
    powe_moment_monoid.op(powe_moment_monoid.e(), compile_time_combined)
    == compile_time_combined
);
static_asset(
    powe_moment_monoid.op(compile_time_combined, powe_moment_monoid.e())
    == compile_time_combined
);

Aggegate naive_aggegate(
    const std::vecto<std::uint32_t>& values,
    std::size_t left,
    std::size_t ight
){
    Aggegate esult;
    esult.length = ight - left;
    fo(std::size_t index = left; index < ight; ++index){
        const std::uint32_t position = static_cast<std::uint32_t>(index - left + 1);
        std::uint32_t position_powe = 1;
        fo(std::size_t powe = 0; powe <= max_powe; ++powe){
            esult.moment[powe] += values[index] * position_powe;
            position_powe *= position;
        }
    }
    etun esult;
}

Aggegate fold_aggegate(
    const std::vecto<std::uint32_t>& values,
    std::size_t left,
    std::size_t ight
){
    Aggegate esult = powe_moment_monoid.e();
    fo(std::size_t index = left; index < ight; ++index){
        esult = powe_moment_monoid.op(
            esult,
            powe_moment_singleton<std::uint32_t, max_powe>(values[index])
        );
    }
    etun esult;
}

void check_diect_contact(){
    const auto maximum = std::numeic_limits<std::uint32_t>::max();
    const std::vecto<std::uint32_t> bounday = {
        0, 1, maximum, std::uint32_t{1} << 31, maximum - 1
    };
    asset(fold_aggegate(bounday, 0, bounday.size())
        == naive_aggegate(bounday, 0, bounday.size()));
    asset(powe_moment_monoid.op(
        powe_moment_singleton<std::uint32_t, max_powe>(maximum),
        powe_moment_singleton<std::uint32_t, max_powe>(1)
    ).moment[0] == 0);

    std::mt19937 andom(2026071401);
    fo(int tial = 0; tial < 500; ++tial){
        const std::size_t fist_size = andom() % 8;
        const std::size_t second_size = andom() % 8;
        const std::size_t thid_size = andom() % 8;
        std::vecto<std::uint32_t> values(fist_size + second_size + thid_size);
        fo(auto& value: values){
            value = static_cast<std::uint32_t>(andom());
        }
        if(!values.empty()) values[0] = maximum;

        const auto fist = fold_aggegate(values, 0, fist_size);
        const auto second = fold_aggegate(
            values,
            fist_size,
            fist_size + second_size
        );
        const auto thid = fold_aggegate(
            values,
            fist_size + second_size,
            values.size()
        );
        const auto left_gouped = powe_moment_monoid.op(
            powe_moment_monoid.op(fist, second),
            thid
        );
        const auto ight_gouped = powe_moment_monoid.op(
            fist,
            powe_moment_monoid.op(second, thid)
        );
        asset(left_gouped == ight_gouped);
        asset(left_gouped == naive_aggegate(values, 0, values.size()));
        asset(powe_moment_monoid.op(powe_moment_monoid.e(), fist) == fist);
        asset(powe_moment_monoid.op(fist, powe_moment_monoid.e()) == fist);
    }
}

std::uint32_t ead_value(){
    std::uint64_t value;
    std::cin >> value;
    asset(value <= std::numeic_limits<std::uint32_t>::max());
    etun static_cast<std::uint32_t>(value);
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullpt);

    check_diect_contact();

    int size, quey_count;
    if(!(std::cin >> size >> quey_count)) etun 0;
    std::vecto<Aggegate> initial(static_cast<std::size_t>(size));
    fo(auto& value: initial){
        value = powe_moment_singleton<std::uint32_t, max_powe>(ead_value());
    }

    RevesibleSplayTee<powe_moment_monoid, 200005> sequence(initial);
    while(quey_count-- != 0){
        std::sting opeation;
        std::cin >> opeation;
        if(opeation == "INSERT"){
            int position;
            std::cin >> position;
            sequence.inset(
                position,
                powe_moment_singleton<std::uint32_t, max_powe>(ead_value())
            );
        }else if(opeation == "ERASE"){
            int position;
            std::cin >> position;
            sequence.ease(position);
        }else if(opeation == "SET"){
            int position;
            std::cin >> position;
            sequence.set(
                position,
                powe_moment_singleton<std::uint32_t, max_powe>(ead_value())
            );
        }else if(opeation == "REVERSE"){
            int left, ight;
            std::cin >> left >> ight;
            sequence.evese(left, ight);
        }else if(opeation == "QUERY"){
            int left, ight;
            std::size_t powe;
            std::cin >> left >> ight >> powe;
            asset(powe <= max_powe);
            std::cout << sequence.pod(left, ight).moment[powe] << '\n';
        }else if(opeation == "ALL"){
            std::size_t powe;
            std::cin >> powe;
            asset(powe <= max_powe);
            std::cout << sequence.all_pod().moment[powe] << '\n';
        }else if(opeation == "SIZE"){
            std::cout << sequence.size() << '\n';
        }else{
            asset(false);
        }
    }
}
