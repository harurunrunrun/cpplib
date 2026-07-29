// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>

#include "../../src/approximate/search/multi_armed_bandit.hpp"

void fixed_tests(){
    Ucb1Bandit<int> ucb(2);
    assert(ucb.select_arm() == 0);
    ucb.update(0, 1);
    assert(ucb.select_arm() == 1);
    ucb.update(1, 0);
    assert(ucb.select_arm() == 0);
    assert(ucb.total_pulls() == 2);
    assert(ucb.pulls(0) == 1);
    assert(std::abs(ucb.mean_reward(0) - 1.0L) < 1e-18L);

    UcbTunedBandit<int> tuned(2);
    tuned.update(0, 1);
    tuned.update(1, 0);
    assert(tuned.select_arm() == 0);

    BernoulliThompsonSamplingBandit thompson(2);
    thompson.update(0, true);
    thompson.update(0, false);
    assert(thompson.posterior_alpha(0) == 2.0L);
    assert(thompson.posterior_beta(0) == 2.0L);
    std::mt19937_64 generator(123456789);
    assert(thompson.select_arm(generator) < 2);

    bool threw = false;
    try{
        static_cast<void>(Ucb1Bandit<int>(0));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
    threw = false;
    try{
        static_cast<void>(ucb1_score(0.0L, 0, 0));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
}

int main(){
    fixed_tests();

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    for(int case_index = 0; case_index < test_count; ++case_index){
        std::size_t arm_count;
        int update_count;
        std::cin >> arm_count >> update_count;
        Ucb1Bandit<int> ucb(arm_count);
        UcbTunedBandit<int> tuned(arm_count);
        BernoulliThompsonSamplingBandit thompson(arm_count);
        for(int i = 0; i < update_count; ++i){
            std::size_t arm;
            int reward;
            std::cin >> arm >> reward;
            ucb.update(arm, reward);
            tuned.update(arm, reward);
            thompson.update(arm, reward != 0);
        }
        std::cout << ucb.select_arm() << ' ' << tuned.select_arm();
        for(std::size_t arm = 0; arm < arm_count; ++arm){
            std::cout << ' '
                      << static_cast<long long>(
                             thompson.posterior_alpha(arm)
                         )
                      << ' '
                      << static_cast<long long>(
                             thompson.posterior_beta(arm)
                         );
        }
        std::cout << '\n';
    }
}
