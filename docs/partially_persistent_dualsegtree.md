---
title: Partially Persistent Dual Segment Tree (部分永続双対セグメント木)
documentation_of: ../src/structure/segtree/partially_persistent_dualsegtree.hpp
---

過去の状態を参照でき、最新の状態のみ更新できるdual segment tree。状態は0から始まるversionで指定する。

# テンプレート引数

```cpp
PartiallyPersistentDualSegtree<Monoid_act, MAX_SIZE, MAX_UPDATE>
```

- 作用付きモノイド `Monoid_act`
- 配列長の上限 `MAX_SIZE`
- 更新回数の上限 `MAX_UPDATE`

# コンストラクタ

```cpp
(1) PartiallyPersistentDualSegtree(int n = MAX_SIZE)
(2) PartiallyPersistentDualSegtree(const vector<S>& v)
(3) PartiallyPersistentDualSegtree(const array<S, N>& v)
```

## 時間計算量

- $O(\mathrm{MAX\_SIZE}+\mathrm{MAX\_UPDATE}\log \mathrm{MAX\_SIZE})$

# 更新

```cpp
int set(int k, const S& x)
int apply(int l, int r, const T& f)
```

最新versionを更新した新しいversionを作り、その番号を返す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# get

```cpp
S get(int version, int k) const
S get(int k) const
```

1. 指定versionの値を返す。
2. 最新versionの値を返す。

## 時間計算量

- (1): $O(\log \mathrm{MAX\_SIZE}\log(\mathrm{MAX\_SIZE}\,\mathrm{MAX\_UPDATE}))$
- (2): $O(\log \mathrm{MAX\_SIZE})$

# version情報

```cpp
int size() const
int versions() const
int latest_version() const
```

## 時間計算量

- $O(1)$

# 空間計算量

- $O(\mathrm{MAX\_SIZE}+\mathrm{MAX\_UPDATE}\log \mathrm{MAX\_SIZE})$

## 注意点

version 0が初期状態で、queryは `0 <= version < versions()` の任意versionを参照できる。
更新は `latest_version()` だけを元に行い、新しいversion番号を返す。過去versionを更新元に
指定すると `runtime_error`。点は列内、区間は半開区間 `[l,r)` である。
境界探索のpredicateは単調かつ単位元に対してtrueが必要。

不正なversion・点・区間・predicate、version/node/change容量超過では `runtime_error`。
失敗した更新はversion・node・履歴使用量を更新前へ戻す。各APIの計算量は上記表の通り。
