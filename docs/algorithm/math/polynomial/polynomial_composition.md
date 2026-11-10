---
title: Polynomial Composition (多項式合成) [composition_of_formal_power_series]
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_composition.hpp
---

係数を `Modint<MOD>` とする多項式について、打ち切った合成 $f(g(x)) \bmod x^N$ を求める。

## API

- `math::polynomial_compose<MOD>(outer, inner, size)`: `outer(inner(x))` の $0$ 次から $\mathrm{size} - 1$ 次までの係数を返す。戻り値の長さは常に `size` である。

## 時間計算量・空間計算量

$n$ を `outer` の長さ（$n = \lvert \mathrm{outer} \rvert$）、次数 `size` までの多項式の乗算の時間を $M(\mathrm{size})$ とする。`polynomial_compose` は係数を約 $\sqrt n$ 個ずつまとめる Brent–Kung 型の分割（baby-step giant-step）により、時間 $O(\lceil \sqrt n \rceil M(\mathrm{size}) + n \cdot \mathrm{size})$、追加領域 $O(\lceil \sqrt n \rceil \cdot \mathrm{size})$ で計算する。

## 注意点

- `MOD` は既存の畳み込みが利用できる法でなければならない。必要な変換長が $\mathrm{MOD} - 1$ を割り切らない場合は、既存の NTT API と同じ例外を送出する。
- `inner[0] != 0` の場合は `outer` の `size` 次以上の係数も低次の項に寄与するので、`outer` の係数をすべて処理する。
- より高速な $O(N \log^2 N)$ の版は `polynomial_composition_fast.hpp` にある。
