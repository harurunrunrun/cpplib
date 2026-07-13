---
title: Persistent Lazy Segment Tree (完全永続遅延セグメント木)
documentation_of: ../src/structure/segtree/persistent_lazysegtree.hpp
---

任意の過去の状態を残したまま、1点更新または区間作用した新しい状態を作るlazy segment tree。

状態は0から始まるversionで指定する。version 0は初期状態。

# テンプレート引数

```cpp
PersistentLazySegtree<Monoid_act, MAX_SIZE, MAX_VERSION>
```

- 作用付きモノイド `Monoid_act`
- 配列長の上限 `MAX_SIZE`
- version 0から追加できるversion数の上限 `MAX_VERSION`

オブジェクト自体のcopy/moveはできない。

# コンストラクタ

```cpp
(1) PersistentLazySegtree(int n = MAX_SIZE)
(2) PersistentLazySegtree(const vector<S>& v)
(3) PersistentLazySegtree(const array<S, N>& v)
```

1. 長さ `n`、初期値 `Monoid_act.e()` で初期化する。
2. `v` で初期化する。
3. `v` で初期化する。

## 時間計算量

- $O(\mathrm{MAX\_SIZE}+\mathrm{MAX\_VERSION}\log \mathrm{MAX\_SIZE})$

# 更新

```cpp
(1) int set(int version, int k, const S& x)
(2) int apply(int version, int k, const T& f)
(3) int apply(int version, int l, int r, const T& f)
(4) int fork(int version)
```

1. `version` の `k` 番目を `x` に変更した新しいversionを作る。
2. `version` の `k` 番目に `f` を作用させた新しいversionを作る。
3. `version` の区間 $[l, r)$ に `f` を作用させた新しいversionを作る。
4. `version` と同じ内容の新しいversionを作る。

いずれも新しいversion番号を返す。

## 時間計算量

- (1), (2), (3): $O(\log \mathrm{MAX\_SIZE})$
- (4): $O(1)$

# 区間取得

```cpp
(1) S get(int version, int k) const
(2) S prod(int version, int l, int r) const
(3) S all_prod(int version) const
```

1. `version` の `k` 番目を返す。
2. `version` の区間 $[l, r)$ の積を返す。
3. `version` の全要素の積を返す。

## 時間計算量

- (1), (2): $O(\log \mathrm{MAX\_SIZE})$
- (3): $O(1)$

# 二分探索

```cpp
(1) int max_right(int version, int l, auto f, auto&&... args) const
(2) int min_left(int version, int r, auto f, auto&&... args) const
```

1. `f(prod(version, l, r), args...)` がtrueとなる最大の `r` を返す。
2. `f(prod(version, l, r), args...)` がtrueとなる最小の `l` を返す。

`f(Monoid_act.e(), args...)` はtrueでなければならない。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

# version情報

```cpp
(1) int size() const
(2) int versions() const
(3) int latest_version() const
```

1. 配列長を返す。
2. 現在存在するversion数を返す。
3. 最後に作ったversionの番号を返す。

## 時間計算量

- $O(1)$

# 共通API契約

version 0が初期状態で、`0 <= version < versions()` のversionだけを参照できる。
更新と `fork` は指定versionを変更せず、新しいversion番号を返す。`latest_version()` は
`versions()-1`。点は実際の列内、区間は半開区間 `[l,r)` で指定する。
境界探索を持つ型ではpredicateは単調かつ単位元に対してtrueでなければならない。

不正なversion・点・区間・predicate、version/node容量超過では `runtime_error`。
容量不足で更新が失敗した場合、version数と使用node数は更新前へ戻る。
各APIの計算量は上記表の通り。
