---
title: Persistent Dynamic BitOverwrite RangeSum (完全永続動的ビット上書き・区間和)
documentation_of: ../src/structure/segtree/persistent_dynamic_bitoverwrite_rangesum.hpp
---

巨大なbit列に対する完全永続な区間1上書き・区間和。暗黙segment treeを使う。

## 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `assign`, `flip`, `set_one`, `set`, `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`とversion・容量情報: $O(1)$

## API

公開APIのsignature・version・半開区間・容量・例外契約は `Persistent Bit Range Structures` と同じ。
この型で利用できる各操作の計算量は下表の通り。

## 公開API

このheaderは `PersistentDynamicBitOverwriteRangeSum` を
`PersistentBitOverwriteRangeSum` の別名として公開する。

- constructor：長さ `n` の0列をversion 0として作る。
- `size`, `versions`, `latest_version`, `nodes_used`：列長と使用量を返す。
- `fork(version)`：同内容の新versionを返す。
- `set` / `set_one`：指定半開区間を1へ代入した新versionを返す。
- `assign` / `flip`：指定半開区間を代入／反転した新versionを返す。
- `sum` / `get`：指定versionの1の個数／1点を返す。

更新とqueryは $O(\log \mathtt{MAX\_SIZE})$、`fork` と状態情報は $O(1)$。
更新1回は $O(\log \mathtt{MAX\_SIZE})$ node、queryは同量の再帰stackを使う。

## 注意点

include互換用headerであり、実装と契約は非`Dynamic`名の型と同じ。public継承により
`assign` と `flip` も利用できる。半開区間・点・version・各容量の違反では
`runtime_error`。
