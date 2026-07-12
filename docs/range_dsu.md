---
title: Range DSU
documentation_of: ../src/algorithm/range/range_dsu.hpp
---

点同士の併合に加えて、区間内のすべての点を1点に併合できる DSU。

# コンストラクタ

```cpp
RangeDSU dsu(n)
```

# 関数

```cpp
dsu.leader(x)
dsu.same(x, y)
dsu.unite(x, y)
dsu.unite_range(l, r, x)
dsu.size(x)
dsu.same_range(l, r)
```

`unite_range(l, r, x)` は半開区間 $[l, r)$ のすべての点を `x` と同じ連結成分にする。

## 時間計算量

- `unite`, `same`: ならし $O(\alpha(N))$
- `unite_range`: $O(K \log N + K\alpha(N))$

`K` は操作でまたぐ区間数。
