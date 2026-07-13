---
title: Rollback Dynamic BitAssign RangeSum RangeFlip
documentation_of: ../src/structure/segtree/rollback_dynamic_bitassign_rangesum_rangeflip.hpp
---

巨大bit列に対するrollback可能な区間代入・区間反転・区間和。

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- `assign`, `flip`, `set`, `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`, `snapshot`, `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`: $O(1)$
- `rollback`: 巻き戻す変更記録数を $K$ として $O(K)$
