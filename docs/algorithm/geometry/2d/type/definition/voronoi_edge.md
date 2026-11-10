---
title: Voronoi Edge (Voronoi 辺)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/voronoi_edge.hpp
---

2 つのサイトのセル境界となる Voronoi 辺を保持する結果型。

## API

```cpp
inline constexpr std::size_t VORONOI_NO_VERTEX = std::numeric_limits<std::size_t>::max();

struct VoronoiEdge {
    VoronoiEdgeKind kind = VoronoiEdgeKind::SEGMENT;
    std::size_t first_site = 0;
    std::size_t second_site = 0;
    Point origin;
    Point endpoint_or_direction;
    std::size_t first_vertex = VORONOI_NO_VERTEX;
    std::size_t second_vertex = VORONOI_NO_VERTEX;
};
```

- `kind`: 辺の種類（線分・半直線・直線）。
- `first_site`, `second_site`: 辺を共有する 2 つのサイトの、元の入力における添字。
- `origin`: 線分の一端、半直線の始点、または直線上の 1 点。
- `endpoint_or_direction`: `SEGMENT` では他端、`RAY` と `LINE` では長さ 1 の方向ベクトル。
- `first_vertex`, `second_vertex`: 図の `vertices` における有限端点の添字。
- `VORONOI_NO_VERTEX`: 対応する有限端点がないことを表す値。

有限端点の個数は `SEGMENT` が 2 個、`RAY` が 1 個、`LINE` が 0 個である。

## デフォルト値

デフォルト構築では `kind == VoronoiEdgeKind::SEGMENT`、サイト添字と座標は 0、両端点の頂点添字は `VORONOI_NO_VERTEX` になる。

## 時間計算量

| 操作 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| デフォルト構築、全メンバの参照 | $O(1)$ | $O(1)$ |

## 注意点

デフォルト構築した値は有効な Voronoi 辺を表さない。`endpoint_or_direction` の意味と有限端点の添字の個数は、必ず `kind` に従って解釈する。

## 使用例

```cpp
VoronoiEdge edge;
assert(edge.first_vertex == VORONOI_NO_VERTEX);
```
