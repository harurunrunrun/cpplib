---
title: Modular K-th Root (法K乗根)
documentation_of: ../../../../../src/algorithm/math/number_theory/modular/modular_kth_root.hpp
---

素数 $p$ に対して $x^k \equiv a \pmod p$ を満たすすべての剰余 $x$ を列挙する。

## API

```cpp
std::vector<math::u64> math::modular_kth_roots_prime(
    math::u64 value,
    math::u64 exponent,
    math::u64 prime
);
```

`value` $= a$、`exponent` $= k$、`prime` $= p$ として、解を $[0, p)$ の昇順で返す。解がなければ空列を返す。$a \equiv 0$ なら `{0}` を返す。

## 時間計算量・空間計算量

$d = \gcd(k, p-1)$ とする。原始根の計算（$p-1$ の素因数分解）の後、baby-step giant-step による離散対数と解の列挙・ソートで、期待 $O(\sqrt p + d \log p)$ 時間、$O(\sqrt p + d)$ 空間。

## 注意点

- `prime` は素数（Miller–Rabin で判定）、`exponent` は正でなければならず、違反時は `std::runtime_error` を送出する。
- `value` は内部で `prime` を法として正規化する。
- 解を 1 つだけ求めればよい場合は `discrete_root_mod_prime` や `one_kth_root_mod_prime` を使う。
