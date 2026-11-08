---
title: Pólya Exponential (ポリア指数)
documentation_of: ../../../../src/algorithm/fft/formal_power_series/polya_exponential.hpp
---

定数項が $0$ の形式的冪級数 $f(x)$ に対し、Pólya 指数（plethystic exponential）

$$
\operatorname{PExp}(f)(x)
=\exp\left(\sum_{k\ge 1}\frac{f(x^k)}{k}\right)
$$

を計算する。非標識組合せ構造の多重集合構成や Euler 変換に利用できる。

## `polya_exponential`

```cpp
template<int MOD>
std::vector<Modint<MOD>> polya_exponential(
    const std::vector<Modint<MOD>>& values,
    std::size_t size
)
```

`values` を $f$ の係数列として、Pólya 指数の $x^{\texttt{size}}$ 未満の係数を返す。足りない入力係数は $0$ とみなし、余分な入力係数は無視する。

```cpp
template<int MOD>
std::vector<Modint<MOD>> polya_exponential(const std::vector<Modint<MOD>>& values)
```

出力長を `values.size()` とする省略形である。

## `polya_logarithm`

```cpp
template<int MOD>
std::vector<Modint<MOD>> polya_logarithm(
    const std::vector<Modint<MOD>>& values,
    std::size_t size
)
```

定数項が $1$ の級数 $F$ に対し、Pólya 指数の逆変換

$$
\operatorname{PLog}(F)(x)
=\sum_{k\ge1}\frac{\mu(k)}{k}\log F(x^k)
$$

の $x^{\texttt{size}}$ 未満の係数を返す。

```cpp
template<int MOD>
std::vector<Modint<MOD>> polya_logarithm(const std::vector<Modint<MOD>>& values)
```

出力長を `values.size()` とする省略形である。

## 計算量

出力長を $N$ とする。

| 操作 | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `polya_exponential`（両オーバーロード） | $O(N \log N \log \texttt{MOD})$ | $O(N)$ |
| `polya_logarithm`（両オーバーロード） | $O(N \log N + N \log \texttt{MOD})$ | $O(N)$ |

`polya_exponential` では $\sum_k f(x^k)/k$ の各項（調和級数個）で `Modint` の除算を行うため $\log \texttt{MOD}$ の因子がかかる。`fps_exp` と `fps_log` 自体は $O(N \log N)$ である。

## 注意点

- `MOD` は素数で、`size <= MOD` でなければならない（途中で $1, 2, \ldots, N - 1$ の逆元を用いる）。`size > MOD` なら `std::invalid_argument` を送出する。
- `polya_exponential` は定数項 $0$ を要求し、違反すると `std::invalid_argument` を送出する。
- `polya_logarithm` は定数項 $1$ を要求し、違反すると `std::invalid_argument` を送出する。
- `size == 0` なら入力によらず空列を返す。
- 次数が大きい場合は、必要な NTT 長を法が許容している必要がある（`fps_exp`・`fps_log` の制約に従う）。
