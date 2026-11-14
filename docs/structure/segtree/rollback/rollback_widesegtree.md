---
title: Rollback Wide Segtree (ロールバック可能ワイドセグメント木)
documentation_of: ../../../../src/structure/segtree/rollback/rollback_widesegtree.hpp
---

`long long` の和を扱う、ロールバック可能な疎セグメント木。`RollbackDynamicSegtree` に和のモノイドを与えたラッパーである。

```cpp
RollbackWideSegtree<MAX_SIZE, MAX_NODES, MAX_CHANGES>
```

別名 `RollbackWide_Segtree` も定義されている。固定容量版はデフォルトコンストラクタで構築し、範囲は $[0, \texttt{MAX_SIZE})$ になる。容量（3 つのテンプレート引数）をすべて省略すると可変長版になり、`RollbackWideSegtree<> tree(n);` のように長さを実行時に与える。

## API

```cpp
void set(long long index, long long value)
void add(long long index, long long value)
long long get(long long index) const
long long prod(long long l, long long r) const
long long all_prod() const
Snapshot snapshot() const
void undo()
void rollback(Snapshot snapshot)
int history_size() const
int nodes_used() const
int changes_used() const
bool can_undo() const
```

`set`, `add` は 1 点への代入・加算、`get`, `prod`, `all_prod` は 1 点の値・半開区間 $[l, r)$ の和・全体の和を返す。`Snapshot` は `int` である。

## 時間計算量

- コンストラクタ: $O(\texttt{MAX_NODES}+\texttt{MAX_CHANGES})$
- `set`, `add`, `get`, `prod`: $O(\log \texttt{MAX_SIZE})$
- `all_prod`, `snapshot`, `history_size`, `nodes_used`, `changes_used`, `can_undo`: $O(1)$
- `undo`: 直前の更新が作ったノード数・変更数に線形で、$O(\log \texttt{MAX_SIZE})$
- `rollback`: 取り消す更新が作ったノード数・変更数の合計に線形

## 注意点

- 点は範囲内、区間は半開区間 $[l, r)$ で指定する。
- `snapshot()` は現在の状態を表す値を返し、`rollback(s)` は `s` より後の更新をすべて破棄する。`undo()` は直前の更新を 1 つ取り消し、`can_undo()` はその可否を返す。
- 不正な点・区間・スナップショット、履歴が空の状態での `undo`、ノード・変更履歴の容量超過では `std::runtime_error` を送出する。
- 失敗した更新は本体と履歴を更新前に戻す。
