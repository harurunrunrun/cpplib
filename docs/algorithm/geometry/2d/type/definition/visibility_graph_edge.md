---
title: Visibility Graph Edge (可視グラフの辺)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/visibility_graph_edge.hpp
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

- `first`: 辺の端点のうち、小さい方の入力頂点の添字。
- `second`: 辺の端点のうち、大きい方の入力頂点の添字。
- `distance`: 両端点間のユークリッド距離。

## 時間計算量

| 操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 集成体初期化による構築、コピー、各メンバの参照 | $O(1)$ | $O(1)$ |

## 注意点

`visibility_graph` が返す辺では常に `first < second` であり、`distance` は非負である。
メンバにデフォルトメンバ初期化子はないため、デフォルト初期化だけではメンバの値は不定である。値初期化（`VisibilityGraphEdge{}`）または集成体初期化を用いる。
