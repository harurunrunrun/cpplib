---
title: Sparse Formal Power Series (疎な形式的冪級数)
documentation_of: ../src/algorithm/fft/sparse_formal_power_series.hpp
---

非零項だけが与えられる形式的冪級数を扱う。
項は `(degree, coefficient)` で表し、各演算は先頭 `size` 項を密な
`std::vector<Modint<MOD>>` として返す。入力順は任意で、同じ次数を複数回与えてもよい。
各演算は重複次数を加算し、係数が 0 になった項を除去してから計算する。

`MOD` は素数でなければならない。以下で $N=\mathtt{size}$、正規化後に
次数 $N$ 未満に残る項数を $K$、入力項数を $R$ とする。
正規化に $O(R\log R)$、各漸化式に $O(NK)$ 時間がかかる。
NTT などを使う密な形式的冪級数へ変換しない。

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

項を次数の昇順に並べ、同じ次数の係数を加算し、0 係数を除去して返す。
この関数自体は次数による切り捨てを行わない。

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

$f(x)g(x)\equiv1\pmod{x^N}$ となる $g$ の先頭 $N$ 項を返す。
$N>0$ では $f$ の定数項が非零でなければならず、満たさない場合は
`runtime_error` を送出する。$N=0$ では空列を返す。

$g_0=f_0^{-1}$ とし、

$$
g_n=-f_0^{-1}\sum_{1\le d\le n}f_dg_{n-d}
$$

で計算する。この演算は次数で割らないため、$N$ と `MOD` の大小に制約はない。

- 時間計算量: $O(R\log R+NK+\log\mathtt{MOD})$
- 追加空間計算量: $O(N+K)$

## `sparse_fps_log`

```cpp
template<int MOD>
std::vector<Modint<MOD>> sparse_fps_log(
    const std::vector<SparseFormalPowerSeriesTerm<MOD>>& terms,
    std::size_t size
)
```

$\log f$ の先頭 $N$ 項を返す。$N>0$ では $f_0=1$ が必要であり、
満たさない場合は `runtime_error` を送出する。また $1,\ldots,N-1$ を
法 `MOD` で割れるように $N\le\mathtt{MOD}$ が必要である。
$N=0$ では条件を検査せず空列を返す。

$f(\log f)'=f'$ の係数比較による漸化式を使う。

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

$\exp f$ の先頭 $N$ 項を返す。$N>0$ では $f_0=0$ が必要であり、
満たさない場合は `runtime_error` を送出する。$N\le\mathtt{MOD}$ も必要である。
$N=0$ では空列を返す。

$g_0=1$ として $g'=f'g$、すなわち

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

$f^{\mathtt{exponent}}$ の先頭 $N$ 項を返す。指数は非負でなければならず、
負なら `runtime_error` を送出する。指数 0 では入力にかかわらず定数項 1 を返す。
$N>0$ では $N\le\mathtt{MOD}$ が必要である。

最初の非零項を $c x^v$ とする。$v\cdot\mathtt{exponent}\ge N$ なら全て 0 である。
そうでなければ $u=f/(cx^v)$ とおき、$h=u^{\mathtt{exponent}}$ を
$u h'=\mathtt{exponent}\,u'h$ から得る漸化式で計算する。
先頭の 0、零多項式、大きな指数による次数計算もオーバーフローしないよう処理する。

- 時間計算量: $O(R\log R+N(K+1)+\log\mathtt{MOD}+\log(\mathtt{exponent}+1))$
- 追加空間計算量: $O(N+K)$

## `sparse_fps_sqrt`

```cpp
template<int MOD>
std::optional<std::vector<Modint<MOD>>> sparse_fps_sqrt(
    const std::vector<SparseFormalPowerSeriesTerm<MOD>>& terms,
    std::size_t size
)
```

$g^2\equiv f\pmod{x^N}$ となる $g$ を返す。存在しなければ `nullopt` を返す。
零級数の平方根には零級数を返す。最初の非零項の次数が奇数、またはその係数が
平方剰余でない場合は平方根が存在しない。先頭係数の平方根は
Tonelli--Shanks 法で求め、小さい方の剰余を選ぶ。

非零入力では `MOD` が奇素数、かつ $N\le\mathtt{MOD}$ でなければならない。
標数 2 の非零入力や $N>\mathtt{MOD}$ では `runtime_error` を送出する。
$N=0$ では空列を返す。

$f=x^{2v}u$、$g=x^vq$ とし、$u q'=\frac12u'q$ の係数比較を使う。
入力に現れない高次係数を 0 とした一つの延長を返すため、返り値は常にちょうど
$N$ 項である。

- 時間計算量: $O(R\log R+N(K+1)+\log^2\mathtt{MOD})$
- 追加空間計算量: $O(N+K)$

## 注意点

各演算では次数 $N$ 以上の入力項を無視する。定数項条件は重複次数の加算と
0 係数の除去後に判定する。`sparse_fps_log`、`sparse_fps_exp`、
`sparse_fps_pow`、`sparse_fps_sqrt` で $N>\mathtt{MOD}$ を指定すると
必要な次数が標数 0 になり逆元を持たないため `runtime_error` となる。
