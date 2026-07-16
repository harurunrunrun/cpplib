---
title: Directed MST (有向最小全域木) [GRL_2_B]
documentation_of: ../src/algorithm/graph/directed_mst.hpp
---

最小有向全域木。rootから全頂点へ到達する有向木の最小コストを求める。

## 関数

```cpp
directed_mst<T>(n, root, edges, inf)
```

`edges` は `DirectedMstEdge<T>{from, to, cost}` の列。

`DirectedMstResult<T>` は次を持つ。

```cpp
T cost;
bool exists;
```

存在しない場合 `exists` は `false`。

有向全域木のコストと縮約時の辺コストは `T` で表現できる必要がある。辺コストが `inf` 以上でも扱える。

## 時間計算量

$N$ を元の頂点数、$M$ を元の辺数とする。

- `directed_mst`: $O(N(N+M))$

1回の最小流入辺選択・閉路検出・縮約は $O(N+M)$ で、縮約は高々 $N$ 回。戻り値の構築は $O(1)$。

## 空間計算量

- $O(N+M)$

## 注意点

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
