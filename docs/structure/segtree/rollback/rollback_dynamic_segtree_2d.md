---
title: Rollback Dynamic 2D Segment Tree (ロールバック可能動的二次元セグメント木)
documentation_of: ../../../../src/structure/segtree/rollback/rollback_dynamic_segtree_2d.hpp
---

必要な経路だけを生成する、ロールバック可能な二次元セグメント木。更新前の値と疎な木のノードプールの位置を記録し、更新を逆順に取り消す。

```cpp
RollbackDynamicSegtree2D<
    Monoid, MAX_HEIGHT, MAX_WIDTH,
    MAX_ROW_NODES, MAX_COLUMN_NODES, MAX_CHANGES
> seg;
```

座標領域は $[0, \texttt{MAX_HEIGHT}) \times [0, \texttt{MAX_WIDTH})$ で、未更新の点の値は `Monoid.e()` である。`Monoid.op` は結合的かつ可換でなければならない。`Snapshot` は `int` の別名である。

容量（5 つのテンプレート引数）をすべて省略すると可変長版になり、`RollbackDynamicSegtree2D<Monoid> tree(height, width);` のように大きさを実行時に与える。ノードと履歴は使用量に応じて `std::vector` を拡張する。

## 使用量と履歴

```cpp
int history_size() const
int row_nodes_used() const
int column_nodes_used() const
int changes_used() const
bool can_undo() const
```

順に現在残っている更新数、行方向・列方向のノードプールの使用ノード数、値の変更記録数、取り消せる更新があるかどうかを返す。すべて $O(1)$ である。

## 更新

```cpp
void set(long long row, long long col, const S& value)
void apply(long long row, long long col, const S& value)
```

`set` は点の値を置き換え、`apply` は `Monoid.op(current, value)` を格納する。$R, C$ を行方向・列方向の更新経路長とすると、値の変更をちょうど $RC$ 件記録し、まだ存在しない経路上のノードだけを生成する。

## 取得

```cpp
S get(long long row, long long col) const
S prod(long long rb, long long cb, long long re, long long ce) const
S all_prod() const
```

`get` は 1 点の値、`prod` は半開矩形 $[rb, re) \times [cb, ce)$ の総積、`all_prod` は全領域の総積を返す。

## ロールバック

```cpp
Snapshot snapshot() const
void undo()
void rollback(Snapshot target)
```

`snapshot` は現在の更新数を返す。`undo` は最後の更新を 1 回取り消す。`rollback(target)` は `snapshot() == target` になるまで更新を取り消す。

## 時間計算量

$H=\texttt{MAX_HEIGHT}$、$W=\texttt{MAX_WIDTH}$ とする。

| API | 時間計算量 | 履歴・一時領域 |
| --- | --- | --- |
| コンストラクタ | $O(\texttt{MAX_ROW_NODES}+\texttt{MAX_COLUMN_NODES}+\texttt{MAX_CHANGES})$ | 同量の固定領域 |
| `history_size`, `row_nodes_used`, `column_nodes_used`, `changes_used`, `can_undo` | $O(1)$ | $O(1)$ |
| `set`, `apply` | $O(\log H\log W)$ | 変更履歴 $O(\log H\log W)$、スタック $O(\log H+\log W)$ |
| `get` | $O(\log H+\log W)$ | スタック $O(\log H+\log W)$ |
| `prod` | $O(\log H\log W)$ | スタック $O(\log H+\log W)$ |
| `all_prod`, `snapshot` | $O(1)$ | $O(1)$ |
| `undo` | $O(\log H\log W)$ | $O(1)$ |
| `rollback(target)` | 取り消す変更記録数と解放する疎ノード数の合計に線形 | $O(1)$ |

## 注意点

- 点・矩形・snapshot の範囲外、逆向きの区間、履歴が空の状態での `undo`、ノード・履歴の容量超過では `std::runtime_error` を送出する。
- 失敗した更新の値の変更と新規ノードは更新前に戻る。
- ロールバック後に新しい更新を行えるが、取り消した先の履歴へ分岐して戻ることはできない。
- コピーコンストラクタとコピー代入は削除されている。
