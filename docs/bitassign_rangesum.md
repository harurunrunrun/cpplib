---
title: BitAssign RangeSum
documentation_of: ../src/structure/segtree/bitassign_rangesum.hpp
---

$0,1$ のみの列に対して、区間更新 と 区間和取得を SIMDとbitset高速化で定数倍を削ったデータ構造。

# 計算量

- `Bit01WST(n)`, `Bit01WST(vector)`, `init`, `build`: $O(N)$
- `assign`, `sum`, `get`: $O(\log N)$
- `size`: $O(1)$
