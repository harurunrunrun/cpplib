---
title: Partially Persistent Dynamic BitAssign RangeSum RangeFlip (部分永続動的ビット代入・区間和・区間反転)
documentation_of: ../src/structure/segtree/partially_persistent_dynamic_bitassign_rangesum_rangeflip.hpp
---

巨大bit列に対する部分永続な区間代入・区間反転・区間和。

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- updateと最新versionのquery: $O(\log \mathtt{MAX\_SIZE})$
- 過去versionのquery: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- version・容量情報: $O(1)$

# API

公開APIのsignature・最新版更新・過去version query・半開区間・容量・例外契約は
`Partially Persistent Bit Range Structures` と同じ。各操作の計算量は下表の通り。
