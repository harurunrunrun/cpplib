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

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- `assign`, `flip`, `set`: $O(\log \mathtt{MAX\_SIZE})$
- 最新versionの`sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- 過去versionの`sum`, `get`: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- `fork`, `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`: $O(1)$

空間計算量は $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$。
