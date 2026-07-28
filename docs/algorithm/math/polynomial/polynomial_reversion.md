---
title: Polynomial Reversion (形式的多項式逆関数)
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_reversion.hpp
---

$f(g(x))=x\pmod{x^N}$ を満たす形式的逆関数 $g$ をNewton反復で求める。

## API

- `polynomial_reversion<MOD>(function, size)`: `function` の合成逆関数を
  `size` 項返す。

## API別の時間計算量・空間計算量

次数 $n$ までの多項式乗算時間を $M(n)$ とする。各Newton段階で多項式合成、
微分、形式的逆数、積を行うため、現在の平方根分割合成を用いた計算量は
$O(\sqrt N M(N)+N^2)$ 時間、$O(N\sqrt N)$ 追加領域。

## 注意点

`function[0] == 0` かつ `function[1] != 0` が必要である。満たさない場合は
`std::invalid_argument`。`size == 0` では空vector、`size == 1` では `{0}` を
返す。係数体で `function[1]` が可逆でなければ、`Modint::inv()` の例外を
送出する。
