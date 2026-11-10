---
title: Almost Shortest Path (ほぼ最短路) [SAMER08A]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/enumeration/almost_shortest_path.hpp
---

非負重みの有向グラフで、`source` から `target` へのいずれかの最短路に含まれる辺をすべて取り除き、残ったグラフでの最短距離を求める。辺は入力での位置で区別するため、同じ端点をもつ多重辺も個別に判定する。

## `AlmostShortestPathEdge`

```cpp
template <class T>
using AlmostShortestPathEdge = ShortestPathEdgeUsageEdge<T>;
```

`AlmostShortestPathEdge<T>{from, to, cost}` で有向辺を表す。

## `AlmostShortestPathResult`

```cpp
template <class T>
struct AlmostShortestPathResult {
    T distance;
    bool reachable;
    std::vector<char> removed;
};
```

- `reachable`: 最短路の辺をすべて取り除いた後も `target` に到達できるなら真。
- `distance`: `reachable` が真なら残ったグラフでの最短距離、偽なら引数の `inf`。
- `removed[i]`: 入力の辺 `edges[i]` が元のグラフのいずれかの最短路に含まれ、取り除かれたなら真。

## `almost_shortest_path`

```cpp
template <class T>
AlmostShortestPathResult<T> almost_shortest_path(
    int n,
    const std::vector<AlmostShortestPathEdge<T>>& edges,
    int source,
    int target,
    T inf = std::numeric_limits<T>::max() / 4
)
```

`shortest_path_edge_usage` で最短路に含まれる辺を求め、それらを除いたグラフで Dijkstra 法を実行する。

## 計算量

$N = n$、$M$ を `edges` の要素数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `almost_shortest_path` | $O((N + M) \log (N + M + 1))$ | $O(N + M)$ |

## 注意点

- `source`, `target` と各辺の `from`, `to` は $[0, N)$ の範囲内で、辺の重みは非負でなければならない。違反時は `runtime_error` を送出する。
- `source == target` のときは空の路を最短路として扱い、辺を取り除かずに `reachable = true`, `distance = 0` を返す。
- 元のグラフで `target` に到達できない場合は辺を取り除かず、`reachable = false`, `distance = inf` を返す。
- 重み $0$ の辺と多重辺を扱える。
- 到達可能な経路上での距離の加算結果は `T` で表現できなければならない。
