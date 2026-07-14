---
title: Weighted DSU (重み付き素集合データ構造) [CORNET] [DSL_1_B] [unionfind_with_potential]
documentation_of: ../src/structure/dsu/weighted_dsu.hpp
---

差分制約付き素集合データ構造。

# テンプレート引数

```cpp
WeightedDSU<T, MAX_SIZE>
```

- ポテンシャル差の型 `T`
- 頂点数の上限 `MAX_SIZE`

`T` は `+`, `-`, `==`, `T()` を持つ。

# コンストラクタ

```cpp
WeightedDSU(int n = MAX_SIZE)
```

`n` 頂点の集合を作る。

## 時間計算量

- $O(\mathtt{MAX\_SIZE})$

# merge / diff

```cpp
bool merge(int u, int v, T w)
T diff(int u, int v)
optional<T> diff_if_connected(int u, int v)
```

`merge(u, v, w)` は `potential(v) - potential(u) = w` を追加する。

矛盾しない場合 `true`、矛盾する場合 `false` を返す。

`diff(u, v)` は `potential(v) - potential(u)` を返す。

`diff_if_connected(u, v)` は連結なら同じ差を、非連結なら `nullopt` を返す。

## 時間計算量

- amortized $O(\alpha(N))$

`diff_if_connected`, `weight`, `component_size` も amortized $O(\alpha(N))$、`size`, `groups` は $O(1)$。

# same / leader

```cpp
bool same(int u, int v)
int leader(int v)
T weight(int v)
int component_size(int v)
int groups()
```

`weight(v)` は `potential(v) - potential(leader(v))` を返す。

## 時間計算量

- amortized $O(\alpha(N))$

## 空間計算量（API別の追加領域）

- constructor、`size`、`groups`: $O(1)$
- `leader`、`same`、`weight`、`diff`、`diff_if_connected`、`component_size`、`merge`: 再帰する代表探索のstackを含めworst-case $O(\log N)$

本体の固定保存領域は $O(\mathtt{MAX\_SIZE})$ であり、上記はそれを除く各呼び出しの追加領域である。

## 注意点

- `0 <= n <= MAX_SIZE`、頂点は`[0,n)`。違反時は例外。
- `diff(u,v)`は同一成分であることを前提とし、実装は連結性を検査しない。
- `diff_if_connected(u,v)`は連結性も同時に検査し、非連結なら `nullopt`。
- `merge(u,v,w)`は制約と整合する場合`true`。同一成分なら既存差と一致する場合だけ`true`。
- `T`の加減算はpotential差の群演算として使う。
- 固定配列は$O(MAX\_SIZE)$で、mergeによる追加領域消費はない。
