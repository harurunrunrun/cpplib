---
title: Dynamic Forest Vertex Add Path Sum (動的森頂点加算パス和) [dynamic_tree_vertex_add_path_sum]
documentation_of: ../src/structure/tree/dynamic_forest_vertex_add_path_sum.hpp
---

頂点に値を持つ動的森について、辺の追加・削除・置換、頂点加算、パス和を処理する。
Link-Cut Tree、遅延Link-Cut Tree、Top Tree、遅延Top Treeを基盤とする4種類を提供する。
以下では、この4種類のいずれかを `Tree` と表す。

## 型

```cpp
DynamicForestVertexAddPathSumLinkCutTree<T, MAX_SIZE>
DynamicForestVertexAddPathSumLazyLinkCutTree<T, MAX_SIZE>
DynamicForestVertexAddPathSumTopTree<T, MAX_SIZE>
DynamicForestVertexAddPathSumLazyTopTree<T, MAX_SIZE>
```

`MAX_SIZE` は格納できる頂点数の上限。
非遅延版の `T` は零元としてのdefault構築と加算をサポートする型とする。
遅延版ではさらに、`T(long long)` と乗算をサポートする型とする。

## コンストラクタ

```cpp
Tree(int n = MAX_SIZE)
Tree(const vector<T>& values)
```

前者は `n` 個の孤立頂点を値 `T{}` で作る。
後者は `values[v]` を頂点 `v` の初期値として孤立頂点を作る。

## size

```cpp
int size() const
```

頂点数を返す。

## connected

```cpp
bool connected(int left, int right)
```

`left` と `right` が同じ木に属するかを返す。

## link

```cpp
bool link(int left, int right)
```

異なる木に属する2頂点を辺で結ぶ。
追加できた場合は `true`、既に連結で閉路になる場合は `false` を返す。

## cut

```cpp
bool cut(int left, int right)
```

辺 `left-right` を削除する。
削除できた場合は `true`、その辺が存在しない場合は `false` を返す。

## replace_edge

```cpp
bool replace_edge(int old_left, int old_right, int new_left, int new_right)
```

辺 `old_left-old_right` を削除し、辺 `new_left-new_right` を追加する。
両方に成功した場合は `true` を返す。
旧辺が存在しない場合は何も変更せず `false` を返す。
新辺が閉路を作る場合は旧辺を復元して `false` を返す。

## add_vertex

```cpp
void add_vertex(int vertex, const T& delta)
```

頂点 `vertex` の値に `delta` を加える。

## path_sum

```cpp
T path_sum(int left, int right)
```

端点を含む単純パス `left-right` 上の頂点値の総和を返す。

## 時間計算量

頂点数を $N$ とする。

- コンストラクタ: $O(MAX\_SIZE+N\log(N+1))$
- `size`: $O(1)$
- `connected`: amortized $O(\log(N+1))$
- `link`: amortized $O(\log(N+1))$
- `cut`: amortized $O(\log(N+1))$
- `replace_edge`: amortized $O(\log(N+1))$
- `add_vertex`: amortized $O(\log(N+1))$
- `path_sum`: amortized $O(\log(N+1))$

## 空間計算量

$O(MAX\_SIZE)$。

## 注意点

- 頂点番号は `0` 以上 `size()` 未満でなければならない。違反時は `runtime_error`。
- `path_sum` の2頂点が非連結の場合は `runtime_error`。
- 4つの型はcopyできず、moveできる。
- `T` の演算がoverflowする場合の動作は `T` の演算規則に従う。
