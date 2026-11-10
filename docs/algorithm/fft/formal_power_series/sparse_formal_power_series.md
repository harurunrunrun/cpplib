---
title: Sparse Formal Power Series (疎な形式的冪級数) [exp_of_formal_power_series_sparse] [inv_of_formal_power_series_sparse] [log_of_formal_power_series_sparse] [pow_of_formal_power_series_sparse] [sqrt_of_formal_power_series_sparse]
documentation_of: ../../../../src/algorithm/fft/formal_power_series/sparse_formal_power_series.hpp
---

非零項だけで与えられる形式的冪級数を扱う。項は `(degree, coefficient)` の組で表し、各演算は先頭 `size` 項を密な `std::vector<Modint<MOD>>` として返す。入力の順序は任意で、同じ次数を複数回与えてもよい。各演算は同じ次数の係数を足し合わせ、係数が $0$ になった項を除去してから計算する。

`MOD` は素数でなければならない（`static_assert` で検査する）。以下、$N = \texttt{size}$、正規化後に次数 $N$ 未満に残る項数を $K$、入力の項数を $R$ とする。正規化に $O(R \log R)$ 時間、各漸化式に $O(NK)$ 時間かかる。NTT などを使う密な形式的冪級数への変換は行わない。

## `SparseFormalPowerSeriesTerm`

```cpp
template<int MOD>
using SparseFormalPowerSeriesTerm =
    std::pair<std::size_t, Modint<MOD>>;
```

`first` が次数、`second` がその次数の係数である。

## `sparse_fps_normalize`

```cpp
template<int MOD>
std::vector<SparseFormalPowerSeriesTerm<MOD>> sparse_fps_normalize(
    std::vector<SparseFormalPowerSeriesTerm<MOD>> terms
)
```

項を次数の昇順に並べ、同じ次数の係数を足し合わせ、係数が $0$ の項を除去して返す。この関数自体は次数による切り捨てを行わない。

- 時間計算量: $O(R\log R)$
- 追加空間計算量: $O(R)$

## `sparse_fps_inv`

```cpp
template<int MOD>
std::vector<Modint<MOD>> sparse_fps_inv(
    const std::vector<SparseFormalPowerSeriesTerm<MOD>>& terms,
    std::size_t size
)
```

$f(x) g(x) \equiv 1 \pmod{x^N}$ を満たす $g$ の先頭 $N$ 項を返す。$N \gt 0$ のとき $f$ の定数項は非零でなければならず、満たさない場合は `std::runtime_error` を送出する。$N = 0$ なら空列を返す。

$g_0=f_0^{-1}$ とし、

$$
g_n=-f_0^{-1}\sum_{1\le d\le n}f_dg_{n-d}
$$

で計算する。この演算は次数で割らないため、$N$ と `MOD` の大小関係に制約はない。

- 時間計算量: $O(R\log R+NK+\log\texttt{MOD})$
- 追加空間計算量: $O(N+K)$

## `sparse_fps_log`

```cpp
template<int MOD>
std::vector<Modint<MOD>> sparse_fps_log(
    const std::vector<SparseFormalPowerSeriesTerm<MOD>>& terms,
    std::size_t size
)
```

$\log f$ の先頭 $N$ 項を返す。$N \gt 0$ のとき $f_0 = 1$ が必要であり、満たさない場合は `std::runtime_error` を送出する。また、$1, \ldots, N - 1$ で割れるように $N \le \texttt{MOD}$ が必要である。$N = 0$ なら条件を検査せずに空列を返す。

$f (\log f)' = f'$ の係数比較から得られる漸化式を使う。

- 時間計算量: $O(R\log R+NK)$
- 追加空間計算量: $O(N+K)$

## `sparse_fps_exp`

```cpp
template<int MOD>
std::vector<Modint<MOD>> sparse_fps_exp(
    const std::vector<SparseFormalPowerSeriesTerm<MOD>>& terms,
    std::size_t size
)
```

$\exp f$ の先頭 $N$ 項を返す。$N \gt 0$ のとき $f_0 = 0$ が必要であり、満たさない場合は `std::runtime_error` を送出する。$N \le \texttt{MOD}$ も必要である。$N = 0$ なら空列を返す。

$g_0 = 1$ として $g' = f' g$、すなわち

$$
g_n=\frac1n\sum_{1\le d\le n}d f_dg_{n-d}
$$

を使う。

- 時間計算量: $O(R\log R+N(K+1))$
- 追加空間計算量: $O(N+K)$

## `sparse_fps_pow`

```cpp
template<int MOD>
std::vector<Modint<MOD>> sparse_fps_pow(
    const std::vector<SparseFormalPowerSeriesTerm<MOD>>& terms,
    long long exponent,
    std::size_t size
)
```

$f^{\texttt{exponent}}$ の先頭 $N$ 項を返す。指数は非負でなければならず、負なら（$N = 0$ でも）`std::runtime_error` を送出する。$N \gt 0$ のとき $N \le \texttt{MOD}$ が必要である。指数が $0$ なら入力によらず $1$ を返す。

最初の非零項を $c x^v$ とする。$v \cdot \texttt{exponent} \ge N$ ならすべて $0$ を返す。そうでなければ $u = f / (c x^v)$ とおき、$h = u^{\texttt{exponent}}$ を $u h' = \texttt{exponent} \cdot u' h$ から得られる漸化式で計算する。先頭の $0$、零多項式、大きな指数でも次数の計算がオーバーフローしないように処理している。

- 時間計算量: $O(R\log R+N(K+1)+\log\texttt{MOD}+\log(\texttt{exponent}+1))$
- 追加空間計算量: $O(N+K)$

## `sparse_fps_sqrt`

```cpp
template<int MOD>
std::optional<std::vector<Modint<MOD>>> sparse_fps_sqrt(
    const std::vector<SparseFormalPowerSeriesTerm<MOD>>& terms,
    std::size_t size
)
```

$g^2 \equiv f \pmod{x^N}$ を満たす $g$ を返し、存在しなければ `std::nullopt` を返す。零級数の平方根としては零級数を返す。最初の非零項の次数が奇数、またはその係数が平方非剰余の場合は平方根が存在しない。先頭係数の平方根は Tonelli-Shanks 法で求め、2 つの候補のうち代表値が小さい方を選ぶ。

$N \gt 0$ のとき $N \le \texttt{MOD}$ でなければならず、$N \gt \texttt{MOD}$ なら（零級数でも）`std::runtime_error` を送出する。また非零入力では `MOD` が奇素数でなければならず、`MOD == 2` なら `std::runtime_error` を送出する。$N = 0$ なら空列を返す。

$f = x^{2v} u$, $g = x^v q$ とし、$u q' = \frac{1}{2} u' q$ の係数比較を使う。次数 $N$ 以上の係数を $0$ とみなした入力に対する平方根を返すので、返り値は常にちょうど $N$ 項である。

- 時間計算量: $O(R\log R+N(K+1)+\log^2\texttt{MOD})$
- 追加空間計算量: $O(N+K)$

## 注意点

- 各演算では次数 $N$ 以上の入力項を無視する。
- 定数項の条件は、同じ次数の係数の加算と係数 $0$ の項の除去を行った後に判定する。
- `sparse_fps_log`・`sparse_fps_exp`・`sparse_fps_pow`・`sparse_fps_sqrt` で $N \gt \texttt{MOD}$ を指定すると、次数 $\texttt{MOD}$ の逆元が存在せず漸化式を計算できないため `std::runtime_error` を送出する。
