---
title: Dynamic Tree Vertex Subtree Sum (動的木頂点部分木和) [vertex_add_subtree_sum]
documentation_of: ../src/structure/tree/dynamic_tree_vertex_subtree_sum.hpp
---

静的な根付き無向木の各頂点に加法群の値を持ち、頂点への加算と部分木の総和を処理する。
木の辺と根は構築後に変更しない。

## 型

```cpp
DynamicTreeVertexSubtreeSum<T>
```

`T` は零元としてのdefault構築、`+=`、減算をサポートする型。

## コンストラクタ

```cpp
DynamicTreeVertexSubtreeSum(
    const vector<T>& values,
    const vector<pair<int, int>>& edges,
    int root = 0
)
```

`values[v]` を頂点 `v` の初期値として、`edges` から根 `root` の木を構築する。
`values` の長さを頂点数とし、非空木では辺をちょうど `N-1` 本指定する。
空木では `edges` は空、`root` は `0` とする。

## size

```cpp
int size() const
```

頂点数を返す。

## add

```cpp
void add(int vertex, const T& delta)
```

頂点 `vertex` の値に `delta` を加える。

## subtree_sum

```cpp
T subtree_sum(int vertex) const
```

構築時の根に対する `vertex` の部分木にある頂点値の総和を返す。

## 時間計算量

- コンストラクタ: $O(N)$
- `size`: $O(1)$
- `add`: $O(\log(N+1))$
- `subtree_sum`: $O(\log(N+1))$

## 空間計算量

木、HL分解、Fenwick索引を含めて $O(N)$。

## 注意点

- 頂点番号は `0` 以上 `size()` 未満でなければならない。
- 辺数、端点、根、連結性、閉路に違反した場合は `runtime_error`。
- `T` の演算がoverflowする場合の動作は `T` の演算規則に従う。
