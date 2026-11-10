---
title: Minimum Spanning Tree Cost (最小全域木コスト) [BLINNET] [CSTREET]
documentation_of: ../../../../src/algorithm/graph/spanning_tree/minimum_spanning_tree_cost.hpp
---

重み付き無向グラフの最小全域木の重みの総和を返す。

## `KruskalEdge<long long>`

```cpp
template <class T>
struct KruskalEdge {
    int from;
    int to;
    T cost;
};
```

辺の両端と重みを表す。`minimum_spanning_tree_cost` では `T = long long` を使う。

## `minimum_spanning_tree_cost`

```cpp
long long minimum_spanning_tree_cost(
    int vertex_count,
    const std::vector<KruskalEdge<long long>>& edges
);
```

`vertex_count` 頂点の重み付き無向グラフについて、最小全域木の重みの総和を Kruskal 法で求めて返す。`vertex_count` が 0 または 1 なら $0$ を返す。辺の重みは負でもよい。

## 計算量

頂点数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_spanning_tree_cost` | $O(N + M\log(M + 1))$ | $O(N + M)$ |

## 注意点

- `vertex_count` は非負で、各端点は $[0, N)$ の範囲内でなければならない。違反時は `runtime_error` を送出する。
- 全頂点を結ぶ全域木が存在しない場合は `invalid_argument` を送出する。
- 総和は内部で符号付き 128 ビット整数に拡張して計算する。`long long` に収まらない場合は `overflow_error` を送出する。
