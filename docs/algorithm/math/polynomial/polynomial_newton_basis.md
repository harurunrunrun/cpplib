---
title: Polynomial Newton Basis Conversion (Newton 基底への変換) [conversion_from_monomial_basis_to_newton_basis]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_newton_basis.hpp
---

単項式基底で表された多項式と、点列 $p_0, p_1, \ldots$ に対する Newton 基底

$$
f(x) = \sum_i b_i \prod_{j < i} (x - p_j)
$$

での係数列 $b$ を相互に変換する。

## API

- `math::polynomial_to_newton_basis<MOD>(f, points)`: 上式を満たす係数列 `b`（長さ `points.size()`）を返す。末尾の零係数を除いた `f` の長さが `points.size()` 以下である必要があり、そうでなければ `std::invalid_argument` を投げる。`points` の重複を許し、空入力も扱える。
- `math::polynomial_from_newton_basis<MOD>(b, points)`: 逆に Newton 基底の係数列から単項式基底の係数列（長さ `b.size()`）を復元する。`b` と `points` の長さが異なる場合は `std::invalid_argument` を投げる。

すべての点を $c$ とすれば、$(x - c)^i$ を基底とする Taylor 展開との相互変換になる。

## 時間計算量

積木（product tree）を構築し、左半分の点の積多項式による商と余りへ再帰的に分割する。$N$ 点に対し、両方向とも $O(M(N) \log N)$ 時間、$O(N \log N)$ 領域である。ここで $M(N)$ は多項式乗算の時間である。

## 注意点

法・可逆性・添字の範囲は、上記の各 API の条件を満たす必要がある。
