---
title: Rollback Dual Segment Tree (ロールバック可能双対セグメント木)
documentation_of: ../src/structure/segtree/rollback_dualsegtree.hpp
---

1点更新と区間作用を取り消せるdual segment tree。

# テンプレート引数

```cpp
RollbackDualSegtree<Monoid_act, MAX_SIZE, MAX_UPDATE>
```

# コンストラクタ

```cpp
(1) RollbackDualSegtree(int n = MAX_SIZE)
(2) RollbackDualSegtree(const vector<S>& v)
(3) RollbackDualSegtree(const array<S, N>& v)
```

## 時間計算量

- $O(\mathrm{MAX\_SIZE}+\mathrm{MAX\_UPDATE}\log \mathrm{MAX\_SIZE})$

# 更新と取得

```cpp
(1) void set(int k, const S& x)
(2) void apply(int l, int r, const T& f)
(3) S get(int k) const
```

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# rollback

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

# 情報

```cpp
int size() const
int history_size() const
bool can_undo() const
```

## 時間計算量

- $O(1)$

# 共通API契約

点は列内、区間は半開区間 `[l,r)`。`snapshot()` は現在状態を表すtokenを返し、
`rollback(snapshot)` はそれ以後の更新を破棄する。`undo()` を持つ型では直前の更新を1つ戻し、
`can_undo()` はその可否を返す。境界探索のpredicateは単調かつ単位元に対してtrueが必要。

不正な点・区間・snapshot・predicate、空状態での `undo`、node/change/history容量超過では
`runtime_error`。失敗した更新は本体と履歴を更新前へ戻す。各APIの計算量は上記表の通り。
