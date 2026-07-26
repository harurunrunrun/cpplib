---
title: Visibility Graph Edge (可視グラフの辺)
documentation_of: ../../../../src/algorithm/geometry/2d/visibility_graph_edge.hpp
---

単純多角形の可視グラフに含まれる無向辺を保持する型。

## API

```cpp
struct VisibilityGraphEdge {
    std::size_t first;
    std::size_t second;
    long double distance;
};
```

- `first`: 辺の小さい方の入力頂点添字。
- `second`: 辺の大きい方の入力頂点添字。
- `distance`: 両端点間のEuclid距離。

## 時間計算量

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| aggregate構築、コピー、各fieldの参照 | $O(1)$ | $O(1)$ |

## 注意点

`visibility_graph` が返す辺では常に `first < second` であり、`distance` は非負である。
既定初期化だけではfield値は初期化されないため、値初期化またはaggregate初期化を用いる。
