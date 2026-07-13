---
title: Polynomial Interpolation
documentation_of: ../src/algorithm/math/polynomial_interpolation.hpp
---

固定長配列による Lagrange 補間と、積木を用いた高速な任意点補間。

```cpp
#include "src/algorithm/math/polynomial_interpolation.hpp"

using mint = Modint998244353;
math::PolynomialInterpolation<mint, 200000> interpolation;
```

`MAX_DEGREE` は扱う多項式の最大次数である。標本数は
`MAX_DEGREE + 1` 以下でなければならない。係数列は定数項から昇冪順に格納する。

## 多項式評価

```cpp
template<class T, std::size_t CAPACITY>
T math::polynomial_evaluate(
    const std::array<T, CAPACITY>& coefficients,
    std::size_t coefficient_count,
    T x
);
```

$$
f(X)=\sum_{i=0}^{coefficient\_count-1} coefficients_i X^i
$$

を Horner 法で `x` において評価する。`coefficient_count == 0` では0を返す。

## 連続整数点での Lagrange 評価

```cpp
mint value = interpolation.evaluate_consecutive(values, sample_count, x);
```

次数が `sample_count - 1` 以下で、

$$
f(i)=values_i\quad(0\le i<sample\_count)
$$

を満たす多項式の `f(x)` を返す。`x` が標本点と一致する場合も同じ API を使える。

## 等間隔点での Lagrange 評価

```cpp
mint value = interpolation.evaluate_equidistant(
    values, sample_count, first, step, x
);
```

`f(first + step * i) == values[i]` を満たす多項式の `f(x)` を返す。
`step` は0でなく、`Mint` 上で逆元を持たなければならない。

## 任意点から係数を復元

```cpp
auto coefficients = interpolation.interpolate_coefficients(
    xs, ys, sample_count
);
```

`f(xs[i]) == ys[i]` を満たす次数 `sample_count - 1` 以下の多項式を、
定数項から昇冪順の係数列として返す。使用しない末尾は0になる。
`xs[0..sample_count)` は相異なる必要がある。

分母 $\prod_{j\ne i}(x_i-x_j)$ は一括逆元で反転するため、`inv()` の呼出しは
標本数によらず1回である。

## 任意点での補間評価

```cpp
mint value = interpolation.evaluate(xs, ys, sample_count, x);
```

`interpolate_coefficients` で係数を復元してから Horner 法で評価する。

## 積木による高速な任意点補間

```cpp
template<int MOD>
math::Polynomial<MOD> math::polynomial_interpolation(
    const std::vector<Modint<MOD>>& points,
    const std::vector<Modint<MOD>>& values
);
```

`points[i]` で値 `values[i]` を取る次数 `points.size() - 1` 以下の多項式を、
定数項から昇冪順の係数列として返す。末尾の0は省略され、標本が空なら
0多項式を表す空の係数列を返す。

内部では

1. $\prod_i(X-points_i)$ の積木を作る
2. その導関数を多点評価して Lagrange 分母を求める
3. 積木を下から合成して係数列を復元する

という処理を行う。`points.size() != values.size()`、または評価点に重複がある場合は
`std::invalid_argument` を送出する。

## 計算量

`Mint` の四則演算と `inv()` を $O(1)$ とする。`n = sample_count`、
`d = coefficient_count` とする。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| `PolynomialInterpolation()` | $O(MAX\_DEGREE)$ | オブジェクトに $O(MAX\_DEGREE)$ |
| `polynomial_evaluate` | $O(d)$ | $O(1)$ |
| `evaluate_consecutive` | $O(n)$ | $O(MAX\_DEGREE)$ |
| `evaluate_equidistant` | $O(n)$ | $O(MAX\_DEGREE)$ |
| `interpolate_coefficients` | $O(n^2)$ | $O(MAX\_DEGREE)$ |
| `evaluate` | $O(n^2)$ | $O(MAX\_DEGREE)$ |
| `polynomial_interpolation<MOD>` | $O(M(n)\log n)$ | $O(n\log n)$ |

作業配列にも `std::array` を使うため、追加領域は実際の `n` ではなくテンプレートの
固定容量で決まる。非常に大きい `MAX_DEGREE` のインスタンスは静的領域へ置くとよい。

## 前提

- `Mint` は加減乗除、単項マイナス、比較、`inv()`、整数からの構築を持つ体要素型。
- constructor は `MAX_DEGREE!` の逆元を1回求める。通常の素数 modint では
  `MAX_DEGREE < mod` が必要。
- 任意点補間では各 `xs[i] - xs[j]` が逆元を持つ必要がある。重複点は例外になる。
- `sample_count == 0` または固定容量を超える指定は例外になる。
- 高速版の `points` は `Modint<MOD>` 上で相異なる必要がある。
- 高速版では NTT に必要な2冪変換長が `MOD - 1` を割り切る必要がある。
- 高速版の $M(n)$ は長さ $n$ の多項式積の計算量で、この実装では $O(n\log n)$ である。
