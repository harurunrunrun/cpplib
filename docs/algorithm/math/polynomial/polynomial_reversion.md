---
title: Polynomial Reversion (形式的多項式逆関数)
documentation_of: ../../../../src/algorithm/math/polynomial/polynomial_reversion.hpp
---

$f(g(x)) \equiv x \pmod{x^N}$ を満たす形式的逆関数 $g$ を Newton 法で求める。

## API

- `math::polynomial_reversion<MOD>(function, size)`: `function` の合成に関する逆関数の先頭 `size` 項を返す。

## API別の時間計算量・空間計算量

$N$ を `size`、次数 $n$ までの多項式乗算の時間を $M(n)$ とする。Newton 法の各段階で多項式の合成、微分、形式的冪級数の逆元、積を計算する。現在の平方分割による合成を用いるため、時間計算量は $O(\sqrt{N} M(N) + N^2)$、追加領域は $O(N \sqrt{N})$ である。

## 注意点

- `size == 0` のときは入力を検査せずに空の列を返す。
- `size >= 1` では `function[0] == 0` かつ `function[1] != 0` が必要であり、満たさない場合（`function.size() < 2` を含む）は `std::invalid_argument` を送出する。`size == 1` では `{0}` を返す。
- 係数体で `function[1]` が可逆でなければ、`Modint::inv()` の例外が送出される。
