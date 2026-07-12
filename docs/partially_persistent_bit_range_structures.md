---
title: Partially Persistent Bit Range Structures
documentation_of: ../src/structure/segtree/partially_persistent_bitassign_rangesum_rangeflip.hpp
---

$0,1$ 列の区間更新と区間和を部分永続化する。更新は最新versionだけに行い、過去の全versionを参照できる。

完全永続版は使用せず、各疎segment tree nodeの変更履歴だけを保存する。

```cpp
PartiallyPersistentBitAssignRangeSum<MAX_SIZE, MAX_NODES, MAX_VERSIONS, MAX_CHANGES>
PartiallyPersistentBitAssignRangeSumRangeFlip<...>
PartiallyPersistentBitOverwriteRangeSum<...>
```

`PartiallyPersistentDynamic...` という同じAPIの別名も定義される。

更新は $O(\log MAX\_SIZE)$、過去versionの問い合わせは $O(\log MAX\_SIZE\log MAX\_CHANGES)$。
