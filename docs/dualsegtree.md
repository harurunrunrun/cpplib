---
title: Dual Segment Tree
documentation_of: ../src/structure/segtree/dualsegtree.hpp
---

作用付きモノイドに対する区間作用・一点取得。

# 計算量

- `DualSegtree(int)`, `DualSegtree(vector)`: $O(\mathtt{MAX\_SIZE})$
- `set`, `get`, `apply`: $O(\log \mathtt{MAX\_SIZE})$
