---
title: Rollback Segment Tree Beats (ロールバック可能セグメント木Beats)
documentation_of: ../../../../src/structure/segtree/rollback/rollback_segtree_beats.hpp
---

変更前nodeを履歴へ記録するrollback可能Segment Tree Beats。

```cpp
RollbackSegmentTreeBeats<T, MAX_SIZE, MAX_CHANGES, MAX_OPERATIONS>
```

`MAX_CHANGES` はnode変更数の合計上限。`MAX_OPERATIONS` はrollbackされていない更新回数の上限で、省略時は `MAX_CHANGES`。

```cpp
void range_chmin(int l, int r, const T& x)
void range_chmax(int l, int r, const T& x)
void range_add(int l, int r, const T& x)
void set(int k, const T& x)
```

容量不足時は変更履歴と配列を更新前へ戻す。

```cpp
using Snapshot = int;
Snapshot snapshot() const
void rollback(Snapshot snapshot)
```

`snapshot` 取得時点まで戻す。rollbackの時間計算量は取り消すnode変更数に比例する。

```cpp
T get(int k) const
T range_sum(int l, int r) const
T range_min(int l, int r) const
T range_max(int l, int r) const
T all_sum() const
T all_min() const
T all_max() const
```

queryは履歴を追加せず、祖先nodeの加算・上限制約・下限制約を子へ順に合成する。

```cpp
int size() const
int operations() const
int changes_used() const
```

## 計算量

- constructor: $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_CHANGES}+\mathtt{MAX\_OPERATIONS})$
- `range_chmin`, `range_chmax`: amortized $O(\log^2 N)$
- `range_add`, `set`, `get`, `range_sum`, `range_min`, `range_max`: $O(\log N)$
- `all_sum`, `all_min`, `all_max`, `snapshot`, 状態・容量情報: $O(1)$
- `rollback`: 取り消すnode変更数を $K$ として $O(K)$

空間計算量は $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_CHANGES}+\mathtt{MAX\_OPERATIONS})$。

## 注意点

点は列内、区間は半開区間 `[l,r)`。`snapshot()` は現在状態を表すtokenを返し、
`rollback(snapshot)` はそれ以後の更新を破棄する。`undo()` を持つ型では直前の更新を1つ戻し、
`can_undo()` はその可否を返す。境界探索のpredicateは単調かつ単位元に対してtrueが必要。

不正な点・区間・snapshot・predicate、空状態での `undo`、node/change/history容量超過では
`runtime_error`。失敗した更新は本体と履歴を更新前へ戻す。各APIの計算量は上記表の通り。

## 容量を省略する場合

容量引数の既定値は `-1`。`RollbackSegmentTreeBeats<T> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
