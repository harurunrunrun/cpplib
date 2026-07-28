---
title: Set Power Series Analytic Functions (集合冪級数解析関数) [exp_of_set_power_series] [log_of_set_power_series]
documentation_of: ../../../../src/algorithm/math/polynomial/set_power_series_analytic.hpp
---

部分集合を添字とし、積をdisjoint unionに対するsubset convolutionとする
集合冪級数の指数関数と対数関数を計算する。
次数別zeta変換、各subset上の一変数FPS漸化式、Möbius逆変換を用いる。

## `set_power_series_exponential`

```cpp
template<class T>
vector<T> set_power_series_exponential(const vector<T>& series);
```

`series[0] == 0` とする集合冪級数の $\exp(\mathtt{series})$ を返す。

- 時間計算量: $O(n^2 2^n)$
- 空間計算量: $O(n2^n)$

## `set_power_series_logarithm`

```cpp
template<class T>
vector<T> set_power_series_logarithm(const vector<T>& series);
```

`series[0] == 1` とする集合冪級数の $\log(\mathtt{series})$ を返す。

- 時間計算量: $O(n^2 2^n)$
- 空間計算量: $O(n2^n)$

## 注意点

入力長は非零の2冪で、添字のbit数を $n$ とする。
`T` は四則演算、整数からの構築、`operator==` を提供し、
`1,2,\ldots,n` が可逆でなければならない。
指数関数の空集合係数が0でない場合、対数関数の空集合係数が1でない場合、
入力長が不正な場合は例外を送出する。
