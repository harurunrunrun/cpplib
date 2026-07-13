---
title: Rollback BitAssign RangeSum
documentation_of: ../src/structure/segtree/rollback_bitassign_rangesum.hpp
---

rollback可能なbit列の区間代入・区間和。詳細は `Rollback Bit Range Structures` を参照。

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- `assign`, `flip`, `set`, `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`, `snapshot`, `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`: $O(1)$
- `rollback`: 巻き戻す変更記録数を $K$ として $O(K)$

# API

公開APIのsignature・snapshot・rollback・半開区間・容量・例外契約は
`Rollback Bit Range Structures` と同じ。各操作の計算量は下表の通り。
