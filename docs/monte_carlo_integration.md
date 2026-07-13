---
title: Monte Carlo Integration (モンテカルロ積分)
documentation_of: ../src/approximate/randomized/monte_carlo_integration.hpp
---

一様乱数で1次元区間または超直方体上の積分を推定する。

## `MonteCarloEstimate`

- `estimate`: 標本平均に領域体積を掛けた積分推定値。
- `standard_error`: 不偏標本分散から求めた推定標準誤差。標本が1個なら0。
- `samples`: 評価した標本数。

平均と分散はWelford法により`long double`で更新する。

## `monte_carlo_integrate`

```cpp
auto result = monte_carlo_integrate(function, lower, upper, sample_count, random);
```

`[lower, upper]`上で`function(double)`を一様標本化する。時間計算量は $O(SC_f)$、
追加空間計算量は $O(1)$。ここで$S$は`sample_count`、$C_f$は1回の関数評価時間である。

## `monte_carlo_integrate_hyperrectangle`

```cpp
auto result = monte_carlo_integrate_hyperrectangle(
    lower, upper, sample_count, function, random
);
```

`vector<double>`版と`array<double, D>`版があり、`function`は同じ型の座標列を受け取る。
次元を$D$とすると時間計算量は $O(S(D+C_f))$、追加空間計算量は`vector`版で$O(D)$、
`array`版で関数引数以外$O(D)$。

各標本を独立とし、被積分関数の分散が有限なら推定値は不偏で、標準誤差は
$O(S^{-1/2})$で減少する。boundsと関数値は有限、各`lower[i] <= upper[i]`、
`sample_count > 0`でなければならない。次元不一致、非有限値、逆転したboundsは
`std::invalid_argument`または`std::domain_error`、体積や返却値のoverflowは
`std::overflow_error`となる。幅0の次元があれば積分値と標準誤差は0になる。

## 注意点

確率的保証は各APIで示した独立性と入力条件の下で成り立つ。同じ結果の再現には同じ乱数器状態・入力・標準ライブラリ実装が必要で、中間演算は使用型の表現範囲内でなければならない。
