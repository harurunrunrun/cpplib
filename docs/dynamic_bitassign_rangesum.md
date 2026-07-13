---
title: Dynamic BitAssign RangeSum
documentation_of: ../src/structure/segtree/dynamic_bitassign_rangesum.hpp
---

巨大な $0,1$ のみの列に対して、区間更新 と 区間和取得を SIMDとbitset高速化で定数倍を削ったデータ構造。

# 計算量

- default constructor, `DynamicBit01WSTNoFlip(n)`, `init`: $O(1)$
- `DynamicBit01WSTNoFlip(vector)`: $O(P\log N)$。$P$ は1の個数
- `assign`, `sum`, `get`: $O(\log N)$
- `size`, `node_count`: $O(1)$
