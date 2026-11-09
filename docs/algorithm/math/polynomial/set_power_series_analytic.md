---
title: Set Power Series Analytic Functions (集合冪級数解析関数) [exp_of_set_power_series] [log_of_set_power_series]
documentation_of: ../../../../src/algorithm/math/polynomial/set_power_series_analytic.hpp
---

部分集合を添字とし、積を互いに素な和集合に対する部分集合畳み込み（subset convolution）とする集合冪級数の指数関数と対数関数を計算する。
ランク付き zeta 変換、各部分集合上での一変数形式的冪級数の漸化式、Möbius 変換を用いる。

## `set_power_series_exponential`

```cpp
template<class T>
std::vector<T> set_power_series_exponential(const std::vector<T>& series);
```

`series[0] == 0` である集合冪級数 $\mathtt{series}$ に対し、$\exp(\mathtt{series})$ を返す。

- 時間計算量: $O(n^2 2^n)$
- 空間計算量: $O(n 2^n)$

## `set_power_series_logarithm`

```cpp
template<class T>
std::vector<T> set_power_series_logarithm(const std::vector<T>& series);
```

`series[0] == 1` である集合冪級数 $\mathtt{series}$ に対し、$\log(\mathtt{series})$ を返す。

- 時間計算量: $O(n^2 2^n)$
- 空間計算量: $O(n 2^n)$

## 注意点

- 入力長は $0$ でない 2 の冪 $2^n$ でなければならない（$n$ は添字のビット数）。
- `T` は四則演算、整数からの構築、`operator==` を提供し、$1, 2, \ldots, n$ が可逆でなければならない。
- 指数関数で空集合の係数が $0$ でない場合、対数関数で空集合の係数が $1$ でない場合、および入力が空の場合は `std::domain_error` を送出する。入力長が 2 の冪でない場合は `std::invalid_argument` を送出する。
