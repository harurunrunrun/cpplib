---
title: Partially Persistent Dynamic BitOverwrite RangeSum
documentation_of: ../src/structure/segtree/partially_persistent_dynamic_bitoverwrite_rangesum.hpp
---

巨大bit列に対する部分永続な区間1上書き・区間和。

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- updateと最新versionのquery: $O(\log \mathtt{MAX\_SIZE})$
- 過去versionのquery: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- version・容量情報: $O(1)$
