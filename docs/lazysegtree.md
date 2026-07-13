---
title: Lazy Segment Tree
documentation_of: ../src/structure/segtree/lazysegtree.hpp
---

作用付きモノイドに対する区間作用・区間積。

# 計算量

- `LazySegtree(int)`, `LazySegtree(vector)`: $O(\mathtt{MAX\_SIZE})$
- `set`, `get`, 1点`apply`, 区間`apply`, `prod`: $O(\log \mathtt{MAX\_SIZE})$
- `max_right`, `min_left`: $O(\log \mathtt{MAX\_SIZE})$
- `all_prod`: $O(1)$
