---
title: Dynamic BitAssign RangeSum RangeFlip
documentation_of: ../src/structure/segtree/dynamic_bitassign_rangesum_rangeflip.hpp
---

巨大な $0,1$ のみの列に対して、区間更新 と 区間和取得、区間反転を SIMDとbitset高速化で定数倍を削ったデータ構造。

# 計算量

- default constructor, `DynamicBit01WST(n)`, `init`: $O(1)$
- `DynamicBit01WST(vector)`: $O(P\log N)$。$P$ は1の個数
- `assign`, `flip`, `sum`, `get`: $O(\log N)$
- `size`, `node_count`: $O(1)$
