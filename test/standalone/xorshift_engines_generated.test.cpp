// competitive-verifier: STANDALONE

#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/random/xorshift.hpp"
#include "../../src/algorithm/random/xorshift_cracker.hpp"

namespace{

template<class Function>
bool throws_invalid_argument(Function&& function){
    try{
        function();
    }catch(const std::invalid_argument&){
        return true;
    }catch(...){
    }
    return false;
}

}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    if(!throws_invalid_argument([]{ (void)XorShift32(0); })
       || !throws_invalid_argument([]{ (void)XorShift64(0); })
       || !throws_invalid_argument([]{
            (void)XorShift128(XorShift128::state_type{});
       })
       || recover_xorshift32_seed(0) != 0
       || recover_xorshift64_seed(0) != 0){
        return 1;
    }

    int case_count;
    if(!(std::cin >> case_count)) return 1;
    for(int test_case = 0; test_case < case_count; ++test_case){
        std::uint32_t seed32;
        std::uint64_t seed64;
        XorShift128::state_type seed128{};
        unsigned int skip32;
        unsigned int skip64;
        unsigned int skip128;
        int prediction_count;
        if(!(std::cin >> seed32 >> skip32 >> seed64 >> skip64
             >> seed128[0] >> seed128[1] >> seed128[2] >> seed128[3]
             >> skip128 >> prediction_count)){
            return 1;
        }

        XorShift32 engine32(seed32);
        XorShift64 engine64(seed64);
        XorShift128 engine128(seed128);
        for(unsigned int index = 0; index < skip32; ++index) (void)engine32();
        for(unsigned int index = 0; index < skip64; ++index) (void)engine64();
        for(unsigned int index = 0; index < skip128; ++index) (void)engine128();

        const auto observation_state32 = engine32.state();
        const auto observation_state64 = engine64.state();
        const auto observation_state128 = engine128.state();
        const auto first32 = engine32();
        const auto first64 = engine64();
        std::array<std::uint32_t, 4> observations128{};
        for(auto& output: observations128) output = engine128();

        const auto recovered32 = recover_xorshift32_seed(first32);
        const auto recovered64 = recover_xorshift64_seed(first64);
        XorShift128Cracker cracker128(observations128);
        if(recovered32 != observation_state32
           || recovered64 != observation_state64
           || cracker128.initial_state() != observation_state128
           || cracker128.current_state() != observations128){
            return 1;
        }

        std::cout << recovered32 << ' ' << recovered64;
        for(const auto value: cracker128.initial_state()){
            std::cout << ' ' << value;
        }
        std::cout << '\n';

        XorShift32 predictor32(first32);
        XorShift64 predictor64(first64);
        for(int query = 0; query < prediction_count; ++query){
            const auto predicted32 = predictor32();
            const auto predicted64 = predictor64();
            const auto predicted128 = cracker128.next();
            if(predicted32 != engine32()
               || predicted64 != engine64()
               || predicted128 != engine128()){
                return 1;
            }
            std::cout << predicted32 << ' ' << predicted64 << ' '
                      << predicted128 << '\n';
        }
    }
    return 0;
}
