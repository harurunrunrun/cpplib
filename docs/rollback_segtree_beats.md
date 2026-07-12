---
title: Rollback Segment Tree Beats
documentation_of: ../src/structure/segtree/rollback_segtree_beats.hpp
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

`range_chmin` と `range_chmax` はamortized $O(\log^2 N)$、その他の更新とqueryは $O(\log N)$、`all_*` は $O(1)$。空間計算量は $O(N+\mathtt{MAX\_CHANGES}+\mathtt{MAX\_OPERATIONS})$。
