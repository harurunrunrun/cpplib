---
title: Rollback BitAssign RangeSum (ロールバック可能ビット代入・区間和)
documentation_of: ../src/structure/segtree/rollback_bitassign_rangesum.hpp
---

rollback可能なbit列の区間代入・区間和。詳細は `Rollback Bit Range Structures` を参照。

# 時間計算量

- constructor: $O(\mathtt{MAX\_NODES}+\mathtt{MAX\_VERSIONS}+\mathtt{MAX\_CHANGES})$
- `assign`, `flip`, `set`, `sum`, `get`: $O(\log \mathtt{MAX\_SIZE})$
- `fork`, `snapshot`, `size`, `versions`, `latest_version`, `nodes_used`, `changes_used`: $O(1)$
- `rollback`: 巻き戻す変更記録数を $K$ として $O(K)$

# API

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
