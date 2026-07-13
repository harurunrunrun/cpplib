---
title: Partially Persistent Dynamic BitOverwrite RangeSum (部分永続動的ビット上書き・区間和)
documentation_of: ../src/structure/segtree/partially_persistent_dynamic_bitoverwrite_rangesum.hpp
---

巨大bit列に対する部分永続な区間1上書き・区間和。

# 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- updateと最新versionのquery: $O(\log \mathtt{MAX\_SIZE})$
- 過去versionのquery: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- version・容量情報: $O(1)$

# API

公開APIのsignature・最新版更新・過去version query・半開区間・容量・例外契約は
`Partially Persistent Bit Range Structures` と同じ。各操作の計算量は下表の通り。

## 公開API

このheaderは `PartiallyPersistentDynamicBitOverwriteRangeSum` を非`Dynamic`名の型の別名として公開する。

- constructor：長さ `n` の0列をversion 0として作る。
- `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`：状態情報を返す。
- `fork`：最新版と同内容の新versionを返す。
- `set`, `set_one`, `assign`, `flip`：最新版を更新した新versionを返す。version省略形もある。
- `sum(version,l,r)` / `get(version,k)`：任意versionを参照する。

更新と最新版queryは $O(\log \mathtt{MAX\_SIZE})$、過去version queryは
$O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$。更新1回は同対数個のnode・履歴、
queryは $O(\log \mathtt{MAX\_SIZE})$ stackを使う。

## 注意点

include互換用header。public継承により `assign` と `flip` も利用でき、更新元は最新版だけ。
半開区間・点・version・最新版条件・各固定容量の違反では `runtime_error`。
