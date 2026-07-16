---
title: Top Tree (トップ木)
documentation_of: ../src/structure/tree/top_tree.hpp
---

自己調整 Top Tree。動的森を `Edge`、`Compress`、`Rake` cluster の階層で管理する。`Compress` tree と `Rake` tree はそれぞれ splay により調整され、2 頂点の expose でパス cluster を取り出す。

各頂点には単位元を持つ補助葉を1つ接続する。これにより孤立頂点を含め、各連結成分を1つの Top Tree として扱う。

## テンプレート引数

```cpp
TopTree<Monoid, MAX_SIZE>
```

`Monoid` は次を持つ。

```cpp
S op(S a, S b)
S e()
```

非可換モノイドに対応する。`MAX_SIZE` は頂点数の上限。

## コンストラクタ

```cpp
TopTree(int n = MAX_SIZE)
TopTree(const vector<S>& values)
```

頂点値を単位元、または `values` で初期化する。

## 時間計算量

- $O(\mathtt{MAX\_SIZE})$

固定容量のcluster領域を構築し、$N$ 個の頂点clusterを初期化する。

## link / cut

```cpp
bool link(int u, int v)
bool cut(int u, int v)
```

`link` は異なる木の頂点 `u`, `v` を結ぶ。すでに連結なら `false` を返す。

`cut` は辺 $(u,v)$ を削除する。辺がなければ `false` を返す。

## 時間計算量

- amortized $O(\log N)$

## 根と連結判定

```cpp
void evert(int v)
int root(int v)
bool connected(int u, int v)
```

`evert` は `v` を表現木の根にする。`root` は同じ表現木の根を返す。

## 時間計算量

- amortized $O(\log N)$

## 頂点値

```cpp
void set(int v, const S& x)
S get(int v)
```

## 時間計算量

- amortized $O(\log N)$

## パス積

```cpp
S path_prod(int u, int v)
```

`u` から `v` の順に頂点値を畳み込む。`u`, `v` は連結でなければならない。

## 時間計算量

- amortized $O(\log N)$

## cluster 情報

```cpp
ClusterStatistics cluster_statistics(int v)
```

`v` を含む Top Tree にある `edge`、`compress`、`rake` cluster の個数と最大深さ `depth` を返す。`total()` は3種類の合計を返す。

## 時間計算量

- $O(N)$

## 空間計算量

- $O(\mathtt{MAX\_SIZE})$

## 注意点

`int size() const`は頂点数を$O(1)$で返す。

- `0 <= n <= MAX_SIZE`、vector長も`MAX_SIZE`以下。違反時は例外。
- 全頂点引数は`[0,size())`。範囲外なら例外。
- `path_prod`は両端点が連結であることを要求し、非連結なら例外。
- `link`は既に連結なら`false`、`cut`は直接辺がなければ`false`。
