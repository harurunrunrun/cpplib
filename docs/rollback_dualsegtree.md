---
title: Rollback Dual Segment Tree
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

- $O(\mathrm{MAX\_SIZE})$

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
