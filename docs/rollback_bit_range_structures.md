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

更新と問い合わせは $O(\log MAX\_SIZE)$。rollbackは破棄する変更数に線形。
