---
title: Partially Persistent BitOverwrite RangeSum (部分永続ビット上書き・区間和)
documentation_of: ../src/structure/segtree/partially_persistent_bitoverwrite_rangesum.hpp
---

部分永続なbit列の区間1上書き・区間和。詳細は `Partially Persistent Bit Range Structures` を参照。

# 計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- updateと最新versionのquery: $O(\log \mathtt{MAX\_SIZE})$
- 過去versionのquery: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- version・容量情報: $O(1)$

# API

公開APIのsignature・最新版更新・過去version query・半開区間・容量・例外契約は
`Partially Persistent Bit Range Structures` と同じ。各操作の計算量は下表の通り。

`set(version,l,r)` と `set(l,r)` は指定versionまたは最新版の半開区間を1で上書きし、
新version番号を返す。
