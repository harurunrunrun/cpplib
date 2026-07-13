---
title: Persistent Dynamic BitOverwrite RangeSum
documentation_of: ../src/structure/segtree/persistent_dynamic_bitoverwrite_rangesum.hpp
---

巨大なbit列に対する完全永続な区間1上書き・区間和。暗黙segment treeを使う。

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `assign`, `flip`, `set_one`, `set`, `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`とversion・容量情報: $O(1)$

# API

公開APIのsignature・version・半開区間・容量・例外契約は `Persistent Bit Range Structures` と同じ。
この型で利用できる各操作の計算量は下表の通り。
