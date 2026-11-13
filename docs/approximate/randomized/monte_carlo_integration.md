---
title: Monte Carlo Integration (モンテカルロ積分)
documentation_of: ../../../src/approximate/randomized/monte_carlo_integration.hpp
---

一様乱数を用いて、1 次元区間または超直方体上の積分を推定する。

## `MonteCarloEstimate`

- `estimate`: 標本平均に領域の体積を掛けた積分の推定値。
- `standard_error`: 不偏標本分散から求めた推定標準誤差。標本が 1 個なら 0。
- `samples`: 評価した標本数。

平均と分散は Welford 法により `long double` で更新する。

## `monte_carlo_integrate`

```cpp
auto result = monte_carlo_integrate(function, lower, upper, sample_count, random);
```

$[\mathrm{lower}, \mathrm{upper}]$ 上で `function(double)` を一様に標本化する。$S$ を `sample_count`、$C_f$ を関数 1 回の評価時間として、時間計算量は $O(S C_f)$、追加空間計算量は $O(1)$。

## `monte_carlo_integrate_hyperrectangle`

```cpp
auto result = monte_carlo_integrate_hyperrectangle(
    lower, upper, sample_count, function, random
);
```

`vector<double>` 版と `array<double, D>` 版があり、`function` は同じ型の座標列を受け取る。
次元を $D$ とすると、時間計算量は $O(S(D + C_f))$、追加空間計算量は $O(D)$（`array` 版では関数引数の分を除いて $O(D)$）。

各標本が独立で被積分関数の分散が有限なら、推定値は不偏で、標準誤差は $O(S^{-1/2})$ で減少する。

## 注意点

- 範囲と関数値は有限で、各 `lower[i] <= upper[i]`、`sample_count > 0` でなければならない。
- `sample_count == 0`、次元の不一致、非有限または逆転した範囲に対しては `std::invalid_argument` を、関数が非有限値を返したときは `std::domain_error` を送出する。体積や返値がオーバーフローする場合は `std::overflow_error` を送出する。
- 幅 0 の次元があれば、積分値と標準誤差は 0 になる。
- 同じ結果を再現するには、同じ乱数生成器の状態・入力・標準ライブラリ実装が必要である。
