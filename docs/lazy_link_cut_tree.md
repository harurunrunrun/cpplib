---
title: Lazy Link Cut Tree
documentation_of: ../src/structure/tree/lazy_link_cut_tree.hpp
---

動的森に対して、辺の追加削除、連結判定、パス上の作用、パス積を行う。

# テンプレート引数

```cpp
LazyLinkCutTree<MonoidActLen, MAX_SIZE>
```

- 作用付きモノイド `MonoidActLen`
- 頂点数の上限 `MAX_SIZE`

`MonoidActLen` は次を持つ。

```cpp
S op(S a, long long len_a, S b, long long len_b)
S e()
S mapping(T f, S x, long long len)
T composition(T f, T g)
T id()
```

非可換モノイドにも対応する。

# コンストラクタ

```cpp
LazyLinkCutTree(int n = MAX_SIZE)
LazyLinkCutTree(const vector<S>& values)
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

# パス作用

```cpp
void path_apply(int u, int v, const T& f)
```

`u` から `v` へのパス上の頂点値に `f` を作用させる。

## 時間計算量

- amortized $O(\log N)$

# パス積

```cpp
S path_prod(int u, int v)
```

`u` から `v` へのパス上の頂点値のモノイド積を返す。

## 時間計算量

- amortized $O(\log N)$
