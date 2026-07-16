---
title: Partially Persistent BitAssign RangeSum (部分永続ビット代入・区間和)
documentation_of: ../src/structure/segtree/partially_persistent_bitassign_rangesum.hpp
---

部分永続なbit列の区間代入・区間和。詳細は `Partially Persistent Bit Range Structures` を参照。

## 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- updateと最新versionのquery: $O(\log \mathtt{MAX\_SIZE})$
- 過去versionのquery: $O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$
- version・容量情報: $O(1)$

## API

公開APIのsignature・最新版更新・過去version query・半開区間・容量・例外契約は
`Partially Persistent Bit Range Structures` と同じ。各操作の計算量は下表の通り。

## 公開API

- constructor：長さ `n` の0列をversion 0として作る。
- `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`：列長と使用量を返す。
- `fork(version)`：最新版と同内容の新versionを返す。
- `assign(version,l,r,value)` / `assign(l,r,value)`：最新版の区間を代入する。
- `flip(version,l,r)` / `flip(l,r)`：最新版の区間を反転する。
- `set_one` / `set` のversion有無overload：最新版の区間を1へ代入する。
- `sum(version,l,r)` / `get(version,k)`：任意versionの1の個数／1点を返す。

更新と最新版queryは $O(\log \mathtt{MAX\_SIZE})$、過去version queryは
$O(\log \mathtt{MAX\_SIZE}\log \mathtt{MAX\_CHANGES})$、状態情報と `fork` は $O(1)$。
更新1回は $O(\log \mathtt{MAX\_SIZE})$ node・履歴、queryは同量の再帰stackを使う。

## 注意点

更新元は常に `latest_version()`。version引数付き更新も過去versionを受け付けない。
`0 < n <= MAX_SIZE`、半開区間・点・versionが有効でなければならない。
最新版条件、範囲、node/version/change容量違反では `runtime_error`。
