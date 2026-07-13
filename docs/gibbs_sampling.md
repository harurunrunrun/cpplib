---
title: Gibbs Sampling (ギブスサンプリング)
documentation_of: ../src/approximate/randomized/gibbs_sampling.hpp
---

## `gibbs_sample`

```cpp
auto samples = approximate::randomized::gibbs_sample(
    initial_state, conditional_sampler, burn_in_sweeps,
    sweeps_between_samples, sample_count, rng
);
```

状態の各座標を先頭から順に条件付き分布で更新するsystematic-scan Gibbs
Samplingである。callbackは次の形にする。

```cpp
Value conditional_sampler(
    std::size_t coordinate,
    std::span<const Value> current_state,
    UniformRandomBitGenerator& rng
);
```

1 sweep内では、番号の小さい座標は更新後、番号の大きい座標は更新前の値として
`current_state`に現れる。burn-in後、`sweeps_between_samples` sweep進めるたびに
状態を1個保存する。`sample_count > 0` のときsweep間隔は正でなければならない。
総sweep数の `size_t` overflowには `std::overflow_error`、返却配列が表現不能な
場合は `std::length_error` を送出する。空状態も許す。

座標数を $D$、burn-inを $B$、標本数を $S$、標本間sweep数を $T$、
callback 1回の時間を $Q$ とする。時間計算量は
$O((B+ST)DQ+SD)$、返却値を含む領域は $O(SD+D)$ である。
callbackの戻り値、定常分布、burn-inと自己相関の妥当性は呼び出し側で管理する。
