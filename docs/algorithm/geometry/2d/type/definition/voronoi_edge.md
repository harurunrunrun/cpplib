---
title: Voronoi Edge (Voronoi辺)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/voronoi_edge.hpp
---

2つのsiteのcell境界となるVoronoi辺を保持する結果型。

## API

```cpp
inline constexpr std::size_t VORONOI_NO_VERTEX;

struct VoronoiEdge {
    VoronoiEdgeKind kind;
    std::size_t first_site;
    std::size_t second_site;
    Point origin;
    Point endpoint_or_direction;
    std::size_t first_vertex;
    std::size_t second_vertex;
};
```

- `first_site`, `second_site`: 辺を共有する2つのsiteの元入力添字。
- `origin`: 線分の一端、半直線の始点、または直線上の1点。
- `endpoint_or_direction`: `SEGMENT` では他端、`RAY` と `LINE` では長さ1の方向。
- `first_vertex`, `second_vertex`: 図の `vertices` に対する有限端点の添字。
- `VORONOI_NO_VERTEX`: 対応する有限端点がないことを表す値。

`SEGMENT` は2つ、`RAY` は1つ、`LINE` は0個の有限端点を持つ。

## 既定値

既定構築では `kind == SEGMENT`、site添字と座標は0、両頂点添字は
`VORONOI_NO_VERTEX` になる。

## 時間計算量

| 操作 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 既定構築、全fieldの参照 | $O(1)$ | $O(1)$ |

## 注意点

既定構築値は有効なVoronoi辺を意味しない。`endpoint_or_direction` の意味と
有限頂点添字の個数は必ず `kind` に従って解釈する。

## 使用例

```cpp
VoronoiEdge edge;
assert(edge.first_vertex == VORONOI_NO_VERTEX);
```
