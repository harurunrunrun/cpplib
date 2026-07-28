---
title: Modular K-th Root (法K乗根)
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/modular_kth_root.hpp
---

素数 $p$ に対して $x^k\equiv a\pmod p$ を満たすすべての剰余 $x$ を列挙する。

## API

```cpp
vector<math::u64> math::modular_kth_roots_prime(
    math::u64 value,
    math::u64 exponent,
    math::u64 prime
);
```

解を昇順で返す。解がなければ空列である。

## 時間計算量

$d=\gcd(k,p-1)$ として、Baby-step Giant-step と列挙により期待 $O(\sqrt p+d\log p)$ 時間、$O(\sqrt p+d)$ 空間。

## 注意点

`prime` は素数、`exponent` は正でなければならず、違反時は `runtime_error` を送出する。`value` は内部で `prime` により剰余を取る。
