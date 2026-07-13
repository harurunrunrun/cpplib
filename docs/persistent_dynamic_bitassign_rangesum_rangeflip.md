---
title: Persistent Dynamic BitAssign RangeSum RangeFlip (完全永続動的ビット代入・区間和・区間反転)
documentation_of: ../src/structure/segtree/persistent_dynamic_bitassign_rangesum_rangeflip.hpp
---

巨大なbit列に対する完全永続な区間代入・区間反転・区間和。暗黙segment treeを使う。

# 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `assign`, `flip`, `set_one`, `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`とversion・容量情報: $O(1)$

# API

公開APIのsignature・version・半開区間・容量・例外契約は `Persistent Bit Range Structures` と同じ。
この型で利用できる各操作の計算量は下表の通り。

## 公開API

このheaderは `PersistentDynamicBitAssignRangeSumRangeFlip` を
`PersistentBitAssignRangeSumRangeFlip` の別名として公開する。

- constructor：長さ `n` の0列をversion 0として作る。
- `size`, `versions`, `latest_version`, `nodes_used`：列長と使用量を返す。
- `fork(version)`：同内容の新versionを返す。
- `assign(version,l,r,value)`：`[l,r)` を0または1へ代入した新versionを返す。
- `flip(version,l,r)`：`[l,r)` を反転した新versionを返す。
- `set_one(version,l,r)`：`[l,r)` を1へ代入した新versionを返す。
- `sum(version,l,r)` / `get(version,k)`：1の個数／1点を返す。

更新とqueryは $O(\log \mathtt{MAX\_SIZE})$、`fork` と状態情報は $O(1)$。
更新1回は $O(\log \mathtt{MAX\_SIZE})$ node、queryは同量の再帰stackを使う。

## 注意点

include互換用headerであり、実装と契約は非`Dynamic`名の型と同じ。任意versionから分岐する。
`0 < n <= MAX_SIZE`、半開区間・点・versionが有効でなければならない。
範囲・node/version容量違反では `runtime_error`。
