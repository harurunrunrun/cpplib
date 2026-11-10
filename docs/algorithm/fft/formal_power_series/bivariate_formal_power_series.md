---
title: Bivariate Formal Power Series (二変数形式的冪級数)
documentation_of: ../../../../src/algorithm/fft/formal_power_series/bivariate_formal_power_series.hpp
---

$x$ についての次数を `XDegree`、$y$ についての次数を `YDegree` でそれぞれ打ち切った二変数形式的冪級数である。積・逆数・指数・対数・整数冪・平方根を、長方形領域 $0 \le i \le \texttt{XDegree}$, $0 \le j \le \texttt{YDegree}$ の外を切り捨てて計算する。

```cpp
template<class T,
         std::size_t XDegree = static_cast<std::size_t>(-1),
         std::size_t YDegree = static_cast<std::size_t>(-1)>
struct BivariateFormalPowerSeries;
```

容量テンプレート引数を明示すると係数を固定長の `std::array` に保持する。両方を省略すると実行時に次数を指定する可変長版になる（後述）。

`T` は体の要素として四則演算・単項マイナス・`T{}`・`T{1}`・`long long` からの構築・等値比較が使える必要がある。指数・対数は $1, 2, \ldots, \texttt{XDegree} + \texttt{YDegree}$ が可逆であることを要する。

以下、$X = \texttt{XDegree} + 1$, $Y = \texttt{YDegree} + 1$ とする。

## 構築と係数アクセス

```cpp
BivariateFormalPowerSeries()
explicit BivariateFormalPowerSeries(const std::array<T, coefficient_count>& values)
T& operator()(std::size_t x, std::size_t y)
const T& operator()(std::size_t x, std::size_t y) const
const T& constant_term() const
const std::array<T, coefficient_count>& data() const
static BivariateFormalPowerSeries constant(const T& value)
```

- デフォルトコンストラクタは零級数を作る。`values` を渡すと、`values[x * (YDegree + 1) + y]` を $x^{\texttt{x}} y^{\texttt{y}}$ の係数とする（行優先）。
- `operator()(x, y)` は $x^{\texttt{x}} y^{\texttt{y}}$ の係数を返す。範囲外なら `std::runtime_error` を送出する。
- `data()` は行優先に並んだ係数配列を返す。
- `constant(value)` は定数項だけが `value` の級数を作る。

時間計算量は `operator()`・`constant_term`・`data` が $O(1)$、構築・`constant` が $O(XY)$ である。

## 加減算・乗算

```cpp
operator+=, operator-=, operator*=
operator+, operator-, operator*
operator==
```

長方形領域で打ち切った演算を行う。

- 加減算: $O(XY)$ 時間、追加空間 $O(1)$
- 乗算: $O(X^2 Y^2)$ 時間、追加空間 $O(XY)$

## `inverse`

```cpp
BivariateFormalPowerSeries inverse() const
```

積が $1$ になる逆級数を、係数の漸化式で全次数の小さい順に求めて返す。定数項が $0$ なら `std::runtime_error` を送出する。

- 時間計算量: $O(X^2 Y^2)$
- 追加空間計算量: $O(XY)$

## `logarithm`

```cpp
BivariateFormalPowerSeries logarithm() const
```

$f = 1 + h$ に対し $\log f = \sum_{k \ge 1} (-1)^{k+1} h^k / k$ を計算する。定数項が $1$ でなければ `std::runtime_error` を送出する。

- 時間計算量: $O((X + Y) X^2 Y^2)$
- 追加空間計算量: $O(XY)$

## `exponential`

```cpp
BivariateFormalPowerSeries exponential() const
```

$\exp f = \sum_{k \ge 0} f^k / k!$ を計算する。定数項が $0$ でなければ `std::runtime_error` を送出する。

- 時間計算量: $O((X + Y) X^2 Y^2)$
- 追加空間計算量: $O(XY)$

## `pow`

```cpp
BivariateFormalPowerSeries pow(long long exponent) const
```

繰り返し二乗法で整数冪を返す。指数が負なら先に `inverse` を取ってから冪乗する。$0$ 乗は $1$ である。

- 時間計算量: $O(X^2 Y^2 \log(\lvert \texttt{exponent} \rvert + 1))$。指数が負なら逆数計算の $O(X^2 Y^2)$ が加わる
- 追加空間計算量: $O(XY)$

## `square_root`

```cpp
BivariateFormalPowerSeries square_root(const T& constant_square_root) const
```

定数項が `constant_square_root` で、平方が元の級数に一致する平方根を係数の漸化式で求めて返す。`constant_square_root` の平方が定数項と異なる場合、または `constant_square_root + constant_square_root` が $0$ の場合は `std::runtime_error` を送出する。

- 時間計算量: $O(X^2 Y^2)$
- 追加空間計算量: $O(XY)$

## 可変長版

容量テンプレート引数を両方とも省略すると、実行時に与えた次数に合わせて `std::vector` に係数を保持する。

```cpp
BivariateFormalPowerSeries<Mint> dynamic(x_degree, y_degree);
BivariateFormalPowerSeries<Mint> from_values(x_degree, y_degree, values);
BivariateFormalPowerSeries<Mint, 10, 20> fixed;
auto one = BivariateFormalPowerSeries<Mint>::constant(1, x_degree, y_degree);
```

- コンストラクタ `(x_degree = 0, y_degree = 0)` は零級数を作る。係数の個数が `std::size_t` に収まらない場合は `std::length_error` を送出する。
- `(x_degree, y_degree, values)` は行優先の係数列 `values` から構築する。`values.size()` が $(\texttt{x_degree} + 1)(\texttt{y_degree} + 1)$ と異なれば `std::invalid_argument` を送出する。
- `data()` は `const std::vector<T>&` を返す。メンバ変数 `x_degree`, `y_degree` で次数を参照できる。
- 加減乗算では両辺の次数が一致している必要があり、異なれば `std::invalid_argument` を送出する。`operator==` は次数も比較する。

$X = \texttt{x_degree} + 1$, $Y = \texttt{y_degree} + 1$ として、構築と保存は $O(XY)$ であり、その他の演算は固定長版と同じ計算量になる。

## 注意点

各演算は長方形領域で打ち切る。指数・対数の級数は、定数項が $0$ の級数の $k$ 乗が全次数 $k$ 未満の項を持たないことを使い、$k = \texttt{XDegree} + \texttt{YDegree}$ で打ち切る。
