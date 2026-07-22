---
title: Shortest Path Edge Usage (最短路での辺利用判定)
documentation_of: ../src/algorithm/graph/shortest_path/shortest_path_edge_usage.hpp
---

各辺が `source` から `target` への最短路のいずれかに含まれるかを判定する。

辺の重みは非負である必要がある。

## 関数

```cpp
shortest_path_edge_usage<T>(n, edges, source, target, inf)
```

`edges` は入力順の `ShortestPathEdgeUsageEdge<T>{from, to, cost}`。

`ShortestPathEdgeUsageResult<T>` は次を持つ。

```cpp
T shortest;
bool reachable;
vector<char> used;
vector<char> unused;
```

`used[i]` は辺 `i` が最短路に含まれることを表す。

`reachable` は `target` が到達可能なら真。`inf` は到達不能時の `shortest` に格納する初期値であり、到達可能な最短距離が `inf` 以上でも扱える。

距離と辺コストの加算結果は `T` で表現できる必要がある。

## 時間計算量

$N$ を頂点数、$M$ を辺数とする。

- `shortest_path_edge_usage`: $O((N+M)\log(N+1))$

正方向・逆方向のDijkstraを1回ずつ行い、最後に $M$ 辺分の `used` / `unused` を構築する。

## 空間計算量

- 戻り値と2つの隣接リストを含めて $O(N+M)$

## 注意点

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
