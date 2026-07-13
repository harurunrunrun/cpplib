---
title: Polynomial Discrete Sum (多項式の離散和)
documentation_of: ../src/algorithm/math/polynomial_discrete_sum.hpp
---

Faulhaber 多項式と、係数表示された多項式の離散不定和を扱う。

```cpp
#include "src/algorithm/math/polynomial_discrete_sum.hpp"

using mint = Modint998244353;
math::PolynomialDiscreteSum<mint, 200000> summation;
```

すべての prefix sum は半開区間 $[0,n)$、すなわち添字 `0` から `n - 1` までを
加える。

## 冪和

```cpp
mint answer = summation.power_sum(n, exponent);
```

非負整数 `n` に対して

$$
\sum_{i=0}^{n-1} i^{exponent}
$$

を返す。`exponent == 0` では $0^0=1$ として `n` を返す。

```cpp
mint answer = summation.faulhaber(mint_n, exponent);
```

同じ冪和を表す次数 `exponent + 1` の Faulhaber 多項式を `mint_n` で評価する。
`power_sum` は非負整数を `Mint` へ変換してこの関数を呼ぶ。

実装は $S(t)=\sum_{i=0}^{t-1}i^k$ を `t = 0,1,...,k+1` で作り、
連続整数点用 Lagrange 補間で評価する。

## 離散不定和

```cpp
auto antiderivative = summation.discrete_antiderivative(
    coefficients, coefficient_count
);
```

$$
f(X)=\sum_{i=0}^{coefficient\_count-1} coefficients_i X^i
$$

に対して、`F(0) == 0` かつ

$$
F(X+1)-F(X)=f(X)
$$

を満たす `F` の係数を定数項から昇冪順で返す。返り値の有効な要素数は
`coefficient_count + 1` である。

```cpp
mint answer = summation.polynomial_prefix_sum(
    coefficients, coefficient_count, n
);
```

`F(n)`、すなわち $\sum_{i=0}^{n-1}f(i)$ を返す。`n` には非負 `long long`
または `Mint` を指定できる。多数の `n` を評価する場合は
`discrete_antiderivative` を1回だけ呼び、返り値を
`math::polynomial_evaluate` で評価する。

## API別の時間計算量・空間計算量

`Mint` の四則演算を $O(1)$、`Mint::pow(k)` を $O(\log(k+1))$ とする。
`k = exponent`、`d = coefficient_count` とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `PolynomialDiscreteSum()` | $O(MAX\_DEGREE)$ | オブジェクトに $O(MAX\_DEGREE)$ |
| `faulhaber` | $O(k\log(k+1))$ | $O(MAX\_DEGREE)$ |
| `power_sum` | $O(k\log(k+1))$ | $O(MAX\_DEGREE)$ |
| `discrete_antiderivative` | $O(d^2)$ | $O(MAX\_DEGREE)$ |
| `polynomial_prefix_sum` | $O(d^2)$ | $O(MAX\_DEGREE)$ |
| 構築済み離散不定和の `polynomial_evaluate` | $O(d)$ | $O(1)$ |

`exponent == 0` の `faulhaber` と `power_sum` は $O(1)$。

## 注意点

- `Mint` は多項式補間で要求される演算に加えて `pow(long long)` を持つ。
- constructor は `(MAX_DEGREE + 1)!` の逆元を求める。通常の素数 modint では
  `MAX_DEGREE + 1 < mod` が必要。
- `power_sum` と `polynomial_prefix_sum` の `long long` 版は `n >= 0` を要求する。
- 係数数は `MAX_DEGREE + 1` 以下、冪指数は `MAX_DEGREE` 以下でなければならない。
- 合成数 mod では途中に現れる整数が逆元を持たず、構築または除算が失敗することがある。
