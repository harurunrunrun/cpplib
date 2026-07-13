---
title: Rollback Dynamic BitOverwrite RangeSum
documentation_of: ../src/structure/segtree/rollback_dynamic_bitoverwrite_rangesum.hpp
---

巨大bit列に対するrollback可能な区間1上書き・区間和。

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- `assign`, `flip`, `set`, `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`, `snapshot`, `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`: $O(1)$
- `rollback`: 巻き戻す変更記録数を $K$ として $O(K)$

# API

公開APIのsignature・snapshot・rollback・半開区間・容量・例外契約は
`Rollback Bit Range Structures` と同じ。各操作の計算量は下表の通り。
