---
title: Steiner Tree (シュタイナー木)
documentation_of: ../../../../../src/algorithm/graph/specialized/route_optimization/steiner_tree.hpp
---

重み付き無向グラフの最小シュタイナー木の重みを求める。

## 関数

```cpp
template<class T>
T steiner_tree(
    const vector<vector<SteinerTreeEdge<T>>>& graph,
    const vector<int>& terminals,
    T inf = numeric_limits<T>::max() / 4
)
```

`graph[v]` は `SteinerTreeEdge<T>{to, cost}` の列である。無向グラフでは両方向に辺を入れる。
端点が空なら $0$ を返し、全端点を連結できない場合は `inf` を返す。

- 辺の重みは非負である必要がある。
- 内部の到達判定は `inf` とは別に管理しているため、最適値が `inf` 以上でも計算できる。ただし最適値がちょうど `inf` の場合、戻り値だけでは到達不能と区別できない。
- 部分木のコストと辺のコストの加算結果は `T` で表現できる必要がある。
- 端点数 $K$ は `int` のビット数（`numeric_limits<int>::digits`）未満である必要がある。計算量は $K$ について指数時間なので、実用上は十分小さい $K$ を使う。

## 時間計算量

頂点数を $N$、隣接リストの有向辺の総数を $M$、端点数を $K$ とする。

- `steiner_tree`: $O(3^K N+2^K(N+M)\log(M+N+1))$

第 1 項が部分集合の併合、第 2 項が各部分集合に対する Dijkstra 法である。

## 空間計算量

- DP 表と到達判定表を含めて $O(2^K N+N+M)$

## 注意点

端点の頂点番号と隣接リストの行き先は $[0,N)$ に入っていなければならない。辺重みが負の場合や $K$ が大きすぎる場合も含め、違反時は `runtime_error` を送出する。記載した計算量には引数の検査を含む。
