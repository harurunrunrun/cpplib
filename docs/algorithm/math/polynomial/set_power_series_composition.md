---
title: Set Power Series Composition and Power Projection (集合冪級数の合成と冪射影) [polynomial_composite_set_power_series] [power_projection_of_set_power_series]
documentation_of: ../../../../src/algorithm/math/polynomial/set_power_series_composition.hpp
---

長さ $2^N$ の配列を、$x_i^2=0$ を満たす $N$ 変数多項式として扱う。
積は互いに素な部分集合に対するsubset convolutionである。

## 指数型母関数の合成

```cpp
template<class T>
vector<T> compose_exponential_generating_function_set_power_series(
    const vector<T>& exponential_coefficients,
    const vector<T>& series
);
```

`series[0] == 0` および
`exponential_coefficients.size() == N + 1` のとき、

$$
\sum_{k=0}^{N}
\mathtt{exponential\_coefficients}[k]\frac{s^k}{k!}
$$

を返す。次元を一つずつ追加し、各段階でranked zeta変換された
係数を更新する。

## 一変数多項式の合成

```cpp
template<class T>
vector<T> polynomial_composite_set_power_series(
    const vector<T>& polynomial,
    const vector<T>& series
);
```

$f(x)=\sum_i\mathtt{polynomial}[i]x^i$ に対する $f(s)$ を返す。
空集合係数の周りのTaylor係数を $N$ 次まで計算してから、
指数型母関数の合成へ帰着する。空の `polynomial` は零多項式を表す。

## 冪の線形射影

```cpp
template<class T>
vector<T> set_power_series_power_projection(
    const vector<T>& series,
    const vector<T>& weights,
    size_t output_count
);
```

$0\le k<\mathtt{output\_count}$ について

$$
\sum_{S\subseteq\{0,\ldots,N-1\}}
\mathtt{weights}[S]\,[x^S]s^k
$$

を順に返す。subset convolution の転置を次元ごとに適用し、
$s$ の空集合係数は最後に二項展開で戻す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（戻り値を含む） |
| --- | --- | --- |
| `compose_exponential_generating_function_set_power_series` | $O(N^2 2^N)$ | $O(N2^N)$ |
| `polynomial_composite_set_power_series` | $O(N^2 2^N+NM)$ | $O(N2^N)$ |
| `set_power_series_power_projection` | $O(N^2 2^N+NQ)$ | $O(N2^N+Q)$ |

ここで $M$ は一変数多項式の係数数、$Q$ は出力数である。

## 注意点

- 集合冪級数の長さは非零の2冪でなければならない。
- 冪射影の `series` と `weights` の長さは等しくなければならない。
- `T` は四則演算、整数からの構築、等値比較を提供し、
  $1,\ldots,\max(N,Q-1)$ の階乗が可逆でなければならない。
- 入力契約違反では `std::invalid_argument` または
  `std::domain_error` を送出する。
