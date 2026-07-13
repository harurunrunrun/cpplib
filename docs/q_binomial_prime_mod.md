---
title: q-Binomial Coefficient (Prime Mod)
documentation_of: ../src/algorithm/math/q_binomial_prime_mod.hpp
---

素数 $p$ と $0\leq q<p$ を固定し、Gaussian 二項係数
$\binom{n}{k}_q$ を $p$ で割った余りを求める。

$$
[i]_q=1+q+\cdots+q^{i-1},\qquad
[i]_q!=\prod_{j=1}^{i}[j]_q
$$

を前計算する。$[D]_q=0$ となる最小の正整数 $D$ が前計算範囲内に
存在する場合は、$q$-Lucas の定理

$$
\binom{aD+b}{cD+d}_q=
\binom{a}{c}\binom{b}{d}_q \pmod p
\quad (0\leq b,d<D)
$$

を使う。

## `QBinomialPrimeMod`

```cpp
QBinomialPrimeMod(
    std::uint32_t prime_modulus,
    std::uint32_t q,
    std::size_t maximum_n
)
```

$0\leq n\leq$ `maximum_n` のクエリを前計算する。
`prime_modulus` は素数、`q < prime_modulus`、
`maximum_n < prime_modulus` でなければならない。条件を満たさない場合は
`std::invalid_argument` を投げる。

`maximum_n` を $L$ とする。構築時間は $O(\sqrt p+L)$、領域計算量は
$O(L)$ である。実際には $D$ が範囲内にある場合、階乗表の要素数は
$O(D+L/D)$ となる。

### `modulus`

```cpp
std::uint32_t modulus() const noexcept
```

法 $p$ を返す。時間計算量・追加領域は $O(1)$。

### `base`

```cpp
std::uint32_t base() const noexcept
```

$q$ を返す。時間計算量・追加領域は $O(1)$。

### `maximum_n`

```cpp
std::size_t maximum_n() const noexcept
```

構築時に指定した `maximum_n` を返す。時間計算量・追加領域は $O(1)$。

### `period`

```cpp
std::size_t period() const noexcept
```

$[D]_q=0$ となる最小の正整数 $D$ が `maximum_n` 以下なら $D$、
存在しなければ $0$ を返す。時間計算量・追加領域は $O(1)$。

### `binomial`

```cpp
std::uint32_t binomial(std::size_t n, std::size_t k) const
```

$\binom{n}{k}_q\bmod p$ を返す。$k>n$ なら $0$ を返す。
`n > maximum_n` なら `std::out_of_range` を投げる。

時間計算量・追加領域は $O(1)$。
