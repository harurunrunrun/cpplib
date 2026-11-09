---
title: SPFA Shortest Paths (SPFA最短路)
documentation_of: ../../../../../src/algorithm/graph/shortest_path/source/spfa.hpp
---

負の辺を含む重み付き有向グラフの単一始点・多始点最短路を SPFA (Shortest Path Faster Algorithm) で求める。始点から到達可能な負閉路がある場合は、その負閉路から到達可能で最短距離が下に有界でない頂点も列挙する。

## 型

### `SpfaEdge<T>`

- `to`: 辺の終点。
- `cost`: 辺の重み。負でもよい。

### `SpfaResult<T>`

- `distance[v]`: 到達可能かつ負閉路の影響を受けない頂点 `v` の最短距離。未到達なら `inf`。
- `reachable[v]`: いずれかの始点から到達可能なら 1。
- `negative_cycle_affected[v]`: 到達可能な負閉路から `v` へ到達可能なら 1。
- `parent[v]`: 通常の最短路木における直前の頂点。一度も更新されていない始点と未到達の頂点では `-1`。
- `has_reachable_negative_cycle`: 始点集合から到達可能な負閉路があれば `true`。

## 関数

```cpp
template <class T>
SpfaResult<T> spfa(const std::vector<std::vector<SpfaEdge<T>>>& graph, int source,
                   T inf = std::numeric_limits<T>::max() / 4)
template <class T>
SpfaResult<T> spfa(const std::vector<std::vector<SpfaEdge<T>>>& graph, const std::vector<int>& sources,
                   T inf = std::numeric_limits<T>::max() / 4)
```

- 1 つ目は `source` を始点として実行する。
- 2 つ目は `sources` のすべての頂点を距離 $0$ の始点として多始点最短路を求める。同じ始点が複数回現れてもよい。空集合なら全頂点が未到達になる。

`inf` は未到達の頂点の `distance` に格納する値である。

## 使用例

```cpp
std::vector<std::vector<SpfaEdge<long long>>> graph(3);
graph[0].push_back({1, 4});
graph[1].push_back({2, -7});
auto result = spfa(graph, 0);
```

## 負閉路の扱い

`negative_cycle_affected[v] == 1` の頂点には有限の最短距離が存在しない。このような頂点の `distance` と `parent` は使ってはならない。始点から到達不能な負閉路は `has_reachable_negative_cycle` と `negative_cycle_affected` に影響しない。

## 計算量

$N$ を頂点数、$M$ を辺数、$S$ を始点の個数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `SpfaEdge<T>` のメンバ参照 | $O(1)$ | $O(1)$ |
| `SpfaResult<T>` の各メンバ参照 | $O(1)$ | $O(1)$ |
| `spfa(graph, source, inf)` | 最悪 $O(NM)$ | $O(N + M)$ |
| `spfa(graph, sources, inf)` | 最悪 $O(NM + S)$ | $O(N + M)$ |

負閉路の影響を受ける頂点を求める走査は $O(N + M)$ で、上記に含まれる。

## 注意点

- 辺の終点と始点は $[0, N)$ の範囲内でなければならない。違反時は `runtime_error` を送出する。
- `T` は $0$ の構築、加算、大小比較をサポートしなければならない。`inf` を省略する場合は `std::numeric_limits<T>::max()` を 4 で割れる必要がある。
- 評価される経路の重みの和は `T` で表現できなければならない。
- `inf` は未到達時の格納値にすぎないので、到達判定には `reachable` を使う。
- 非負の辺だけのグラフでは Dijkstra 法の方が安定して高速である。
- SPFA は平均的には速いことがあるが、最悪計算量は Bellman–Ford 法と同じ $O(NM)$ である。
