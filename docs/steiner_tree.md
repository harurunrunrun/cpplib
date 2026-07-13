---
title: Steiner Tree
documentation_of: ../src/algorithm/graph/steiner_tree.hpp
---

無向重み付きグラフの最小シュタイナー木。

# 関数

```cpp
steiner_tree<T>(graph, terminals, inf)
```

`graph[v]` は `SteinerTreeEdge<T>{to, cost}` の列。無向グラフでは両方向に辺を入れる。

到達不能な場合は `inf` を返す。

辺の重みは非負である必要がある。

内部の到達判定は `inf` と分離されているため、最適値が `inf` 以上でも計算できる。ただし最適値がちょうど `inf` の場合、戻り値だけでは到達不能と区別できない。

部分木のコストと辺コストの加算結果は `T` で表現できる必要がある。

端点数 `K` は `int` のビット数未満である必要がある。計算量は指数時間なので、実用上は十分小さい `K` を使う。

## 時間計算量

端点数を $K$ とする。

- `steiner_tree`: $O(3^K N+2^K M\log(N+1))$

subset併合が第1項、各subsetに対するDijkstraが第2項。戻り値の構築は $O(1)$。

## 空間計算量

- DP tableと到達tableを含めて $O(2^K N+N+M)$

## API契約・前提・例外

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
