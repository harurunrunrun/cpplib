---
title: Polynomial Discrete Sum (多項式の離散和) [prefix_sum_of_polynomial]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_discrete_sum.hpp
---

Faulhaber 多項式と、係数表示された多項式の離散不定和を扱う。累積和はすべて半開区間 $[0,n)$、すなわち $0$ から $n-1$ までを足し合わせる。

```cpp
#include "src/algorithm/math/polynomial/polynomial_discrete_sum.hpp"

using mint = Modint998244353;
math::PolynomialDiscreteSum<mint, 200000> summation;
```

## `PolynomialDiscreteSum<Mint, MAX_DEGREE>`

次数の上限をコンパイル時に固定した離散和の表である。公開型 `CoefficientArray` は長さ $\texttt{MAX_DEGREE} + 1$ の入力係数の配列、`AntiderivativeArray` は長さ $\texttt{MAX_DEGREE} + 2$ の離散不定和の係数の配列である。コンストラクタは階乗と逆階乗を構築する。

テンプレート引数 `MAX_DEGREE` を省略すると、実行時のサイズに合わせて `std::vector` を使う可変長版になり、結果も実際の次数に応じた長さの `std::vector` で返す。

```cpp
math::PolynomialDiscreteSum<Mint> dynamic;
math::PolynomialDiscreteSum<Mint, 1000> fixed;
auto answer = dynamic.power_sum(n, exponent);
```

## `power_sum`

```cpp
Mint power_sum(long long n, std::size_t exponent) const;
```

非負整数 `n` に対して

$$
\sum_{i=0}^{n-1} i^{k}
$$

（$k$ は `exponent`）を返す。`exponent == 0` では $0^0=1$ として `n` を返す。

## `faulhaber`

```cpp
Mint faulhaber(Mint n, std::size_t exponent) const;
```

同じ冪和を表す次数 $k+1$ の Faulhaber 多項式を `n` で評価する。`power_sum` は非負整数を `Mint` に変換してこの関数を呼ぶ。

実装は $S(t)=\sum_{i=0}^{t-1}i^k$ を $t = 0, 1, \ldots, k+1$ で求め、連続する整数点用の Lagrange 補間で評価する。

## `discrete_antiderivative`

```cpp
AntiderivativeArray discrete_antiderivative(
    const CoefficientArray& coefficients,
    std::size_t coefficient_count
) const;
```

$d$ を `coefficient_count`、$a_i$ を `coefficients[i]` として

$$
f(X)=\sum_{i=0}^{d-1} a_i X^i
$$

に対して、$F(0) = 0$ かつ

$$
F(X+1)-F(X)=f(X)
$$

を満たす $F$ の係数を、定数項から次数の昇順に返す。戻り値の有効な要素数は $d + 1$ である。空の多項式には零多項式を返す。

`Mint` が `Modint<MOD>` または `FastModint<MOD>` なら、正規化した Bernoulli 数の列 $B_r/r!$ を形式的冪級数の逆元で構築する。$j \ge 1$ に対する

$$
[X^j]F(X)=\frac{1}{j!}\sum_{k=j-1}^{d-1}
a_k k!\frac{B_{k+1-j}}{(k+1-j)!}
$$

を相互相関、すなわち 1 回の畳み込みとして一括で計算する。その他の `Mint` では、同じ結果を三角形状の係数の漸化式で求める汎用の経路を使う。

## `polynomial_prefix_sum`

```cpp
Mint polynomial_prefix_sum(
    const CoefficientArray& coefficients,
    std::size_t coefficient_count,
    Mint n
) const;
Mint polynomial_prefix_sum(
    const CoefficientArray& coefficients,
    std::size_t coefficient_count,
    long long n
) const;
```

$F(n)$ を返す。`Mint` 版は係数体の任意の要素で評価する。`long long` 版は非負整数 $n$ について $\sum_{i=0}^{n-1}f(i)$ を返す。多数の $n$ で評価する場合は、`discrete_antiderivative` を 1 回だけ呼び、戻り値を `math::polynomial_evaluate` で評価する。

## 時間計算量・空間計算量

`Mint` の四則演算を $O(1)$、`Mint::pow(k)` を $O(\log(k+1))$ とする。$k$ を `exponent`、$d$ を `coefficient_count`、長さ $d$ 程度の `arbitrary_mod_convolution` の計算量を $M(d)$ とする。

| API | `Modint` / `FastModint` の時間 | その他の `Mint` の時間 | 空間計算量（追加領域） |
| --- | --- | --- | --- |
| `PolynomialDiscreteSum()` | $O(\texttt{MAX_DEGREE})$ | $O(\texttt{MAX_DEGREE})$ | オブジェクトに $O(\texttt{MAX_DEGREE})$ |
| `faulhaber` | $O(k\log(k+1))$ | $O(k\log(k+1))$ | $O(\texttt{MAX_DEGREE})$ |
| `power_sum` | $O(k\log(k+1))$ | $O(k\log(k+1))$ | $O(\texttt{MAX_DEGREE})$ |
| `discrete_antiderivative` | $O(M(d)\log d)$ | $O(d^2)$ | 戻り値 $O(\texttt{MAX_DEGREE})$、高速経路の作業領域 $O(d)$ |
| `polynomial_prefix_sum` | $O(M(d)\log d+d)$ | $O(d^2)$ | $O(\texttt{MAX_DEGREE}+d)$ |
| 構築済み離散不定和の `polynomial_evaluate` | $O(d)$ | $O(d)$ | $O(1)$ |

`exponent == 0` の `faulhaber` と `power_sum` は $O(1)$。可変長版では $\texttt{MAX_DEGREE}$ を実際の次数（$k$ または $d$）に読み替える。

## 注意点

- `Mint` は加減乗除、整数からの構築、`pow(long long)` を提供する必要がある。
- 固定長版のコンストラクタは $(\texttt{MAX_DEGREE} + 1)!$ の逆元を求める。通常の素数を法とする modint では $\texttt{MAX_DEGREE} + 1 < \mathrm{mod}$ が必要である。
- `power_sum` と `polynomial_prefix_sum` の `long long` 版は `n >= 0` を要求し、違反時は `std::runtime_error` を送出する。
- 固定長版では、係数の個数は $\texttt{MAX_DEGREE} + 1$ 以下、冪指数は $\texttt{MAX_DEGREE}$ 以下でなければならず、違反時は `std::runtime_error` を送出する。
- 高速経路では、対応する法で畳み込みが実行できなければならない。変換長の上限超過など、畳み込みに由来する例外はそのまま伝播する。
- 合成数の法では途中に現れる整数が逆元を持たず、構築や除算が失敗することがある。
