---
title: Almost Shortest Path (ほぼ最短路) [SAMER08A]
documentation_of: ../src/algorithm/graph/almost_shortest_path.hpp
---

非負重み有向グラフで、`source` から `target` へのいずれかの最短路に含まれる辺をすべて除き、残ったグラフの最短距離を求める。
辺は入力位置で区別するため、同じ端点を持つ並列辺も個別に判定する。

# AlmostShortestPathEdge

```cpp
template<class T>
using AlmostShortestPathEdge = ShortestPathEdgeUsageEdge<T>;
```

`AlmostShortestPathEdge<T>{from, to, cost}` で有向辺を表す。

# AlmostShortestPathResult

```cpp
template<class T>
struct AlmostShortestPathResult {
    T distance;
    bool reachable;
    vector<char> removed;
};
```

- `reachable`: 最短路辺をすべて除いた後も `target` に到達できるなら真。
- `distance`: `reachable` なら残ったグラフの最短距離、偽なら引数 `inf`。
- `removed[i]`: 入力辺 `edges[i]` が元のグラフの最短路のいずれかに含まれ、除外されたなら真。

# almost_shortest_path

```cpp
template<class T>
AlmostShortestPathResult<T> almost_shortest_path(
    int n,
    const vector<AlmostShortestPathEdge<T>>& edges,
    int source,
    int target,
    T inf = numeric_limits<T>::max() / 4
)
```

正方向・逆方向のDijkstra法で全最短路に使われ得る辺を判定し、それらを除外してもう一度Dijkstra法を実行する。

## API別の時間計算量・空間計算量

$N=n$、$M=edges.size()$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `almost_shortest_path(n, edges, source, target, inf)` | $O((N+M)\log(N+1))$ | $O(N+M)$ |

## 注意点

- `source`, `target`, `from`, `to` は $[0,N)$ に含まれ、辺重みは非負でなければならない。違反時は `runtime_error` を送出する。
- `source == target` では空路を最短路として扱い、辺を除外せず `reachable = true`, `distance = 0` を返す。
- 元のグラフで `target` に到達不能なら辺は除外されず、`reachable = false`, `distance = inf` を返す。
- 0重み辺と並列辺を扱える。
- 到達可能な経路で行う距離加算は `T` で表現できる必要がある。
