---
title: Polynomial Newton Basis Conversion (Newton 基底への変換) [conversion_from_monomial_basis_to_newton_basis]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_newton_basis.hpp
---

`math::polynomial_to_newton_basis<MOD>(f,points)` は

$$f(x)=\sum_i b_i\prod_{j<i}(x-p_j)$$

となる係数列 `b` を返す。`f.size()<=points.size()` が必要。`points` の重複を許す。空入力も扱う。

## 時間計算量

積木を構築し、左半分の積多項式による商と余りへ再帰的に分割する。$N$ 点に対して $O(M(N)\log N)$ 時間、$O(N\log N)$ 領域。

`polynomial_from_newton_basis<MOD>(b,points)` は逆向きに単項式基底へ復元する。2列の長さを一致させる。計算量は順変換と同じ。

類題: 重複点も含む Newton 基底の係数から元の多項式を復元する。すべての点を `c` とすれば `(x-c)^i` 基底の Taylor 展開からの復元になる。

## 注意点

GCC 13 / C++20。法・可逆性・添字範囲は上記の各APIの条件を満たす必要がある。
