---
title: Rollback Bit Range Structures
documentation_of: ../src/structure/segtree/rollback_bitassign_rangesum_rangeflip.hpp
---

$0,1$ 列の区間更新と区間和をrollback可能にする。変更前nodeを履歴へ保存し、指定snapshot以後の更新を破棄する。

```cpp
RollbackBitAssignRangeSum<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>
RollbackBitAssignRangeSumRangeFlip<...>
RollbackBitOverwriteRangeSum<...>

int snapshot()
void rollback(int version)
```

`RollbackDynamic...` という同じAPIの別名も定義される。

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- `assign`, `flip`, `set`, `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`, `snapshot`, `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`: $O(1)$
- `rollback`: 巻き戻す変更記録数を $K$ として $O(K)$
