---
title: Persistent BitAssign RangeSum (完全永続ビット代入・区間和)
documentation_of: ../../../../src/structure/segtree/persistent/persistent_bitassign_rangesum.hpp
---

完全永続なbit列の区間代入・区間和。APIと計算量は `Persistent Bit Range Structures` を参照。

## 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS})$
- `assign`, `flip`, `set_one`, `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`とversion・容量情報: $O(1)$

## API

公開APIのsignature・version・半開区間・容量・例外契約は `Persistent Bit Range Structures` と同じ。
この型で利用できる各操作の計算量は下表の通り。

## 公開API

- `PersistentBitAssignRangeSum(n = MAX_SIZE)`：長さ `n` の0列をversion 0として作る。
- `size()`：列長を返す。
- `versions()` / `latest_version()`：version数と最新version番号を返す。
- `nodes_used()`：使用中の疎segment tree node数を返す。
- `fork(version)`：指定versionと同内容の新versionを返す。
- `assign(version,l,r,value)`：指定versionの `[l,r)` を0または1へ代入した新versionを返す。
- `flip(version,l,r)`：指定versionの `[l,r)` を反転した新versionを返す。
- `set_one(version,l,r)`：指定versionの `[l,r)` を1へ代入した新versionを返す。
- `sum(version,l,r)`：指定versionの `[l,r)` にある1の個数を返す。
- `get(version,k)`：指定versionの `k` 番目を返す。

`assign`, `flip`, `set_one`, `sum`, `get` は $O(\log \mathtt{MAX\_SIZE})$、
`fork` と状態情報は $O(1)$。更新1回は $O(\log \mathtt{MAX\_SIZE})$ nodeを追加し、
queryの追加空間計算量は再帰stack $O(\log \mathtt{MAX\_SIZE})$。

## 注意点

任意の有効versionから分岐でき、既存versionは変更されない。`0 < n <= MAX_SIZE`、
`0 <= l <= r <= n`、`0 <= k < n`。version・範囲・node/version容量違反では
`runtime_error`。失敗した更新はversion数とnode使用量を元へ戻す。

## 容量を省略する場合

容量引数の既定値は `-1`。`PersistentBitAssignRangeSum<> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
