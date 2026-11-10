---
title: Polynomial Interpolation (多項式補間) [polynomial_interpolation]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_interpolation.hpp
---

固定長配列による Lagrange 補間と、積の木（product tree）を用いた高速な任意点での補間。

```cpp
#include "src/algorithm/math/polynomial/polynomial_interpolation.hpp"

using mint = Modint998244353;
math::PolynomialInterpolation<mint, 200000> interpolation;
```

`MAX_DEGREE` は扱う多項式の最大次数で、標本の個数は $\texttt{MAX_DEGREE} + 1$ 以下でなければならない。係数の列は定数項から次数の昇順に格納する。

テンプレート引数 `MAX_DEGREE` を省略すると、実行時のサイズに合わせて `std::vector` を使う可変長版になる。可変長版の階乗表は、使った最大次数まで保持する。

```cpp
math::PolynomialInterpolation<Mint> dynamic;
math::PolynomialInterpolation<Mint, 1000> fixed;
auto coefficients = dynamic.interpolate_coefficients(xs, ys, xs.size());
```

## 多項式評価

```cpp
template<class T, std::size_t CAPACITY>
T math::polynomial_evaluate(
    const std::array<T, CAPACITY>& coefficients,
    std::size_t coefficient_count,
    T x
);
```

$d$ を `coefficient_count`、$a_i$ を `coefficients[i]` として

$$
f(X)=\sum_{i=0}^{d-1} a_i X^i
$$

を Horner 法で `x` において評価する。`coefficient_count == 0` では $0$ を返す。`coefficient_count` が `CAPACITY` を超えると `std::runtime_error` を送出する（`std::vector` 版では長さを超えると `std::out_of_range`）。

## 連続整数点での Lagrange 評価

```cpp
mint value = interpolation.evaluate_consecutive(values, sample_count, x);
```

$n$ を `sample_count` として、次数が $n - 1$ 以下で

$$
f(i)=\mathrm{values}_i\quad(0\le i<n)
$$

を満たす多項式 $f$ の値 $f(x)$ を返す。$x$ が標本点と一致する場合も同じ API を使える。

## 等間隔点での Lagrange 評価

```cpp
mint value = interpolation.evaluate_equidistant(
    values, sample_count, first, step, x
);
```

$f(\mathrm{first} + \mathrm{step} \cdot i) = \mathrm{values}_i$ を満たす多項式の値 $f(x)$ を返す。`step` は $0$ でなく、`Mint` 上で逆元を持たなければならない。

## 任意点から係数を復元

```cpp
auto coefficients = interpolation.interpolate_coefficients(
    xs, ys, sample_count
);
```

$f(\mathrm{xs}_i) = \mathrm{ys}_i$ を満たす次数 $n - 1$ 以下の多項式を、定数項から次数の昇順の係数の列として返す。使わない末尾の要素は $0$ になる。`xs` の先頭 $n$ 個は相異なる必要がある。

`Mint` が `Modint<MOD>` のときは、積の木による高速な補間に委譲する。それ以外の体の要素型では汎用的な 2 乗時間の Lagrange 補間を使う。分母 $\prod_{j\ne i}(x_i-x_j)$ はまとめて逆元をとるので、`inv()` の呼び出しは標本の個数によらず 1 回である。

## 任意点での補間評価

```cpp
mint value = interpolation.evaluate(xs, ys, sample_count, x);
```

`interpolate_coefficients` で係数を復元してから Horner 法で評価する。

## 積の木による高速な任意点補間

```cpp
template<int MOD>
math::Polynomial<MOD> math::polynomial_interpolation(
    const std::vector<Modint<MOD>>& points,
    const std::vector<Modint<MOD>>& values
);
```

点 `points[i]` で値 `values[i]` をとる次数 $\lvert \mathrm{points} \rvert - 1$ 以下の多項式を、定数項から次数の昇順の係数の列として返す。末尾の $0$ は省略し、標本が空なら零多項式を表す空の列を返す。

内部では

1. $\prod_i(X-p_i)$（$p_i$ は `points[i]`）の積の木を作る
2. その導関数を多点評価して Lagrange 補間の分母を求める
3. 積の木を下から合成して係数の列を復元する

という処理を行う。`points.size() != values.size()`、または評価点に重複がある場合は
`std::invalid_argument` を送出する。

## 時間計算量・空間計算量

`Mint` の四則演算と `inv()` を $O(1)$ とする。$n$ を `sample_count`、$d$ を `coefficient_count`、$M(n)$ を長さ $n$ の多項式の積の計算量（この実装では $O(n \log n)$）とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `PolynomialInterpolation()` | $O(\texttt{MAX_DEGREE})$ | オブジェクトに $O(\texttt{MAX_DEGREE})$ |
| `polynomial_evaluate` | $O(d)$ | $O(1)$ |
| `evaluate_consecutive` | $O(n)$ | $O(\texttt{MAX_DEGREE})$ |
| `evaluate_equidistant` | $O(n)$ | $O(\texttt{MAX_DEGREE})$ |
| `interpolate_coefficients` | `Modint<MOD>`: $O(M(n)\log n)$、その他: $O(n^2)$ | `Modint<MOD>`: $O(n\log n+\texttt{MAX_DEGREE})$、その他: $O(\texttt{MAX_DEGREE})$ |
| `evaluate` | `Modint<MOD>`: $O(M(n)\log n)$、その他: $O(n^2)$ | `interpolate_coefficients` と同じ |
| `polynomial_interpolation<MOD>` | $O(M(n)\log n)$ | $O(n\log n)$ |

固定長版は作業配列にも `std::array` を使うので、追加領域は実際の $n$ ではなくテンプレートの固定容量で決まる。`MAX_DEGREE` が非常に大きいインスタンスは静的領域に置くとよい。可変長版では $\texttt{MAX_DEGREE}$ を $n$ に読み替える（係数の復元は `Modint<MOD>` で $O(n \log n)$ 空間、一般の体では $O(n)$ 空間）。

## 注意点

- `Mint` は加減乗除、単項マイナス、比較、`inv()`、整数からの構築を持つ体の要素型でなければならない。
- 固定長版のコンストラクタは $\texttt{MAX_DEGREE}!$ の逆元を 1 回求める。通常の素数を法とする modint では $\texttt{MAX_DEGREE} < \mathrm{mod}$ が必要である。
- 任意点での補間では、各 $\mathrm{xs}_i - \mathrm{xs}_j$ が逆元を持つ必要があり、重複した点があると `std::runtime_error` を送出する。
- `sample_count == 0` や固定容量を超える指定は `std::runtime_error` を送出する。
- 高速版 `polynomial_interpolation` の `points` は `Modint<MOD>` 上で相異なる必要があり、`points.size() != values.size()` や重複点では `std::invalid_argument` を送出する。
- `Modint<MOD>` を使う固定長配列の API と高速版では、NTT に必要な 2 の冪の変換長が $\mathrm{MOD} - 1$ を割り切る必要がある。
