---
title: Rollback Dynamic BitOverwrite RangeSum (ロールバック可能動的ビット上書き・区間和)
documentation_of: ../src/structure/segtree/rollback_dynamic_bitoverwrite_rangesum.hpp
---

巨大bit列に対するrollback可能な区間1上書き・区間和。

# 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- `assign`, `flip`, `set`, `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`, `snapshot`, `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`: $O(1)$
- `rollback`: 巻き戻す変更記録数を $K$ として $O(K)$

# API

公開APIのsignature・snapshot・rollback・半開区間・容量・例外契約は
`Rollback Bit Range Structures` と同じ。各操作の計算量は下表の通り。

## 公開API

このheaderは `RollbackDynamicBitOverwriteRangeSum` を非`Dynamic`名の型の別名として公開する。

- constructorと状態情報：0列を作り、列長・version・node・履歴使用量を返す。
- `fork`：現在と同内容の新状態を作る。
- `set`, `set_one`, `assign`, `flip`：現在の半開区間を更新する。
- `sum`, `get`：現在状態を参照する。
- `snapshot`, `rollback`：状態番号を取得し、指定状態へ戻す。

更新とqueryは $O(\log \mathtt{MAX\_SIZE})$、状態情報と `fork` は $O(1)$、
rollbackは取り消す履歴数に線形。更新は同対数個のnode・履歴、queryは同量のstackを使う。

## 注意点

include互換用header。public継承により `assign` と `flip` も利用できる。
rollback後の未来側状態は破棄される。半開区間・点・snapshot・各容量の違反では
`runtime_error`。
