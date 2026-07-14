---
title: Formal Power Series (形式的冪級数) [exp_of_formal_power_series] [inv_of_formal_power_series] [log_of_formal_power_series] [pow_of_formal_power_series] [sqrt_of_formal_power_series]
documentation_of: ../src/algorithm/fft/formal_power_series.hpp
---

`Modint<MOD>` を係数とする1変数形式的冪級数の基本演算。
積は既存のNTTを使用し、逆数・指数・平方根などはNewton反復で計算する。

以下で `size` は返す係数数、$N=\mathtt{size}$ とする。`size==0` の場合、
高度な演算も定数項の前提を検査せず空列を返す。入力が短い部分は0、長い部分は
`size` で打ち切って扱う。

## `fps_derivative`

```cpp
template<int MOD>
std::vector<Modint<MOD>> fps_derivative(
    const std::vector<Modint<MOD>>& values
)
```

形式微分を返す。長さ0、1の入力に対する結果は空列。時間 $O(N)$、領域 $O(N)$。

## `fps_integral`

```cpp
template<int MOD>
std::vector<Modint<MOD>> fps_integral(
    const std::vector<Modint<MOD>>& values
)
```

定数項を0とした形式積分を返す。出力長は入力長+1。係数 $i$ で割るため、
$1,2,\ldots,N$ が `MOD` 上で可逆でなければならず、非可逆なら `Modint::inv` の
`runtime_error` が伝播する。時間 $O(N\log MOD)$、領域 $O(N)$。

## `fps_inv`

```cpp
template<int MOD>
std::vector<Modint<MOD>> fps_inv(
    const std::vector<Modint<MOD>>& values,
    std::size_t size
)
```

$g\,f\equiv1\pmod{x^N}$ となる $g$ を返す。$N>0$ では `values[0]` が
非零かつ可逆でなければならない。定数項が0なら `runtime_error`。
Newton反復 $g\leftarrow g(2-fg)$ を用い、時間 $O(N\log N)$、領域 $O(N)$。

## `fps_log`

```cpp
template<int MOD>
std::vector<Modint<MOD>> fps_log(
    const std::vector<Modint<MOD>>& values,
    std::size_t size
)
```

$\log f=\int f'/f$ を返す。$N>0$ では定数項が1でなければならず、違反時は
`runtime_error`。時間 $O(N\log N)$、領域 $O(N)$。

## `fps_exp`

```cpp
template<int MOD>
std::vector<Modint<MOD>> fps_exp(
    const std::vector<Modint<MOD>>& values,
    std::size_t size
)
```

$\exp f$ を返す。$N>0$ では定数項が0でなければならず、違反時は
`runtime_error`。Newton反復 $g\leftarrow g(1+f-\log g)$ を使う。
時間 $O(N\log N)$、領域 $O(N)$。

## `fps_pow`

```cpp
template<int MOD>
std::vector<Modint<MOD>> fps_pow(
    const std::vector<Modint<MOD>>& values,
    long long exponent,
    std::size_t size
)
```

$f^{\mathtt{exponent}}$ を返す。指数は非負でなければならず、負なら
`runtime_error`。指数0では入力に関係なく $1$ を返す。先頭の非零係数までの次数を
$v$ とすると、$v\cdot exponent\ge N$ なら零級数。そうでなければ
$x^v c$ を分離し、`exp(exponent * log(f/(x^v c)))` で計算する。
時間 $O(N\log N)$、領域 $O(N)$。

## `fps_sqrt`

```cpp
template<int MOD>
std::optional<std::vector<Modint<MOD>>> fps_sqrt(
    const std::vector<Modint<MOD>>& values,
    std::size_t size
)
```

$g^2\equiv f\pmod{x^N}$ となる $g$ を返す。存在しなければ `nullopt`。
零級数の平方根には零級数を返す。最初の非零次数が奇数、またはその係数が
平方非剰余なら存在しない。定数係数の平方根はTonelli--Shanksで求め、2で割る
Newton反復 $g\leftarrow(g+f/g)/2$ を行うため、奇素数modulusを前提とする。
標数2では非零入力に `runtime_error`。時間 $O(N\log N+\log^2 MOD)$、領域 $O(N)$。

## 注意点

高度な演算では `MOD` が素数で、必要な整数が可逆であることを前提とする。
NTTで使用する各2冪長が `MOD-1` を割り切る必要がある。Newton反復内の積では
要求出力長より大きい変換長を使う場合があり、利用可能なNTT長を超えると
`runtime_error`。`size > INT_MAX` または変換長を `int` で表せない場合は
`length_error`。`convolution_ntt` や `Modint::inv` の例外はそのまま伝播する。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `fps_derivative` | $O(N)$ | $O(N)$ |
| `fps_integral` | $O(N\log MOD)$ | $O(N)$ |
| `fps_inv` | $O(N\log N)$ | $O(N)$ |
| `fps_log` | $O(N\log N)$ | $O(N)$ |
| `fps_exp` | $O(N\log N)$ | $O(N)$ |
| `fps_pow` | $O(N\log N)$ | $O(N)$ |
| `fps_sqrt` | $O(N\log N+\log^2 MOD)$ | $O(N)$ |
