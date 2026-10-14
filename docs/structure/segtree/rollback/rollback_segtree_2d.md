---
title: Rollback 2D Segment Tree (ロールバック可能二次元セグメント木)
documentation_of: ../../../../src/structure/segtree/rollback/rollback_segtree_2d.hpp
---

点更新をundoし、保存したsnapshotまで巻き戻せる固定容量二次元segment tree。

```cpp
RollbackSegtree2D<Monoid, MAX_HEIGHT, MAX_WIDTH, MAX_UPDATE> seg(values);
```

`MAX_UPDATE` は同時に保持できる未rollback更新数。objectのcopy/moveはできない。

## 構築と形

```cpp
RollbackSegtree2D()
RollbackSegtree2D(height, width)
RollbackSegtree2D(values)

seg.height();
seg.width();
seg.empty();
```

整数指定とデフォルト構築では全要素を `Monoid.e()` にする。`values` は長方形で、実際の高さ・幅はtemplate上限以下でなければならない。

## 更新とquery

```cpp
void set(int row, int col, const S& value);
void apply(int row, int col, const S& value);

S get(int row, int col) const;
S prod(int top, int left, int bottom, int right) const;
S all_prod() const;
```

`set` は指定点を置換し、`apply` は現在値と `value` の `Monoid.op` を保存する。`prod` は半開矩形 `[top,bottom) x [left,right)` の積。

## rollback

```cpp
using Snapshot = int;

Snapshot snapshot() const;
int history_size() const;
bool can_undo() const;
void undo();
void rollback(Snapshot snapshot);
```

`snapshot()` は現在の未rollback更新数を返す。`undo()` は直前の更新1個、`rollback(s)` は更新数が `s` になるまで取り消す。rollback後に別の更新を追加できる。未来側の状態は保持しない。

## 計算量と保存領域

`R`, `C` をtemplate上限以上の最小の2冪とする。1更新で `(log2(R)+1)(log2(C)+1)` 個のnode変更履歴を使う。

| API | 時間計算量 |
| --- | --- |
| 各constructor | $O(RC+MAX\_UPDATE\log R\log C)$ |
| `height`, `width`, `empty`, `history_size`, `can_undo`, `snapshot` | $O(1)$ |
| `set`, `apply` | $O(\log R\log C)$ |
| `get` | $O(1)$ |
| `prod` | $O(\log R\log C)$ |
| `all_prod` | $O(1)$ |
| `undo` | $O(\log R\log C)$ |
| `rollback(s)` | $O((history\_size-s)\log R\log C)$ |

保存領域は $O(RC+MAX\_UPDATE\log R\log C)$。

## 注意点

`Monoid.op` は結合的かつ可換で、`Monoid.e()` を単位元としなければならない。

不正な形、非長方形vector、点、半開矩形、更新/history容量超過、空履歴の `undo`、負または現在より新しいsnapshotへのrollbackでは `runtime_error`。失敗した更新で状態と履歴数は変わらない。

## 容量を省略する場合

容量引数の既定値は `-1`。`RollbackSegtree2D<Monoid> tree(height, width);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
