---
title: Dynamic Lazy Segment Tree
documentation_of: ../src/structure/segtree/dynamic_lazysegtree.hpp
---

巨大な添字範囲を疎に確保する遅延セグメント木。

# 計算量

- constructor: $O(1)$
- `set`, `get`, 1点`apply`, 区間`apply`, `prod`: $O(\log \mathtt{MAX\_SIZE})$
- `max_right`, `min_left`: $O(\log \mathtt{MAX\_SIZE})$
- `all_prod`: $O(1)$

更新で作られるノード数は $O(\log \mathtt{MAX\_SIZE})$。
