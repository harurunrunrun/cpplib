---
title: Rollback Dynamic 2D Segment Tree (ロールバック可能動的二次元セグメント木)
documentation_of: ../src/structure/segtree/rollback_dynamic_segtree_2d.hpp
---

必要な経路だけを生成する、固定容量のrollback可能な二次元segment tree。
更新前の値と疎な木のpool位置を記録し、更新を逆順に取り消す。

```cpp
RollbackDynamicSegtree2D<
    Monoid, MAX_HEIGHT, MAX_WIDTH,
    MAX_ROW_NODES, MAX_COLUMN_NODES, MAX_CHANGES
> seg;
```

座標領域は `[0, MAX_HEIGHT) x [0, MAX_WIDTH)`、未更新点の値は `Monoid.e()`。
`Monoid.op` は結合的かつ可換でなければならない。`Snapshot` は `int` の別名である。

## 使用量と履歴

```cpp
int history_size() const
int row_nodes_used() const
int column_nodes_used() const
int changes_used() const
bool can_undo() const
```

順に現在残っている更新数、row/column poolの使用node数、値変更記録数、
取り消せる更新があるかを返す。すべて $O(1)$。

## 更新

```cpp
void set(long long row, long long col, const S& value)
void apply(long long row, long long col, const S& value)
```

`set` は点を置換し、`apply` は `Monoid.op(current, value)` を格納する。
$R,C$ をrow/columnの更新経路長とすると $O(RC)=O(\log H\log W)$ 時間。
値変更をちょうど $RC$ 件記録し、まだ存在しない経路nodeだけを生成する。

## 取得

```cpp
S get(long long row, long long col) const
S prod(long long rb, long long cb, long long re, long long ce) const
S all_prod() const
```

`get` は1点を $O(\log H+\log W)$、`prod` は半開矩形
`[rb,re) x [cb,ce)` を $O(\log H\log W)$、`all_prod` は全領域を $O(1)$ で返す。

## Rollback

```cpp
Snapshot snapshot() const
void undo()
void rollback(Snapshot target)
```

`snapshot` は現在の更新数を $O(1)$ で返す。`undo` は最後の更新1回を
$O(RC)$ で取り消す。`rollback(target)` は `snapshot()==target` になるまで取り消し、
時間は取り消した各更新の変更記録数と解放した疎node数の合計に比例する。

## 容量・例外

構築時間と保存領域は
$O(MAX\_ROW\_NODES+MAX\_COLUMN\_NODES+MAX\_CHANGES)$。
点・矩形・snapshotの範囲違反、逆区間、空履歴の `undo`、node/history容量超過は
`runtime_error`。失敗した更新の値変更と新規nodeは更新前へ戻る。
rollback後に新しい更新を行えるが、取り消した先の履歴へ分岐して戻ることはできない。
コピー構築・コピー代入は禁止している。
