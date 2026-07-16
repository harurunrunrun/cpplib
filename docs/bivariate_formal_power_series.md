---
title: Bivariate Formal Power Series (二変数形式的冪級数)
documentation_of: ../src/algorithm/fft/bivariate_formal_power_series.hpp
---

$x$ 次数を `XDegree`、$y$ 次数を `YDegree` でそれぞれ打ち切る二変数形式的
冪級数。係数は固定長 `std::array` に保持する。積・逆数・指数・対数・整数冪・
平方根を長方形領域の外を切り捨てて計算する。

```cpp
template<class T, std::size_t XDegree, std::size_t YDegree>
struct BivariateFormalPowerSeries;
```

`T` は体の要素として四則演算、単項minus、`T{0}`, `T{1}`、整数からの構築、
等値比較を使用できる必要がある。必要な小整数が可逆でなければ指数・対数は
使用できない。

## 係数アクセス

```cpp
T& operator()(std::size_t x, std::size_t y)
const T& operator()(std::size_t x, std::size_t y) const
const T& constant_term() const
const std::array<T, coefficient_count>& data() const
static BivariateFormalPowerSeries constant(const T& value)
```

`operator()` は $[x^x y^y]$ の係数を返す。範囲外なら `runtime_error`。
`constant` は定数項だけが `value` の級数を作る。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$（`constant` の戻り値は $O(XY)$）

## 加減算・乗算

```cpp
operator+=, operator-=, operator*=
operator+, operator-, operator*
```

長方形領域で打ち切った演算を行う。

- 加減算: $O(XY)$
- 乗算: $O(X^2Y^2)$
- 追加空間計算量: 加減算 $O(1)$、乗算 $O(XY)$

ここで $X=XDegree+1$, $Y=YDegree+1$。

## `inverse`

```cpp
BivariateFormalPowerSeries inverse() const
```

積が1になる逆級数を係数の漸化式で返す。定数項0なら `runtime_error`。

- 時間計算量: $O(X^2Y^2)$
- 追加空間計算量: $O(XY)$

## `logarithm`

```cpp
BivariateFormalPowerSeries logarithm() const
```

$\log(1+h)=\sum_{k\ge1}(-1)^{k+1}h^k/k$ を計算する。定数項が1でなければ
`runtime_error`。

- 時間計算量: $O((X+Y)X^2Y^2)$
- 追加空間計算量: $O(XY)$

## `exponential`

```cpp
BivariateFormalPowerSeries exponential() const
```

$\exp(f)=\sum_{k\ge0}f^k/k!$ を計算する。定数項が0でなければ
`runtime_error`。

- 時間計算量: $O((X+Y)X^2Y^2)$
- 追加空間計算量: $O(XY)$

## `pow`

```cpp
BivariateFormalPowerSeries pow(long long exponent) const
```

2進冪乗で整数冪を返す。負の指数では先に `inverse` を使う。0乗は1。

- 時間計算量: $O(X^2Y^2\log(|exponent|+1))$。負数では逆数計算を加える
- 追加空間計算量: $O(XY)$

## `square_root`

```cpp
BivariateFormalPowerSeries square_root(
    const T& constant_square_root
) const
```

指定した定数項を持ち、平方が元の級数になる平方根を係数の漸化式で返す。
引数の平方が定数項と異なる場合、または `2*constant_square_root` が0で
除算できない場合は `runtime_error`。

- 時間計算量: $O(X^2Y^2)$
- 追加空間計算量: $O(XY)$

## 注意点

各演算は $0\le i\le XDegree$, $0\le j\le YDegree$ の長方形で打ち切る。
指数・対数の級数は、定数項0の級数の $k$ 乗がtotal degree $k$ 未満を持たない
ことを使い、$k=XDegree+YDegree$ で停止する。

