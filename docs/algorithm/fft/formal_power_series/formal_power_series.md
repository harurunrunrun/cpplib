---
title: Formal Power Series (形式的冪級数) [composition_of_formal_power_series_large] [compositional_inverse_of_formal_power_series] [compositional_inverse_of_formal_power_series_large] [exp_of_formal_power_series] [inv_of_formal_power_series] [log_of_formal_power_series] [pow_of_formal_power_series] [sqrt_of_formal_power_series]
documentation_of: ../../../../src/algorithm/fft/formal_power_series/formal_power_series.hpp
---

`Modint<MOD>` を係数とする一変数形式的冪級数の基本演算である。積には `convolution_ntt` を用い、逆数・指数・平方根などは Newton 法で計算する。

以下、`size` は返す係数の個数で、$N = \texttt{size}$ とする。入力が `size` より短ければ不足分を $0$ とみなし、長ければ `size` で打ち切って扱う。`size == 0` のとき、`fps_inv`・`fps_log`・`fps_exp`・`fps_pow`・`fps_sqrt` は定数項の前提を検査せずに空列を返す（ただし `fps_pow` の負の指数は `size == 0` でも例外になる）。

## `fps_derivative`

```cpp
template<int MOD>
std::vector<Modint<MOD>> fps_derivative(const std::vector<Modint<MOD>>& values)
```

形式微分を返す。出力の長さは入力の長さから $1$ を引いたもので、長さ $0$ または $1$ の入力に対しては空列を返す。

## `fps_integral`

```cpp
template<int MOD>
std::vector<Modint<MOD>> fps_integral(const std::vector<Modint<MOD>>& values)
```

定数項を $0$ とした形式積分を返す。出力の長さは入力の長さに $1$ を足したものである。$i$ 次の係数を $i$ で割るため、入力の長さを $N$ として $1, 2, \ldots, N$ が `MOD` 上で可逆でなければならない。可逆でなければ `Modint::inv` の `std::runtime_error` がそのまま伝播する。

## `fps_inv`

```cpp
template<int MOD>
std::vector<Modint<MOD>> fps_inv(const std::vector<Modint<MOD>>& values, std::size_t size)
```

$f g \equiv 1 \pmod{x^N}$ を満たす $g$ を返す。$N \gt 0$ のとき定数項 `values[0]` は $0$ であってはならず、$0$（または `values` が空）なら `std::runtime_error` を送出する。Newton 法 $g \leftarrow g(2 - fg)$ で計算する。

## `fps_log`

```cpp
template<int MOD>
std::vector<Modint<MOD>> fps_log(const std::vector<Modint<MOD>>& values, std::size_t size)
```

$\log f = \int f' / f \, dx$ を返す。$N \gt 0$ のとき定数項は $1$ でなければならず、違反すると `std::runtime_error` を送出する。

## `fps_exp`

```cpp
template<int MOD>
std::vector<Modint<MOD>> fps_exp(const std::vector<Modint<MOD>>& values, std::size_t size)
```

$\exp f$ を返す。$N \gt 0$ のとき定数項は $0$ でなければならず、違反すると `std::runtime_error` を送出する。Newton 法 $g \leftarrow g(1 + f - \log g)$ で計算する。$g^{-1}$ も各段で Newton 法により更新しながら持ち回すため、$\log g$ を毎回最初から計算することはない。

## `fps_pow`

```cpp
template<int MOD>
std::vector<Modint<MOD>> fps_pow(const std::vector<Modint<MOD>>& values, long long exponent, std::size_t size)
```

$f^{\texttt{exponent}}$ を返す。指数は非負でなければならず、負なら `std::runtime_error` を送出する。指数が $0$ なら入力によらず $1$ を返す。

最初の非零係数の次数を $v$、その係数を $c$ とする。$v \cdot \texttt{exponent} \ge N$ なら零級数を返す。そうでなければ $f = c x^v h$ と分解し、$c^{\texttt{exponent}} x^{v \cdot \texttt{exponent}} \exp(\texttt{exponent} \cdot \log h)$ として計算する。

## `fps_sqrt`

```cpp
template<int MOD>
std::optional<std::vector<Modint<MOD>>> fps_sqrt(const std::vector<Modint<MOD>>& values, std::size_t size)
```

$g^2 \equiv f \pmod{x^N}$ を満たす $g$ を返し、存在しなければ `std::nullopt` を返す。

- 零級数（打ち切った範囲がすべて $0$）に対しては零級数を返す。
- 最初の非零係数の次数が奇数、またはその係数が平方非剰余なら `std::nullopt` を返す。
- 最初の非零係数の平方根は Tonelli-Shanks 法で求め、2 つの候補のうち代表値が小さい方を使う。
- その後 Newton 法 $g \leftarrow (g + f / g) / 2$ を行う（$g^{-1}$ も各段で更新しながら持ち回す）ため、`MOD` は奇素数であることを前提とする。`MOD == 2` のとき、最初の非零係数の次数が偶数の入力に対しては `std::runtime_error` を送出する。

## 計算量

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `fps_derivative` | $O(N)$ | $O(N)$ |
| `fps_integral` | $O(N)$ | $O(N)$ |
| `fps_inv` | $O(N \log N)$ | $O(N)$ |
| `fps_log` | $O(N \log N)$ | $O(N)$ |
| `fps_exp` | $O(N \log N)$ | $O(N)$ |
| `fps_pow` | $O(N \log N + \log \texttt{MOD})$ | $O(N)$ |
| `fps_sqrt` | $O(N \log N + \log^2 \texttt{MOD})$ | $O(N)$ |

`fps_derivative` と `fps_integral` の $N$ は入力の長さである。`MOD` が素数で入力の長さが `MOD` 未満のとき、`fps_integral` は $1, 2, \ldots, N$ の逆元を線形時間の漸化式 $i^{-1} = -\lfloor \texttt{MOD} / i \rfloor \cdot (\texttt{MOD} \bmod i)^{-1}$ で前計算する。それ以外の場合は各係数で `Modint::inv` を呼ぶため $O(N \log \texttt{MOD})$ かかる（`fps_log`・`fps_exp`・`fps_pow` も同様）。

## 注意点

- `fps_inv` 以降の演算では、`MOD` が素数で、必要な整数が可逆であることを前提とする。
- NTT で使う各 2 の冪の変換長が `MOD - 1` を割り切る必要がある。Newton 法の中の積では要求された出力長より大きい変換長を使うことがあり、利用可能な NTT 長を超えると `std::runtime_error` を送出する。
- `size` が `INT_MAX` を超える場合、または変換長を `int` で表せない場合は `std::length_error` を送出する。
- `convolution_ntt` や `Modint::inv` の例外はそのまま伝播する。
