---
title: Shortest Path with Mode Switches (モード切替付き最短路) [ABC277_E]
documentation_of: ../src/algorithm/graph/shortest_path_with_mode_switches.hpp
---

二つのモードを持つ無向グラフで、頂点上のモード切替を許した最短辺数を返す。

## ModeSwitchEdge

```cpp
struct ModeSwitchEdge {
    int from;
    int to;
    int mode;
};
```

辺は `mode` が現在モードと一致するときだけ、両方向へ移動できる。

## shortest_path_with_mode_switches

```cpp
int shortest_path_with_mode_switches(
    int vertex_count,
    const vector<ModeSwitchEdge>& edges,
    const vector<int>& switch_vertices,
    int source = 0,
    int target = -1,
    int initial_mode = 1
)
```

辺の移動コストを $1$、`switch_vertices` 上でのモード反転コストを $0$ とした
`source` から `target` への最短距離を返す。`target == -1` のときは
`vertex_count - 1` を終点とする。到達不能なら `-1` を返す。

## 時間計算量

頂点数を $N$、辺数を $M$、切替頂点の指定数を $K$ として $O(N+M+K)$。

## 空間計算量

$O(N+M+K)$。

## 注意点

- 頂点番号は $[0,N)$、`mode` と `initial_mode` は $0$ または $1$ でなければならない。
- 同じ切替頂点を複数回指定しても結果は変わらない。
- 不正な頂点番号、モード、頂点数には `runtime_error` を送出する。
