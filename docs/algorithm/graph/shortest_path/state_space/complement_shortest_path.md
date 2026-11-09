---
title: Complement Shortest Path (補グラフ最短路)
documentation_of: ../../../../../src/algorithm/graph/shortest_path/state_space/complement_shortest_path.hpp
---

補グラフ上の重みなし単一始点最短路を、補グラフを明示的に作らずに BFS で求める。

`graph[v]` には元のグラフの辺を入れる。補グラフには、`v != u` かつ元のグラフに辺 `v -> u` が存在しないときに辺 `v -> u` がある。無向グラフの補グラフを扱う場合は、元のグラフの各辺を両方向に入れる。

## 関数

```cpp
ComplementShortestPathResult complement_shortest_path(
    const std::vector<std::vector<int>>& graph,
    int source
)
```

戻り値 `ComplementShortestPathResult` は次のメンバをもつ。

```cpp
std::vector<int> dist;
std::vector<int> parent;
```

- `dist[v]`: 補グラフ上での `source` から `v` への最短距離。到達不能なら `-1`。
- `parent[v]`: 最短路における `v` の直前の頂点。始点と到達不能な頂点では `-1`。

## 時間計算量

頂点数を $N$、元のグラフの辺数を $M$ として $O(N + M)$。

未到達の頂点の列を走査するとき、未到達のまま残る頂点は取り出した頂点から出る元のグラフの辺に対応するので、走査の総量は $O(N + M)$ に収まる。元のグラフの全辺の検査と長さ $N$ の結果列の構築を含む。

## 空間計算量

戻り値と探索領域を含めて $O(N)$（入力を除く）。

## 注意点

`source` や隣接リストの行き先が $[0, N)$ の範囲外なら `runtime_error` を送出する。
