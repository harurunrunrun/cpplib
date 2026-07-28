---
title: Discrete Root Modulo a Prime (素数法の離散乗根)
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/discrete_root_mod_prime.hpp
---

素数 $p$ に対して $x^k\equiv a\pmod p$ を満たす剰余を1つ求める。

## API

```cpp
optional<math::u64> math::discrete_root_mod_prime(
    math::u64 value,
    math::u64 exponent,
    math::u64 prime
);
```

解が存在すれば最小の非負解、存在しなければ `nullopt` を返す。

## 時間計算量

$d=\gcd(k,p-1)$ とする。

- 原始根に対する離散対数: 期待 $O(\sqrt p)$ 時間、$O(\sqrt p)$ 空間
- 指数の一次合同式: $O(\log p)$ 時間、$O(1)$ 空間
- $d$ 個の候補から最小解を選ぶ処理: $O(d\log p)$ 時間、$O(1)$ 空間
- `discrete_root_mod_prime` 全体: 期待 $O(\sqrt p+d\log p)$ 時間、
  $O(\sqrt p)$ 空間

## 注意点

`prime` は素数、`exponent` は正でなければならず、違反時は
`runtime_error` を送出する。`value` は関数内で `prime` を法として正規化する。
Baby-step Giant-step で原始根に対する離散対数を求めた後、指数に関する一次合同式を
解く独立実装であり、全解列挙 API には依存しない。
