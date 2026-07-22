// competitive-verifier: STANDALONE

#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/algorithm/random/crack/linear_congruential_cracker.hpp"
#include "../../src/algorithm/random/engine/linear_congruential_generator.hpp"

namespace{

using PrimeLcg = LinearCongruentialGenerator<
    std::uint64_t,
    6364136223846793005ULL,
    1442695040888963407ULL,
    18446744073709551557ULL
>;

using WrapLcg = LinearCongruentialGenerator<
    std::uint64_t,
    6364136223846793005ULL,
    1442695040888963407ULL,
    0ULL
>;

using NoninvertibleLcg = LinearCongruentialGenerator<
    std::uint64_t,
    6ULL,
    5ULL,
    12ULL
>;

constexpr std::array<std::uint32_t, 8> small_unknown_outputs{
    0, 1, 4, 6, 5, 2, 0, 1,
};
constexpr auto small_unknown_candidate =
    recover_linear_congruential_unknown_modulus_candidate(
        small_unknown_outputs
    );
static_assert(small_unknown_candidate);
static_assert(small_unknown_candidate->modulus == 7);
static_assert(small_unknown_candidate->multiplier == 3);
static_assert(small_unknown_candidate->increment == 1);
static_assert(small_unknown_candidate->next(1) == 4);

constexpr std::array<std::uint64_t, 3> out_of_range_outputs{
    0,
    PrimeLcg::modulus,
    1,
};
static_assert(!recover_linear_congruential_parameters<
    std::uint64_t,
    PrimeLcg::modulus
>(out_of_range_outputs));

template<class Engine>
int run_known_engine(){
    using result_type = typename Engine::result_type;
    result_type seed;
    unsigned int skip;
    int prediction_count;
    if(!(std::cin >> seed >> skip >> prediction_count)) return 1;

    Engine engine(seed);
    for(unsigned int index = 0; index < skip; ++index) (void)engine();
    const result_type observation_state = engine.state();
    const result_type first_output = engine();
    LinearCongruentialCracker<Engine> cracker(first_output);
    if(cracker.current_output() != first_output) return 1;
    const auto recovered = cracker.recovered_seed();
    if(!recovered || *recovered != observation_state) return 1;

    std::array<result_type, 8> observations{};
    Engine parameter_engine(observation_state);
    for(auto& output: observations) output = parameter_engine();
    const auto parameters =
        recover_linear_congruential_parameters<
            result_type,
            Engine::modulus
        >(observations);
    if(!parameters
       || parameters->multiplier != Engine::multiplier
       || parameters->increment != Engine::increment){
        return 1;
    }

    std::cout << *recovered << ' ' << parameters->multiplier << ' '
              << parameters->increment << '\n';
    for(int query = 0; query < prediction_count; ++query){
        const result_type predicted = (query & 1) == 0 ? cracker() : cracker.next();
        if(predicted != engine()) return 1;
        if(cracker.current_output() != predicted) return 1;
        std::cout << predicted << '\n';
    }
    return 0;
}

int run_noninvertible(){
    std::uint64_t latest_output;
    int prediction_count;
    if(!(std::cin >> latest_output >> prediction_count)) return 1;
    LinearCongruentialCracker<NoninvertibleLcg> cracker(latest_output);
    if(cracker.recovered_seed()) return 1;
    if(cracker.current_output() != latest_output) return 1;
    std::cout << "none\n";
    for(int query = 0; query < prediction_count; ++query){
        const auto predicted = (query & 1) == 0 ? cracker() : cracker.next();
        if(cracker.current_output() != predicted) return 1;
        std::cout << predicted << '\n';
    }
    return 0;
}

int run_invalid_cracker(){
    std::uint64_t output;
    if(!(std::cin >> output)) return 1;
    try{
        (void)LinearCongruentialCracker<PrimeLcg>(output);
    }catch(const std::invalid_argument&){
        std::cout << "invalid\n";
        return 0;
    }
    return 1;
}

int run_prime_parameters(){
    std::array<std::uint64_t, 8> observations{};
    for(auto& output: observations){
        if(!(std::cin >> output)) return 1;
    }
    const auto result = recover_linear_congruential_parameters<
        std::uint64_t,
        PrimeLcg::modulus
    >(observations);
    if(!result){
        std::cout << "none\n";
    }else{
        std::cout << result->multiplier << ' ' << result->increment << '\n';
    }
    return 0;
}

int run_unknown_modulus(){
    std::size_t count;
    if(!(std::cin >> count)) return 1;
    std::vector<std::uint64_t> observations(count);
    for(auto& output: observations){
        if(!(std::cin >> output)) return 1;
    }
    const auto result =
        recover_linear_congruential_unknown_modulus_candidate<std::uint64_t>(
            std::span<const std::uint64_t>(observations)
        );
    if(!result){
        std::cout << "none\n";
    }else{
        std::cout << result->modulus << ' ' << result->multiplier << ' '
                  << result->increment << '\n';
        std::cout << result->next(observations.back()) << '\n';
    }
    return 0;
}

}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string mode;
    if(!(std::cin >> mode)) return 1;
    if(mode == "prime") return run_known_engine<PrimeLcg>();
    if(mode == "wrap") return run_known_engine<WrapLcg>();
    if(mode == "noninvertible") return run_noninvertible();
    if(mode == "invalid_cracker") return run_invalid_cracker();
    if(mode == "prime_parameters") return run_prime_parameters();
    if(mode == "unknown") return run_unknown_modulus();
    return 1;
}
