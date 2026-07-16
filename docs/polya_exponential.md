---
title: Pólya Exponential (ポリア指数)
documentation_of: ../src/algorithm/fft/polya_exponential.hpp
---

定数項が0の形式的冪級数 $f(x)$ に対するPólya指数（plethystic exponential）

$$
\operatorname{PExp}(f)(x)
=\exp\left(\sum_{k\ge 1}\frac{f(x^k)}{k}\right)
$$

を計算する。非標識組合せ構造のmultiset構成やEuler変換に利用できる。

## `polya_exponential`

```cpp
template<int MOD>
vector<Modint<MOD>> polya_exponential(
    const vector<Modint<MOD>>& values,
    size_t size
)
```

`values` を $f$ の係数列として、$x^{\mathtt{size}}$ 未満のPólya指数を返す。
不足する入力係数は0とみなし、余分な入力係数は無視する。

```cpp
template<int MOD>
vector<Modint<MOD>> polya_exponential(
    const vector<Modint<MOD>>& values
)
```

出力長を `values.size()` とする短縮形である。

## `polya_logarithm`

```cpp
template<int MOD>
vector<Modint<MOD>> polya_logarithm(
    const vector<Modint<MOD>>& values,
    size_t size
)
```

定数項が1の級数 $F$ に対し、Pólya指数の逆変換

$$
\operatorname{PLog}(F)(x)
=\sum_{k\ge1}\frac{\mu(k)}{k}\log F(x^k)
$$

を $x^{\mathtt{size}}$ 未満まで返す。

```cpp
template<int MOD>
vector<Modint<MOD>> polya_logarithm(
    const vector<Modint<MOD>>& values
)
```

出力長を `values.size()` とする短縮形である。

## 時間計算量

出力長を $N$、長さ $N$ の畳み込み時間を $M(N)$ とする。

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `polya_exponential`（両overload） | $O(M(N)\log N+N\log N)$ | $O(N)$ |
| `polya_logarithm`（両overload） | $O(M(N)\log N+N\log N)$ | $O(N)$ |

同梱のNTTを使える法では $M(N)=O(N\log N)$ である。

## 注意点

- `MOD` は素数で、`size <= MOD` でなければならない。途中で $1,2,\ldots,N-1$
  の逆元を用いる。
- `polya_exponential` は定数項0を要求し、違反時は `invalid_argument` を送出する。
- `polya_logarithm` は定数項1を要求し、違反時は `invalid_argument` を送出する。
- `size == 0` なら入力に関係なく空列を返す。
- 大きな次数では、法が必要なNTT長を許容する必要がある。
