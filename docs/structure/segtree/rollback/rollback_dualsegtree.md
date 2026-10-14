---
title: Rollback Dual Segment Tree (ロールバック可能双対セグメント木)
documentation_of: ../../../../src/structure/segtree/rollback/rollback_dualsegtree.hpp
---

1点更新と区間作用を取り消せるdual segment tree。

## テンプレート引数

```cpp
RollbackDualSegtree<Monoid_act, MAX_SIZE, MAX_UPDATE>
```

## コンストラクタ

```cpp
(1) RollbackDualSegtree(int n = MAX_SIZE)
(2) RollbackDualSegtree(const vector<S>& v)
(3) RollbackDualSegtree(const array<S, N>& v)
```

## 時間計算量

- $O(\mathrm{MAX\_SIZE}+\mathrm{MAX\_UPDATE}\log \mathrm{MAX\_SIZE})$

## 更新と取得

```cpp
(1) void set(int k, const S& x)
(2) void apply(int l, int r, const T& f)
(3) S get(int k) const
```

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

## rollback

```cpp
using Snapshot = int;

Snapshot snapshot() const
void rollback(Snapshot s)
void undo()
```

`rollback` は指定したsnapshotまで、`undo` は直前の更新を取り消す。

## 時間計算量

- `snapshot`: $O(1)$
- `undo`: $O(\log \mathrm{MAX\_SIZE})$
- 戻す更新数を $k$ とした `rollback`: $O(k \log \mathrm{MAX\_SIZE})$

## 情報

```cpp
int size() const
int history_size() const
bool can_undo() const
```

## 時間計算量

- $O(1)$

## 注意点

点は列内、区間は半開区間 `[l,r)`。`snapshot()` は現在状態を表すtokenを返し、
`rollback(snapshot)` はそれ以後の更新を破棄する。`undo()` を持つ型では直前の更新を1つ戻し、
`can_undo()` はその可否を返す。境界探索のpredicateは単調かつ単位元に対してtrueが必要。

不正な点・区間・snapshot・predicate、空状態での `undo`、node/change/history容量超過では
`runtime_error`。失敗した更新は本体と履歴を更新前へ戻す。各APIの計算量は上記表の通り。

## 容量を省略する場合

容量引数の既定値は `-1`。`RollbackDualSegtree<Monoid_act> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
