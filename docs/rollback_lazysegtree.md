---
title: Rollback Lazy Segment Tree
documentation_of: ../src/structure/segtree/rollback_lazysegtree.hpp
---

1点更新と区間作用を取り消せるlazy segment tree。

# テンプレート引数

```cpp
RollbackLazySegtree<Monoid_act, MAX_SIZE, MAX_UPDATE>
```

- 作用付きモノイド `Monoid_act`
- 配列長の上限 `MAX_SIZE`
- 同時に保持する更新履歴数の上限 `MAX_UPDATE`

オブジェクト自体のcopy/moveはできない。

# コンストラクタ

```cpp
(1) RollbackLazySegtree(int n = MAX_SIZE)
(2) RollbackLazySegtree(const vector<S>& v)
(3) RollbackLazySegtree(const array<S, N>& v)
```

1. 長さ `n`、初期値 `Monoid_act.e()` で初期化する。
2. `v` で初期化する。
3. `v` で初期化する。

## 時間計算量

- $O(\mathrm{MAX\_SIZE}+\mathrm{MAX\_UPDATE}\log \mathrm{MAX\_SIZE})$

# 更新

```cpp
(1) void set(int k, const S& x)
(2) void apply(int k, const T& f)
(3) void apply(int l, int r, const T& f)
```

1. `k` 番目を `x` に変更する。
2. `k` 番目に `f` を作用させる。
3. 区間 $[l, r)$ に `f` を作用させる。

変更前の状態を履歴に追加する。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# 区間取得

```cpp
(1) S get(int k) const
(2) S prod(int l, int r) const
(3) S all_prod() const
```

1. `k` 番目を返す。
2. 区間 $[l, r)$ の積を返す。
3. 全要素の積を返す。

## 時間計算量

- (1), (2): $O(\log \mathrm{MAX\_SIZE})$
- (3): $O(1)$

# 二分探索

```cpp
(1) int max_right(int l, auto f, auto&&... args) const
(2) int min_left(int r, auto f, auto&&... args) const
```

1. `f(prod(l, r), args...)` がtrueとなる最大の `r` を返す。
2. `f(prod(l, r), args...)` がtrueとなる最小の `l` を返す。

`f(Monoid_act.e(), args...)` はtrueでなければならない。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# snapshot

```cpp
using Snapshot = int;

Snapshot snapshot() const
```

現在の更新履歴数を返す。

`rollback` で取り消した履歴上のsnapshotは無効になる。

## 時間計算量

- $O(1)$

# rollback

```cpp
void rollback(Snapshot s)
```

`snapshot()` が `s` を返した時点まで更新を戻す。

## 時間計算量

- 戻す更新数を $k$ として $O(k \log \mathrm{MAX\_SIZE})$

# undo

```cpp
void undo()
```

直前の更新を取り消す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# 履歴情報

```cpp
(1) bool can_undo() const
(2) int history_size() const
(3) int size() const
```

1. 取り消せる更新があるかを返す。
2. 現在保持している更新履歴数を返す。
3. 配列長を返す。

## 時間計算量

- $O(1)$

# 共通API契約

点は列内、区間は半開区間 `[l,r)`。`snapshot()` は現在状態を表すtokenを返し、
`rollback(snapshot)` はそれ以後の更新を破棄する。`undo()` を持つ型では直前の更新を1つ戻し、
`can_undo()` はその可否を返す。境界探索のpredicateは単調かつ単位元に対してtrueが必要。

不正な点・区間・snapshot・predicate、空状態での `undo`、node/change/history容量超過では
`runtime_error`。失敗した更新は本体と履歴を更新前へ戻す。各APIの計算量は上記表の通り。
