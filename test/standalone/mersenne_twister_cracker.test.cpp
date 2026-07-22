// competitive-verifier: STANDALONE

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../../src/algorithm/random/crack/mersenne_twister_cracker.hpp"

namespace{

template<class Cracker, std::size_t Count>
int run_case(bool corrupted){
    using result_type = typename Cracker::result_type;
    std::array<result_type, Count> outputs{};
    for(auto& value: outputs){
        if(!(std::cin >> value)) return 1;
    }
    int prediction_count;
    if(!(std::cin >> prediction_count)) return 1;

    try{
        Cracker cracker(outputs);
        if(corrupted){
            std::cout << "accepted\n";
            return 0;
        }

        Cracker jumped = cracker;
        Cracker stepped = cracker;
        jumped.discard(37);
        for(int index = 0; index < 37; ++index) (void)stepped();
        if(jumped.state() != stepped.state() || jumped() != stepped()){
            return 1;
        }

        for(int index = 0; index < prediction_count; ++index){
            std::cout << cracker() << '\n';
        }
    }catch(const std::invalid_argument&){
        std::cout << "invalid\n";
    }
    return 0;
}

}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string mode;
    if(!(std::cin >> mode)) return 1;
    if(mode == "mt32_min") return run_case<Mt19937Cracker, 624>(false);
    if(mode == "mt32_extra") return run_case<Mt19937Cracker, 640>(false);
    if(mode == "mt32_corrupt") return run_case<Mt19937Cracker, 625>(true);
    if(mode == "mt64_min") return run_case<Mt19937_64Cracker, 312>(false);
    if(mode == "mt64_extra") return run_case<Mt19937_64Cracker, 320>(false);
    if(mode == "mt64_corrupt"){
        return run_case<Mt19937_64Cracker, 313>(true);
    }
    return 1;
}
