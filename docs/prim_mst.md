---
title: Prim MST (プリム法)
documentation_of: ../src/algorithm/graph/spanning_tree/prim_mst.hpp
---

Prim法で最小全域木を求める。

## 関数

```cpp
prim_mst<T>(graph)
```

`graph[v]` は `PrimEdge<T>{to, cost}` の列。無向グラフでは両方向に辺を入れる。

`PrimMstResult<T>` は次を持つ。

```cpp
T cost;
vector<PrimTreeEdge<T>> edges;
bool connected;
```

非連結なら最小全域森を返し、`connected` は `false`。

## 時間計算量

$N$ を頂点数、$M$ を隣接リストの総要素数とする。

- `prim_mst`: $O(N+M\log(M+1))$

非連結時も全成分を処理する。森の出力辺数は高々 $N-1$。

## 空間計算量

- 戻り値とpriority queueを含めて $O(N+M)$

## 注意点

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
