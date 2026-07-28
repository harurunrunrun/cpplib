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

$d=\gcd(k,p-1)$ として期待 $O(\sqrt p+d\log p)$ 時間、$O(\sqrt p+d)$ 空間。

## 注意点

引数条件と例外は `modular_kth_roots_prime` と同じである。
