---
title: Polynomial Composition (多項式合成)
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_composition.hpp
---

係数体を `Modint<MOD>` とする多項式について、打ち切り合成
$f(g(x)) \bmod x^N$ を求める。

## API

- `polynomial_compose<MOD>(outer, inner, size)`: `outer(inner(x))` の
  $0$ 次から `size - 1` 次までの係数を返す。返却vectorの長さは常に
  `size` である。

## API別の時間計算量・空間計算量

$n=|outer|$、次数 `size` までの多項式乗算時間を $M(size)$
とする。`polynomial_compose` は平方根個ずつ係数をまとめる
Brent--Kung型の分割により
$O(\lceil\sqrt n\rceil M(size)+n\,size)$ 時間、
$O(\lceil\sqrt n\rceil size)$ 追加領域。

## 注意点

`MOD` は既存の畳み込みが利用できる法でなければならない。必要な変換長が
`MOD - 1` を割り切らない場合は既存のNTT APIと同じ例外を送出する。
`inner[0] != 0` の場合は `outer` の `size` 次以上の係数も低次項へ
寄与するため、`outer` の全係数を処理する。
