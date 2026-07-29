#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_MULTI_ARMED_BANDIT_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_MULTI_ARMED_BANDIT_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <random>
#include <stdexcept>
#include <vector>

namespace multi_armed_bandit_internal{

inline void require_arm_count(std::size_t arm_count){
    if(arm_count == 0)[[unlikely]]{
        throw std::invalid_argument("a bandit requires at least one arm");
    }
}

inline void require_arm(std::size_t arm, std::size_t arm_count){
    if(arm >= arm_count)[[unlikely]]{
        throw std::out_of_range("bandit arm index is out of range");
    }
}

template<class Reward>
long double checked_reward(const Reward& reward){
    const long double value = static_cast<long double>(reward);
    if(!std::isfinite(value))[[unlikely]]{
        throw std::invalid_argument("bandit reward must be finite");
    }
    return value;
}

} // namespace multi_armed_bandit_internal

[[nodiscard]] inline long double ucb1_score(
    long double reward_sum,
    std::size_t pulls,
    std::size_t total_pulls
){
    if(pulls == 0)[[unlikely]]{
        throw std::invalid_argument("ucb1_score requires a positive pull count");
    }
    if(total_pulls < pulls)[[unlikely]]{
        throw std::invalid_argument(
            "ucb1_score requires total_pulls >= pulls"
        );
    }
    const long double mean = reward_sum / static_cast<long double>(pulls);
    const long double exploration = std::sqrt(
        2.0L * std::log(static_cast<long double>(
            std::max<std::size_t>(total_pulls, 1)
        )) / static_cast<long double>(pulls)
    );
    return mean + exploration;
}

template<class Reward = long double>
class Ucb1Bandit{
public:
    explicit Ucb1Bandit(std::size_t arm_count)
        : pulls_(arm_count), reward_sums_(arm_count){
        multi_armed_bandit_internal::require_arm_count(arm_count);
    }

    [[nodiscard]] std::size_t arm_count() const noexcept{
        return pulls_.size();
    }

    [[nodiscard]] std::size_t total_pulls() const noexcept{
        return total_pulls_;
    }

    [[nodiscard]] std::size_t pulls(std::size_t arm) const{
        multi_armed_bandit_internal::require_arm(arm, arm_count());
        return pulls_[arm];
    }

    [[nodiscard]] long double mean_reward(std::size_t arm) const{
        multi_armed_bandit_internal::require_arm(arm, arm_count());
        return pulls_[arm] == 0
            ? 0.0L
            : reward_sums_[arm] / static_cast<long double>(pulls_[arm]);
    }

    [[nodiscard]] std::size_t select_arm() const{
        for(std::size_t arm = 0; arm < arm_count(); ++arm){
            if(pulls_[arm] == 0) return arm;
        }
        std::size_t best = 0;
        long double best_score = ucb1_score(
            reward_sums_[0], pulls_[0], total_pulls_
        );
        for(std::size_t arm = 1; arm < arm_count(); ++arm){
            const long double score = ucb1_score(
                reward_sums_[arm], pulls_[arm], total_pulls_
            );
            if(best_score < score){
                best = arm;
                best_score = score;
            }
        }
        return best;
    }

    void update(std::size_t arm, const Reward& reward){
        multi_armed_bandit_internal::require_arm(arm, arm_count());
        const long double value =
            multi_armed_bandit_internal::checked_reward(reward);
        const long double sum = reward_sums_[arm] + value;
        if(!std::isfinite(sum))[[unlikely]]{
            throw std::overflow_error("UCB1 reward sum is not finite");
        }
        reward_sums_[arm] = sum;
        ++pulls_[arm];
        ++total_pulls_;
    }

private:
    std::vector<std::size_t> pulls_;
    std::vector<long double> reward_sums_;
    std::size_t total_pulls_ = 0;
};

template<class Reward = long double>
class UcbTunedBandit{
public:
    explicit UcbTunedBandit(std::size_t arm_count)
        : pulls_(arm_count), reward_sums_(arm_count),
          squared_reward_sums_(arm_count){
        multi_armed_bandit_internal::require_arm_count(arm_count);
    }

    [[nodiscard]] std::size_t arm_count() const noexcept{
        return pulls_.size();
    }

    [[nodiscard]] std::size_t total_pulls() const noexcept{
        return total_pulls_;
    }

    [[nodiscard]] std::size_t pulls(std::size_t arm) const{
        multi_armed_bandit_internal::require_arm(arm, arm_count());
        return pulls_[arm];
    }

    [[nodiscard]] long double mean_reward(std::size_t arm) const{
        multi_armed_bandit_internal::require_arm(arm, arm_count());
        return pulls_[arm] == 0
            ? 0.0L
            : reward_sums_[arm] / static_cast<long double>(pulls_[arm]);
    }

    [[nodiscard]] std::size_t select_arm() const{
        for(std::size_t arm = 0; arm < arm_count(); ++arm){
            if(pulls_[arm] == 0) return arm;
        }
        const long double logarithm = std::log(
            static_cast<long double>(std::max<std::size_t>(total_pulls_, 1))
        );
        std::size_t best = 0;
        long double best_score = score(0, logarithm);
        for(std::size_t arm = 1; arm < arm_count(); ++arm){
            const long double candidate = score(arm, logarithm);
            if(best_score < candidate){
                best = arm;
                best_score = candidate;
            }
        }
        return best;
    }

    void update(std::size_t arm, const Reward& reward){
        multi_armed_bandit_internal::require_arm(arm, arm_count());
        const long double value =
            multi_armed_bandit_internal::checked_reward(reward);
        if(value < 0.0L || 1.0L < value)[[unlikely]]{
            throw std::invalid_argument(
                "UCB-Tuned requires rewards in [0, 1]"
            );
        }
        const long double sum = reward_sums_[arm] + value;
        const long double squared_sum =
            squared_reward_sums_[arm] + value * value;
        if(!std::isfinite(sum) || !std::isfinite(squared_sum))[[unlikely]]{
            throw std::overflow_error(
                "UCB-Tuned reward statistics are not finite"
            );
        }
        reward_sums_[arm] = sum;
        squared_reward_sums_[arm] = squared_sum;
        ++pulls_[arm];
        ++total_pulls_;
    }

private:
    [[nodiscard]] long double score(
        std::size_t arm,
        long double logarithm
    ) const{
        const long double count = static_cast<long double>(pulls_[arm]);
        const long double mean = reward_sums_[arm] / count;
        const long double variance = std::max(
            0.0L, squared_reward_sums_[arm] / count - mean * mean
        );
        const long double adjusted_variance = std::min(
            0.25L, variance + std::sqrt(2.0L * logarithm / count)
        );
        return mean + std::sqrt(logarithm * adjusted_variance / count);
    }

    std::vector<std::size_t> pulls_;
    std::vector<long double> reward_sums_;
    std::vector<long double> squared_reward_sums_;
    std::size_t total_pulls_ = 0;
};

class BernoulliThompsonSamplingBandit{
public:
    explicit BernoulliThompsonSamplingBandit(
        std::size_t arm_count,
        long double prior_alpha = 1.0L,
        long double prior_beta = 1.0L
    )
        : alpha_(arm_count, prior_alpha), beta_(arm_count, prior_beta){
        multi_armed_bandit_internal::require_arm_count(arm_count);
        if(!(prior_alpha > 0.0L) || !(prior_beta > 0.0L)
           || !std::isfinite(prior_alpha) || !std::isfinite(prior_beta)){
            throw std::invalid_argument(
                "Thompson sampling requires positive finite beta priors"
            );
        }
    }

    [[nodiscard]] std::size_t arm_count() const noexcept{
        return alpha_.size();
    }

    [[nodiscard]] long double posterior_alpha(std::size_t arm) const{
        multi_armed_bandit_internal::require_arm(arm, arm_count());
        return alpha_[arm];
    }

    [[nodiscard]] long double posterior_beta(std::size_t arm) const{
        multi_armed_bandit_internal::require_arm(arm, arm_count());
        return beta_[arm];
    }

    template<class URBG>
    [[nodiscard]] std::size_t select_arm(URBG& generator) const{
        std::size_t best = 0;
        long double best_sample = sample_beta(
            alpha_[0], beta_[0], generator
        );
        for(std::size_t arm = 1; arm < arm_count(); ++arm){
            const long double sample = sample_beta(
                alpha_[arm], beta_[arm], generator
            );
            if(best_sample < sample){
                best = arm;
                best_sample = sample;
            }
        }
        return best;
    }

    void update(std::size_t arm, bool success){
        multi_armed_bandit_internal::require_arm(arm, arm_count());
        if(success) alpha_[arm] += 1.0L;
        else beta_[arm] += 1.0L;
    }

private:
    template<class URBG>
    static long double sample_beta(
        long double alpha,
        long double beta,
        URBG& generator
    ){
        std::gamma_distribution<long double> first(alpha, 1.0L);
        std::gamma_distribution<long double> second(beta, 1.0L);
        const long double x = first(generator);
        const long double y = second(generator);
        if(x == 0.0L && y == 0.0L) return 0.5L;
        return x / (x + y);
    }

    std::vector<long double> alpha_;
    std::vector<long double> beta_;
};

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_MULTI_ARMED_BANDIT_HPP_INCLUDED
