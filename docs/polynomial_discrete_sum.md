---
title: Polynomial Discrete Sum (多項式の離散和)
documentation_of: ../src/algorithm/math/polynomial/polynomial_discrete_sum.hpp
---

Faulhaber 多項式と、係数表示された多項式の離散不定和を扱う。すべての
prefix sum は半開区間 $[0,n)$、すなわち添字 `0` から `n - 1` までを加える。

```cpp
#include "src/algorithm/math/polynomial/polynomial_discrete_sum.hpp"

using mint = Modint998244353;
math::PolynomialDiscreteSum<mint, 200000> summation;
```

## `PolynomialDiscreteSum<Mint, MAX_DEGREE>`

次数上限をコンパイル時に固定した離散和テーブルである。公開型
`CoefficientArray` は長さ `MAX_DEGREE + 1` の入力係数配列、
`AntiderivativeArray` は長さ `MAX_DEGREE + 2` の離散不定和係数配列である。
constructor は階乗と逆階乗を構築する。

## `power_sum`

```cpp
Mint power_sum(long long n, std::size_t exponent) const;
```

非負整数 `n` に対して

$$
\sum_{i=0}^{n-1} i^{exponent}
$$

を返す。`exponent == 0` では $0^0=1$ として `n` を返す。

## `faulhaber`

```cpp
Mint faulhaber(Mint n, std::size_t exponent) const;
```

同じ冪和を表す次数 `exponent + 1` の Faulhaber 多項式を `mint_n` で評価する。
`power_sum` は非負整数を `Mint` へ変換してこの関数を呼ぶ。

実装は $S(t)=\sum_{i=0}^{t-1}i^k$ を `t = 0,1,...,k+1` で作り、
連続整数点用 Lagrange 補間で評価する。

## `discrete_antiderivative`

```cpp
AntiderivativeArray discrete_antiderivative(
    const CoefficientArray& coefficients,
    std::size_t coefficient_count
) const;
```

$$
f(X)=\sum_{i=0}^{coefficient\_count-1} coefficients_i X^i
$$

に対して、`F(0) == 0` かつ

$$
F(X+1)-F(X)=f(X)
$$

を満たす `F` の係数を定数項から昇冪順で返す。返り値の有効な要素数は
`coefficient_count + 1` である。空の多項式には零多項式を返す。

`Mint` が `Modint<MOD>` または `FastModint<MOD>` なら、正規化 Bernoulli 数列
$B_r/r!$ を形式的冪級数逆元で構築する。$j \ge 1$ に対する

$$
[X^j]F(X)=\frac{1}{j!}\sum_{k=j-1}^{d-1}
a_k k!\frac{B_{k+1-j}}{(k+1-j)!}
$$

を相互相関、すなわち 1 回の畳み込みとして一括計算する。その他の `Mint` では
同じ結果を三角形の係数漸化式で求める汎用経路を使う。

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

`F(n)` を返す。`Mint` 版は任意の係数体要素で多項式を評価する。
`long long` 版は非負整数について $\sum_{i=0}^{n-1}f(i)$ を返す。
多数の `n` を評価する場合は
`discrete_antiderivative` を1回だけ呼び、返り値を
`math::polynomial_evaluate` で評価する。

## API別の時間計算量・空間計算量

`Mint` の四則演算を $O(1)$、`Mint::pow(k)` を $O(\log(k+1))$ とする。
`k = exponent`、`d = coefficient_count`、長さ $d$ 程度の
`arbitrary_mod_convolution` の計算量を $M(d)$ とする。

| API | `Modint` / `FastModint` の時間 | その他の `Mint` の時間 | 空間計算量（追加領域） |
| --- | --- | --- | --- |
| `PolynomialDiscreteSum()` | $O(MAX\_DEGREE)$ | $O(MAX\_DEGREE)$ | オブジェクトに $O(MAX\_DEGREE)$ |
| `faulhaber` | $O(k\log(k+1))$ | $O(k\log(k+1))$ | $O(MAX\_DEGREE)$ |
| `power_sum` | $O(k\log(k+1))$ | $O(k\log(k+1))$ | $O(MAX\_DEGREE)$ |
| `discrete_antiderivative` | $O(M(d)\log d)$ | $O(d^2)$ | 返り値 $O(MAX\_DEGREE)$、高速経路の作業領域 $O(d)$ |
| `polynomial_prefix_sum` | $O(M(d)\log d+d)$ | $O(d^2)$ | $O(MAX\_DEGREE+d)$ |
| 構築済み離散不定和の `polynomial_evaluate` | $O(d)$ | $O(d)$ | $O(1)$ |

`exponent == 0` の `faulhaber` と `power_sum` は $O(1)$。

## 注意点

- `Mint` は加減乗除、整数からの構築、`pow(long long)` を提供する。
- constructor は `(MAX_DEGREE + 1)!` の逆元を求める。通常の素数 modint では
  `MAX_DEGREE + 1 < mod` が必要。
- `power_sum` と `polynomial_prefix_sum` の `long long` 版は `n >= 0` を要求する。
- 係数数は `MAX_DEGREE + 1` 以下、冪指数は `MAX_DEGREE` 以下でなければならない。
- 高速経路では対応する法で畳み込みが実行可能でなければならない。変換長の
  上限超過など、畳み込み由来の例外はそのまま伝播する。
- 合成数 mod では途中に現れる整数が逆元を持たず、構築または除算が失敗することがある。
