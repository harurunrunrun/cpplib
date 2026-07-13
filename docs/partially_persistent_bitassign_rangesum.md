---
title: Partially Persistent BitAssign RangeSum (部分永続ビット代入・区間和)
documentation_of: ../src/structure/segtree/partially_persistent_bitassign_rangesum.hpp
---

部分永続なbit列の区間代入・区間和。詳細は `Partially Persistent Bit Range Structures` を参照。

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- updateと最新versionのquery: $O(\log \mathtt{MAX\_SIZE})$
- 過去versionのquery: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- version・容量情報: $O(1)$

# API

公開APIのsignature・最新版更新・過去version query・半開区間・容量・例外契約は
`Partially Persistent Bit Range Structures` と同じ。各操作の計算量は下表の通り。
