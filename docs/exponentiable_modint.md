---
title: ExponentiableModint
documentation_of: ../src/structure/modint/exponentiable_modint.hpp
---

指数にも使える modint。

`mod P`, `mod phi(P)`, `mod phi(phi(P))`, ... の値を持つ。`P` 以上の値は `mod P` に落としたあと `P` を足して持つため、指数が小さいか周期に入っているかを区別できる。

## 使い方

```cpp
using mint = ExponentiableModint<998244353>;

mint n = 4;
mint k = 3;
mint m = 2;

auto ans = m.pow(k.pow(n)).val(); // 2^(3^4) mod 998244353
```

## メンバ関数

- `ExponentiableModint(uint64_t x)`
    - `x` から構築する。
- `ExponentiableModint()`
    - `0` として構築する。
- `static constexpr uint32_t get_mod()`
    - mod を返す。
- `uint32_t val() const`
    - `mod P` での値を返す。
- `ExponentiableModint pow(const ExponentiableModint& rhs) const`
    - `this^rhs` を返す。

## 演算

- `+`, `+=`
- `*`, `*=`
- `==`, `!=`

引き算はない。値が 0 か、正か、十分大きいかの情報を保つため。

## 計算量

`P` に `phi` を繰り返し適用して `1` になるまでの回数を `L` とする。

- 構築: $O(L)$
- `+`: $O(L)$
- `*`: $O(L)$
- `pow`: $O(L^2)$

`L = O(\log P)`。
