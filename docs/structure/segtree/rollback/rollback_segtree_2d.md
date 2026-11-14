---
title: Rollback 2D Segment Tree (ロールバック可能二次元セグメント木)
documentation_of: ../../../../src/structure/segtree/rollback/rollback_segtree_2d.hpp
---

点更新を `undo` でき、保存したスナップショットまで巻き戻せる二次元セグメント木。

```cpp
RollbackSegtree2D<Monoid, MAX_HEIGHT, MAX_WIDTH, MAX_UPDATE> seg(values);
```

`MAX_HEIGHT`, `MAX_WIDTH` は高さ・幅の上限、`MAX_UPDATE` は同時に保持できる（まだ取り消していない）更新数の上限である。オブジェクトのコピー・ムーブはできない。

容量（3 つのテンプレート引数）をすべて省略すると可変長版になり、`RollbackSegtree2D<Monoid> tree(height, width);` のように大きさを実行時に与える。履歴は使用量に応じて `std::vector` を拡張する。

## 構築と形

```cpp
RollbackSegtree2D()
RollbackSegtree2D(int height, int width)
explicit RollbackSegtree2D(const std::vector<std::vector<S>>& values)

seg.height();
seg.width();
seg.empty();
```

デフォルトコンストラクタは $0 \times 0$ の空の木を作る。高さ・幅を指定した場合は全要素を `Monoid.e()` にする。`values` は長方形でなければならない。いずれも高さ・幅はテンプレート引数の上限以下である必要がある。

## 更新とクエリ

```cpp
void set(int row, int col, const S& value);
void apply(int row, int col, const S& value);
S get(int row, int col) const;
S prod(int top, int left, int bottom, int right) const;
S all_prod() const;
```

`set` は指定した点の値を置き換え、`apply` は現在の値と `value` の `Monoid.op` を格納する。`prod` は半開矩形 $[top, bottom) \times [left, right)$ の総積を返す。

## ロールバック

```cpp
using Snapshot = int;
Snapshot snapshot() const;
int history_size() const;
bool can_undo() const;
void undo();
void rollback(Snapshot snapshot);
```

`snapshot()` は現在の（まだ取り消していない）更新数を返す。`undo()` は直前の更新を 1 つ、`rollback(s)` は更新数が `s` になるまで更新を取り消す。ロールバック後に別の更新を追加できるが、取り消した先の状態は保持しない。

## 時間計算量

$R$, $C$ を高さ・幅の上限以上の最小の 2 冪とする。1 回の更新で $(\log_2 R+1)(\log_2 C+1)$ 個のノード変更履歴を使う。$h$ を `history_size()` とする。

| API | 時間計算量 |
| --- | --- |
| 各コンストラクタ | $O(RC+\texttt{MAX_UPDATE}\log R\log C)$ |
| `height`, `width`, `empty`, `history_size`, `can_undo`, `snapshot` | $O(1)$ |
| `set`, `apply` | $O(\log R\log C)$ |
| `get` | $O(1)$ |
| `prod` | $O(\log R\log C)$ |
| `all_prod` | $O(1)$ |
| `undo` | $O(\log R\log C)$ |
| `rollback(s)` | $O((h-s)\log R\log C)$ |

保存領域は $O(RC+\texttt{MAX_UPDATE}\log R\log C)$ である。

## 注意点

- `Monoid.op` は結合的かつ可換で、`Monoid.e()` を単位元としなければならない。
- 不正な形、長方形でない `values`、不正な点・半開矩形、更新数・履歴の容量超過、履歴が空の状態での `undo`、負または現在より新しいスナップショットへの `rollback` では `std::runtime_error` を送出する。可変長版で高さか幅が $2^{28}$ を超える場合は `std::length_error` になる。
- 失敗した更新では状態と履歴数は変わらない。
