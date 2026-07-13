---
title: Persistent BitOverwrite RangeSum
documentation_of: ../src/structure/segtree/persistent_bitoverwrite_rangesum.hpp
---

完全永続なbit列の区間1上書き・区間和。APIと計算量は `Persistent Bit Range Structures` を参照。

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `assign`, `flip`, `set`, `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`とversion・容量情報: $O(1)$
