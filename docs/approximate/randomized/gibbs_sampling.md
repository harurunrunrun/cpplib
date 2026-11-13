---
title: Gibbs Sampling (ギブスサンプリング)
documentation_of: ../../../src/approximate/randomized/gibbs_sampling.hpp
---

## `gibbs_sample`

```cpp
auto samples = approximate::randomized::gibbs_sample(
    initial_state, conditional_sampler, burn_in_sweeps,
    sweeps_between_samples, sample_count, rng
);
```

状態の各座標を先頭から順に条件付き分布で更新する、systematic-scan 型のギブスサンプリングである。コールバックは次の形にする。

```cpp
Value conditional_sampler(
    std::size_t coordinate,
    std::span<const Value> current_state,
    UniformRandomBitGenerator& rng
);
```

1 回のスイープの中では、番号の小さい座標は更新後の値、番号の大きい座標は更新前の値として `current_state` に現れる。
burn-in の後、`sweeps_between_samples` 回スイープを進めるたびに状態を 1 つ保存する。空の状態も許す。

座標数を $D$、burn-in のスイープ数を $B$、標本数を $S$、標本間のスイープ数を $T$、コールバック 1 回の時間を $Q$ とする。
時間計算量は $O((B + ST) DQ + SD)$、返値を含む追加空間計算量は $O(SD + D)$ である。

## 注意点

- `sample_count > 0` のとき、`sweeps_between_samples` は正でなければならない（違反時は `std::invalid_argument`）。
- 総スイープ数が `size_t` でオーバーフローする場合は `std::overflow_error`、返す配列が表現できない大きさの場合は `std::length_error` を送出する。
- コールバックの戻り値の正しさ、定常分布、burn-in の長さや自己相関の妥当性は呼び出し側で管理する。
- 同じ結果を再現するには、同じ乱数生成器の状態・入力・標準ライブラリ実装が必要である。
