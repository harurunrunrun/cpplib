---
title: Prim MST (プリム法)
documentation_of: ../../../../src/algorithm/graph/spanning_tree/prim_mst.hpp
---

Prim 法で最小全域木を求める。

## 関数

```cpp
template <class T>
PrimMstResult<T> prim_mst(const std::vector<std::vector<PrimEdge<T>>>& graph)
```

`graph[v]` は `PrimEdge<T>{to, cost}` の列で、頂点 `v` から出る辺を表す。無向グラフでは各辺を両方向に入れる。

戻り値 `PrimMstResult<T>` は次のメンバをもつ。

```cpp
T cost;
std::vector<PrimTreeEdge<T>> edges;
bool connected;
```

- `cost`: 採用した辺のコストの総和。
- `edges`: 採用した辺 `PrimTreeEdge<T>{from, to, cost}` の列。
- `connected`: グラフが連結なら `true`。

非連結なら最小全域森を返し、`connected` は `false` になる。

## 時間計算量

$N$ を頂点数、$M$ を隣接リストの総要素数として $O(N + M\log(M + 1))$。

非連結の場合もすべての連結成分を処理する。出力する森の辺数は高々 $N-1$ である。

## 空間計算量

戻り値と優先度付きキューを含めて $O(N + M)$。

## 注意点

隣接リストの行き先が $[0, N)$ の範囲外なら `runtime_error` を送出する。
