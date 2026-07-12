---
title: Common Monoids
documentation_of: ../src/structure/types/common_monoids.hpp
---

よく使う monoid。

`Segtree` など、`using S`, `op`, `e` を持つ monoid オブジェクトを受け取る構造に渡せる。

## 種類

- `AddMonoid<T>`
    - 加算。単位元は `0`。
- `MulMonoid<T>`
    - 乗算。単位元は `1`。
- `MinMonoid<T>`
    - 最小値。単位元は `numeric_limits<T>::max()`。
- `MaxMonoid<T>`
    - 最大値。単位元は `numeric_limits<T>::lowest()`。
- `GcdMonoid<T>`
    - gcd。単位元は `0`。
- `LcmMonoid<T>`
    - lcm。単位元は `1`。
- `XorMonoid<T>`
    - xor。単位元は `0`。
- `BitAndMonoid<T>`
    - bitwise and。単位元は `~T(0)`。
- `BitOrMonoid<T>`
    - bitwise or。単位元は `0`。

## 使い方

```cpp
constexpr AddMonoid<long long> add_monoid{};
Segtree<add_monoid, 200000> seg(n);
```

`MinMonoid` と `MaxMonoid` は単位元を指定できる。

```cpp
constexpr MinMonoid<long long> min_monoid{(1LL << 60)};
```
