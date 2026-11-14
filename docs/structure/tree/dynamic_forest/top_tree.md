---
title: Top Tree (トップ木)
documentation_of: ../../../../src/structure/tree/dynamic_forest/top_tree.hpp
---

自己調整 Top Tree である。動的な森を `Edge`、`Compress`、`Rake` の 3 種類のクラスタの階層で管理する。`Compress` 木と `Rake` 木はそれぞれ splay により調整され、2 頂点を expose してパスのクラスタを取り出す。

各頂点には単位元を持つ補助葉を 1 つ接続する。これにより、孤立頂点を含めて各連結成分を 1 つの Top Tree として扱う。

## テンプレート引数

```cpp
TopTree<Monoid, MAX_SIZE>
```

`Monoid` はモノイドのオブジェクト（非型テンプレート引数）で、次を持つ。

```cpp
S op(S a, S b)
S e()
```

非可換モノイドに対応する。`MAX_SIZE` は頂点数の上限で、省略するとデフォルト値 `-1` の可変長版になる。

## コンストラクタ

```cpp
TopTree(int n = MAX_SIZE)
TopTree(const vector<S>& values)
```

`n` 個の孤立頂点を作り、頂点値を単位元、または `values` で初期化する。可変長版（`MAX_SIZE` 省略時）では `n` のデフォルト値は `0` である。コピーはできず、ムーブはできる。

## 時間計算量

- 固定長版は $O(\texttt{MAX_SIZE})$、可変長版は $O(N)$

容量分のクラスタ領域を構築し、$N$ 個の頂点クラスタを初期化する。

## `link` / `cut`

```cpp
bool link(int u, int v)
bool cut(int u, int v)
```

`link` は異なる木の頂点 `u`, `v` を辺で結んで `true` を返す。既に連結なら `false` を返す。

`cut` は辺 $(u,v)$ を削除して `true` を返す。直接の辺がなければ `false` を返す。

## 時間計算量

- ならし $O(\log N)$

## 根と連結判定

```cpp
void evert(int v)
int root(int v)
bool connected(int u, int v)
```

`evert` は `v` を表現木の根にする。`root` は `v` を含む表現木の根を返す。`connected` は 2 頂点が同じ木に属するかを返す。

## 時間計算量

- ならし $O(\log N)$

## 頂点値

```cpp
void set(int v, const S& x)
S get(int v)
```

## 時間計算量

- ならし $O(\log N)$

## パス積

```cpp
S path_prod(int u, int v)
```

`u` から `v` の順に頂点値を畳み込む。`u`, `v` は連結でなければならない。

## 時間計算量

- ならし $O(\log N)$

## クラスタ情報

```cpp
ClusterStatistics cluster_statistics(int v)
```

`v` を含む Top Tree にある `edge`、`compress`、`rake` クラスタの個数と最大深さ `depth` を返す。`total()` は 3 種類の合計を返す。

## 時間計算量

- $O(N)$

## 空間計算量

- 固定長版は $O(\texttt{MAX_SIZE})$、可変長版は $O(N)$

## 注意点

`int size() const` は頂点数を $O(1)$ で返す。

- `0 <= n <= MAX_SIZE` で、`values` の長さも `MAX_SIZE` 以下でなければならない。違反時は `runtime_error` を送出する。
- 全頂点引数は `[0, size())` でなければならない。範囲外なら `runtime_error` を送出する。
- `path_prod` は両端点が連結であることを要求し、非連結なら `runtime_error` を送出する。
- 容量を指定した固定配列版と可変長版は同じヘッダで利用できる。
