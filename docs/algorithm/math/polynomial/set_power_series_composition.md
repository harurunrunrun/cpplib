---
title: Set Power Series Composition and Power Projection (集合冪級数の合成と冪射影) [polynomial_composite_set_power_series] [power_projection_of_set_power_series]
documentation_of: ../../../../src/algorithm/math/polynomial/set_power_series_composition.hpp
---

長さ $2^N$ の配列を、$x_i^2 = 0$ を満たす $N$ 変数多項式として扱う。
積は互いに素な部分集合に対する部分集合畳み込み（subset convolution）である。

## 指数型母関数の合成

```cpp
template<class T>
std::vector<T> compose_exponential_generating_function_set_power_series(
    const std::vector<T>& exponential_coefficients,
    const std::vector<T>& series
);
```

`series[0] == 0` かつ `exponential_coefficients.size() == N + 1` のとき、$e_k$ を `exponential_coefficients[k]`、$s$ を `series` として

$$
\sum_{k=0}^{N} e_k \frac{s^k}{k!}
$$

を返す。次元を 1 つずつ追加し、各段階でランク付き zeta 変換された係数を更新する。

## 一変数多項式の合成

```cpp
template<class T>
std::vector<T> polynomial_composite_set_power_series(
    const std::vector<T>& polynomial,
    const std::vector<T>& series
);
```

$f(x) = \sum_i \mathtt{polynomial}_i x^i$ に対し、$f(s)$ を返す。
空集合の係数のまわりの Taylor 係数を $N$ 次まで計算してから、指数型母関数の合成に帰着する。空の `polynomial` は零多項式を表す。

## 冪の線形射影

```cpp
template<class T>
std::vector<T> set_power_series_power_projection(
    const std::vector<T>& series,
    const std::vector<T>& weights,
    std::size_t output_count
);
```

$Q$ を `output_count`、$w_S$ を `weights[S]` として、$0 \le k < Q$ について

$$
\sum_{S\subseteq\{0,\ldots,N-1\}} w_S \, [x^S] s^k
$$

を順に並べた長さ $Q$ の列を返す。部分集合畳み込みの転置を次元ごとに適用し、$s$ の空集合の係数は最後に二項展開で戻す。`output_count == 0` なら空の列を返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（戻り値を含む） |
| --- | --- | --- |
| `compose_exponential_generating_function_set_power_series` | $O(N^2 2^N)$ | $O(N 2^N)$ |
| `polynomial_composite_set_power_series` | $O(N^2 2^N + N M)$ | $O(N 2^N)$ |
| `set_power_series_power_projection` | $O(N^2 2^N + N Q)$ | $O(N 2^N + Q)$ |

ここで $M$ は一変数多項式の係数の個数、$Q$ は出力の個数である。

## 注意点

- 集合冪級数の長さは $0$ でない 2 の冪でなければならない。
- `exponential_coefficients` の長さは $N + 1$ でなければならない。
- 冪射影の `series` と `weights` の長さは等しくなければならない。
- `T` は四則演算、整数からの構築、等値比較を提供する必要がある。冪射影では $\max(N, Q-1)$ の階乗が可逆でなければならない（合成の 2 関数は除算を行わない）。
- 入力の契約違反では `std::invalid_argument`（長さの不正）または `std::domain_error`（指数型母関数の合成で `series[0] != 0`）を送出する。
