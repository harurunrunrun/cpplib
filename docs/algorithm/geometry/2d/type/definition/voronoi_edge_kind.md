---
title: Voronoi Edge Kind (Voronoi 辺の種類)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/voronoi_edge_kind.hpp
---

Voronoi 辺が持つ有限端点の個数を表す列挙型。

## API

```cpp
enum class VoronoiEdgeKind {
    SEGMENT,
    RAY,
    LINE,
};
```

- `SEGMENT`: 有限端点を 2 つ持つ線分。
- `RAY`: 有限端点を 1 つ持つ半直線。
- `LINE`: 有限端点を持たない直線。

## 時間計算量

| 操作 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 構築、比較 | $O(1)$ | $O(1)$ |

## 注意点

列挙値が表すのは幾何的な形状だけである。具体的な端点や方向は、`VoronoiEdge` のメンバを `kind` に応じて解釈して得る。

## 使用例

```cpp
const VoronoiEdgeKind kind = VoronoiEdgeKind::RAY;
```
