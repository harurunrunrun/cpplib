---
title: Shortest Path with Mode Switches (モード切替付き最短路) [ABC277_E]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/state_space/shortest_path_with_mode_switches.hpp
---

2 つのモードをもつ無向グラフで、指定された頂点上でのモード切替を許したときの最短の辺数を、頂点を (頂点, モード) に倍化したグラフ上の 0-1 BFS で求める。

## `ModeSwitchEdge`

```cpp
struct ModeSwitchEdge {
    int from;
    int to;
    int mode;
};
```

この辺は、`mode` が現在のモードと一致するときだけ両方向に移動できる。

## `shortest_path_with_mode_switches`

```cpp
int shortest_path_with_mode_switches(
    int vertex_count,
    const std::vector<ModeSwitchEdge>& edges,
    const std::vector<int>& switch_vertices,
    int source = 0,
    int target = -1,
    int initial_mode = 1
)
```

辺の移動コストを $1$、`switch_vertices` に含まれる頂点上でのモード反転のコストを $0$ としたときの、`source` から `target` への最短距離を返す。`target == -1` のときは `vertex_count - 1` を終点とする。終点ではどちらのモードでもよい。到達不能なら `-1` を返す。

## 時間計算量

頂点数を $N$、辺数を $M$、切替頂点の指定数を $K$ として $O(N + M + K)$。

## 空間計算量

$O(N + M + K)$

## 注意点

- 頂点番号は $[0, N)$ の範囲内、`mode` と `initial_mode` は $0$ または $1$ でなければならない。
- 同じ切替頂点を複数回指定しても結果は変わらない。
- 頂点番号・モードが不正な場合や、`vertex_count` が正でないか `INT_MAX / 2` を超える場合は `runtime_error` を送出する。
