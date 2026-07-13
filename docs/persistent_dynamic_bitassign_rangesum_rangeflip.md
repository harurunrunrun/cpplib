---
title: Persistent Dynamic BitAssign RangeSum RangeFlip
documentation_of: ../src/structure/segtree/persistent_dynamic_bitassign_rangesum_rangeflip.hpp
---

巨大なbit列に対する完全永続な区間代入・区間反転・区間和。暗黙segment treeを使う。

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `assign`, `flip`, `set`, `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`とversion・容量情報: $O(1)$
