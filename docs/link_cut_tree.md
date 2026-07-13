---
title: Link Cut Tree
documentation_of: ../src/structure/tree/link_cut_tree.hpp
---

動的森に対して、辺の追加削除、連結判定、パス上のモノイド積を行う。

# テンプレート引数

```cpp
LinkCutTree<Monoid, MAX_SIZE>
```

- モノイド `Monoid`
- 頂点数の上限 `MAX_SIZE`

`Monoid` は `S op(S, S)` と `S e()` を持つ。非可換モノイドにも対応する。

# コンストラクタ

```cpp
LinkCutTree(int n = MAX_SIZE)
LinkCutTree(const vector<S>& values)
```

頂点値を単位元、または `values` で初期化する。

## 時間計算量

- $O(\mathtt{MAX\_SIZE})$

固定容量のnode配列全体を初期化する。

# link / cut

```cpp
bool link(int u, int v)
bool cut(int u, int v)
```

`link` は辺 $(u,v)$ を追加する。すでに連結なら `false` を返す。

`cut` は辺 $(u,v)$ を削除する。辺がなければ `false` を返す。

## 時間計算量

- amortized $O(\log N)$

# 連結判定

```cpp
bool connected(int u, int v)
int root(int v)
```

## 時間計算量

- amortized $O(\log N)$

# 頂点値

```cpp
void set(int v, const S& x)
S get(int v)
```

## 時間計算量

- amortized $O(\log N)$

# パス積

```cpp
S path_prod(int u, int v)
```

`u` から `v` へのパス上の頂点値のモノイド積を返す。

## 時間計算量

- amortized $O(\log N)$
