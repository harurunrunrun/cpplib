---
title: Fast Modint
documentation_of: ../src/structure/modint/fast_modint.hpp
---

固定 mod の整数型。

乗算に Barrett reduction を使う。

# テンプレート引数

```cpp
FastModint<mod>
```

# 関数

```cpp
FastModint x
FastModint::raw(v)
x.val()
x.pow(n)
x.inv()
```

# 演算

```cpp
+, --, +, -, *, /, +=, -=, *=, /=, ==, !=
```

# エイリアス

```cpp
FastModint998244353
FastModint1000000007
```

## 時間計算量

- 四則演算: $O(1)$
- `pow`: $O(\log n)$
- `inv`: $O(\log mod)$
