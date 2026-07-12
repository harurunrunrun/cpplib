---
title: Wide Segtree
documentation_of: ../src/structure/segtree/widesegtree.hpp
---

広い分岐数を持つ動的セグメント木。

値は `long long`、演算は和。

# テンプレート引数

```cpp
Wide_Segtree<MAX_SIZE>
```

`[0, MAX_SIZE)` を扱う。

# point update

```cpp
void set(idx, x)
void add(idx, x)
long long get(idx)
```

# range product

```cpp
long long prod(l, r)
long long all_prod()
```

`prod(l, r)` は半開区間 `[l,r)` の和を返す。

## 時間計算量

- $O(\log MAX\_SIZE)$
