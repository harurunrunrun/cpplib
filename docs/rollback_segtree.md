---
title: Rollback Segment Tree (ロールバック可能セグメント木)
documentation_of: ../src/structure/segtree/rollback_segtree.hpp
---

1点更新を取り消せるsegment tree。

# テンプレート引数

```cpp
RollbackSegtree<Monoid, MAX_SIZE, MAX_UPDATE>
```

- モノイド `Monoid`
- 配列長の上限 `MAX_SIZE`
- 同時に保持する更新履歴数の上限 `MAX_UPDATE`

オブジェクト自体のcopy/moveはできない。

# コンストラクタ

```cpp
(1) RollbackSegtree(int n = MAX_SIZE)
(2) RollbackSegtree(const vector<S>& v)
(3) RollbackSegtree(const array<S, N>& v)
```

1. 長さ `n`、初期値 `Monoid.e()` で初期化する。
2. `v` で初期化する。
3. `v` で初期化する。

## 時間計算量

- $O(\mathrm{MAX\_SIZE}+\mathrm{MAX\_UPDATE}\log \mathrm{MAX\_SIZE})$

# set

```cpp
void set(int k, const S& x)
```

`k` 番目を `x` に変更し、変更前の状態を履歴に追加する。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# get

```cpp
S get(int k) const
```

`k` 番目を返す。

## 時間計算量

- $O(1)$

# prod

```cpp
S prod(int l, int r) const
```

区間 $[l, r)$ の積を返す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# all_prod

```cpp
S all_prod() const
```

全要素の積を返す。

## 時間計算量

- $O(1)$

# max_right

```cpp
int max_right(int l, auto f, auto&&... args) const
```

`f(Monoid.e(), args...)` がtrueであるとき、`f(prod(l, r), args...)` がtrueとなる最大の `r` を返す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# min_left

```cpp
int min_left(int r, auto f, auto&&... args) const
```

`f(Monoid.e(), args...)` がtrueであるとき、`f(prod(l, r), args...)` がtrueとなる最小の `l` を返す。

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

現在より後のsnapshot、および無効になったsnapshotは指定できない。

## 時間計算量

- 戻す更新数を $k$ として $O(k \log \mathrm{MAX\_SIZE})$

# undo

```cpp
void undo()
```

直前の更新を取り消す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# can_undo

```cpp
bool can_undo() const
```

取り消せる更新があるかを返す。

## 時間計算量

- $O(1)$

# history_size

```cpp
int history_size() const
```

現在保持している更新履歴数を返す。

## 時間計算量

- $O(1)$

# size

```cpp
int size() const
```

配列長を返す。

## 時間計算量

- $O(1)$

# 共通API契約

点は列内、区間は半開区間 `[l,r)`。`snapshot()` は現在状態を表すtokenを返し、
`rollback(snapshot)` はそれ以後の更新を破棄する。`undo()` を持つ型では直前の更新を1つ戻し、
`can_undo()` はその可否を返す。境界探索のpredicateは単調かつ単位元に対してtrueが必要。

不正な点・区間・snapshot・predicate、空状態での `undo`、node/change/history容量超過では
`runtime_error`。失敗した更新は本体と履歴を更新前へ戻す。各APIの計算量は上記表の通り。
