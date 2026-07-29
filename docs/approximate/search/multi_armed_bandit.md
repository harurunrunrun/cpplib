---
title: Multi-Armed Bandit Policies (多腕バンディット方策)
documentation_of: ../../../src/approximate/search/multi_armed_bandit.hpp
---

有限個の腕について、観測済み報酬から次に試す腕を選ぶオンライン方策を提供する。
UCB 系は決定的、Thompson Sampling は乱択である。

## `ucb1_score`

```cpp
long double ucb1_score(reward_sum, pulls, total_pulls)
```

平均報酬と探索項 $\sqrt{2\log T/n}$ の和を返す。時間・空間計算量は
$O(1)$。

## `Ucb1Bandit`

```cpp
Ucb1Bandit<Reward> bandit(arm_count);
bandit.select_arm();
bandit.update(arm, reward);
bandit.arm_count();
bandit.total_pulls();
bandit.pulls(arm);
bandit.mean_reward(arm);
```

未試行の腕を番号順に選び、その後は UCB1 値が最大の腕を選ぶ。
`select_arm()` は腕数を $K$ として時間計算量 $O(K)$、追加空間
$O(1)$。`update`、`pulls`、`mean_reward` は時間計算量 $O(1)$。
構築の時間・空間計算量は $O(K)$。

## `UcbTunedBandit`

```cpp
UcbTunedBandit<Reward> bandit(arm_count);
bandit.select_arm();
bandit.update(arm, reward);
bandit.arm_count();
bandit.total_pulls();
bandit.pulls(arm);
bandit.mean_reward(arm);
```

標本分散で探索幅を調整する UCB-Tuned 方策である。`select_arm()` の時間計算量は
$O(K)$、それ以外の公開操作は $O(1)$。保持する空間は $O(K)$。

## `BernoulliThompsonSamplingBandit`

```cpp
BernoulliThompsonSamplingBandit bandit(
    arm_count, prior_alpha = 1, prior_beta = 1);
bandit.select_arm(generator);
bandit.update(arm, success);
bandit.arm_count();
bandit.posterior_alpha(arm);
bandit.posterior_beta(arm);
```

各腕の Bernoulli 成功率を独立な Beta 分布で管理する。`select_arm()` は各腕から
1回標本を取り、最大値の腕を返すため、期待時間計算量 $O(K)$、追加空間
$O(1)$。更新と posterior の取得は $O(1)$、保持する空間は $O(K)$。

## 注意点

腕数は正でなければならない。腕番号が範囲外なら `std::out_of_range`、UCB の
報酬が非有限なら `std::invalid_argument`、累積値が非有限になるなら
`std::overflow_error` を送出する。UCB-Tuned の分散上界は報酬が $[0,1]$
であることを前提とし、範囲外の報酬を拒否する。`ucb1_score` の
`pulls` は正で、`total_pulls >= pulls` が必要である。Thompson Sampling の
事前分布パラメータは正かつ有限でなければならず、報酬は Bernoulli の成否で
与える。同点では番号の小さい腕を選ぶ。
