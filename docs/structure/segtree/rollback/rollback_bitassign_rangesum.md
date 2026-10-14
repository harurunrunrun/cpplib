---
title: Rollback BitAssign RangeSum (ロールバック可能ビット代入・区間和)
documentation_of: ../../../../src/structure/segtree/rollback/rollback_bitassign_rangesum.hpp
---

rollback可能なbit列の区間代入・区間和。詳細は `Rollback Bit Range Structures` を参照。

## 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- `assign`, `flip`, `set`, `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`, `snapshot`, `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`: $O(1)$
- `rollback`: 巻き戻す変更記録数を $K$ として $O(K)$

## API

公開APIのsignature・snapshot・rollback・半開区間・容量・例外契約は
`Rollback Bit Range Structures` と同じ。各操作の計算量は下表の通り。

## 公開API

- constructor：長さ `n` の0列を作る。
- `size`, `versions`, `latest_version`, `snapshot`, `nodes_used`, `changes_used`：状態情報を返す。
- `fork()`：現在と同内容の新状態を作る。
- `assign(l,r,value)` / `flip(l,r)`：現在の半開区間を代入／反転する。
- `set_one(l,r)` / `set(l,r)`：現在の半開区間を1へ代入する。
- `sum(l,r)` / `get(k)`：現在状態の1の個数／1点を返す。
- `rollback(snapshot)`：指定状態より新しい更新を破棄する。

更新とqueryは $O(\log \mathtt{MAX\_SIZE})$、状態情報と `fork` は $O(1)$、
`rollback` は取り消す履歴数 $K$ に対して $O(K)$。更新1回は
$O(\log \mathtt{MAX\_SIZE})$ node・履歴、queryは同量のstackを使う。

## 注意点

rollback後に破棄されたsnapshotは無効。`0 < n <= MAX_SIZE`、半開区間と点が有効で
なければならない。範囲・snapshot・node/version/change容量違反では `runtime_error`。

## 容量を省略する場合

容量引数の既定値は `-1`。`RollbackBitAssignRangeSum<> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
