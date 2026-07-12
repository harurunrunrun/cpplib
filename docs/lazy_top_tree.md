---
title: Lazy Top Tree
documentation_of: ../src/structure/tree/lazy_top_tree.hpp
---

遅延伝播付き自己調整 Top Tree。動的森を `Edge`、`Compress`、`Rake` cluster の階層で管理する。パスへの作用は exposed path cluster に保持し、`Compress` の左右の path cluster と共有頂点へ伝播する。`Rake` では境界パス側の子だけへ伝播する。

各頂点には単位元を持つ補助葉を1つ接続する。これにより孤立頂点を含め、各連結成分を1つの Top Tree として扱う。

# テンプレート引数

```cpp
LazyTopTree<MonoidActLen, MAX_SIZE>
```

`MonoidActLen` は次を持つ。

```cpp
S op(S a, long long len_a, S b, long long len_b)
S e()
S mapping(T f, S x, long long len)
T composition(T f, T g)
T id()
```

非可換モノイドに対応する。`MAX_SIZE` は頂点数の上限。

# コンストラクタ

```cpp
LazyTopTree(int n = MAX_SIZE)
LazyTopTree(const vector<S>& values)
```

頂点値を単位元、または `values` で初期化する。

## 時間計算量

- $O(N)$

# link / cut

```cpp
bool link(int u, int v)
bool cut(int u, int v)
```

`link` は異なる木の頂点 `u`, `v` を結ぶ。すでに連結なら `false` を返す。

`cut` は辺 $(u,v)$ を削除する。辺がなければ `false` を返す。

## 時間計算量

- amortized $O(\log N)$

# 根と連結判定

```cpp
void evert(int v)
int root(int v)
bool connected(int u, int v)
```

`evert` は `v` を表現木の根にする。`root` は同じ表現木の根を返す。

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

`u` から `v` のパス上の各頂点へ `f` を作用させる。`u`, `v` は連結でなければならない。

## 時間計算量

- amortized $O(\log N)$

# パス積

```cpp
S path_prod(int u, int v)
```

`u` から `v` の順に頂点値を畳み込む。`u`, `v` は連結でなければならない。

## 時間計算量

- amortized $O(\log N)$

# cluster 情報

```cpp
ClusterStatistics cluster_statistics(int v)
```

`v` を含む Top Tree にある `edge`、`compress`、`rake` cluster の個数と最大深さ `depth` を返す。`total()` は3種類の合計を返す。

## 時間計算量

- $O(N)$

# 空間計算量

- $O(\mathtt{MAX\_SIZE})$
