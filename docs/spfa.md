---
title: SPFA Shortest Paths (SPFA最短路)
documentation_of: ../src/algorithm/graph/shortest_path/spfa.hpp
---

負辺を含む重み付き有向グラフの単一始点・多始点最短路を求める。
始点から到達可能な負閉路がある場合は、その負閉路から到達可能で
最短距離が下に有界でない頂点も列挙する。

## 型

### `SpfaEdge<T>`

- `to`
    - 辺の終点。
- `cost`
    - 辺重み。負でもよい。

### `SpfaResult<T>`

- `distance[v]`
    - 到達可能かつ負閉路の影響を受けない頂点 `v` の最短距離。
- `reachable[v]`
    - いずれかの始点から到達可能なら1。
- `negative_cycle_affected[v]`
    - 到達可能な負閉路から `v` へ到達可能なら1。
- `parent[v]`
    - 通常の最短路木における直前の頂点。更新されていない始点と未到達頂点では `-1`。
- `has_reachable_negative_cycle`
    - 始点集合から到達可能な負閉路があれば `true`。

## 関数

### `spfa(graph, source, inf)`

`source` を始点として実行する。
`inf` は未到達頂点の `distance` に格納する値。

### `spfa(graph, sources, inf)`

`sources` の全頂点を距離0として多始点最短路を求める。
同じ始点が複数回現れてもよい。空集合なら全頂点が未到達となる。

辺の終点と始点は `[0, graph.size())` でなければならない。
違反時は `runtime_error`。

## 使用例

```cpp
std::vector<std::vector<SpfaEdge<long long>>> graph(3);
graph[0].push_back({1, 4});
graph[1].push_back({2, -7});
auto result = spfa(graph, 0);
```

## 負閉路の扱い

`negative_cycle_affected[v] == 1` の頂点では有限の最短距離が存在しない。
この頂点の `distance` と `parent` は使用しない。
始点から到達不能な負閉路は
`has_reachable_negative_cycle` と `negative_cycle_affected` に影響しない。

## API別の時間計算量・空間計算量

$N$ を頂点数、$M$ を辺数、$S$ を始点数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `SpfaEdge<T>` のfield参照 | $O(1)$ | $O(1)$ |
| `SpfaResult<T>` の各field参照 | $O(1)$ | $O(1)$ |
| `spfa(graph,source,inf)` | 最悪 $O(NM)$ | $O(N+M)$ |
| `spfa(graph,sources,inf)` | 最悪 $O(NM+S)$ | $O(N+M)$ |

負閉路の影響頂点を求める走査は $O(N+M)$ で、上記に含む。

## 注意点

- `T` は0の構築、加算、大小比較をサポートする必要がある。
- infを省略する場合は numeric_limits の max を4で除算できる必要がある。
- 評価される経路重みの和は `T` で表現できなければならない。
- `inf` は未到達時の格納値であり、到達判定には
  `reachable` を使用する。
- 非負辺だけのグラフではDijkstra法の方が安定して高速。
- SPFAの平均的な実行時間は短い場合があるが、最悪計算量は
  Bellman--Ford法と同じ $O(NM)$。
