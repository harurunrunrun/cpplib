---
title: Line Arrangement Result (直線アレンジメント結果型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/line_arrangement_result.hpp
---

直線アレンジメントの有限な頂点、有界な辺、半直線、交点をもたない無限直線を格納する。

## API

```cpp
struct LineArrangementEdge { std::size_t first_vertex, second_vertex, line; };
struct LineArrangementRay { std::size_t vertex; Point direction; std::size_t line; };
struct LineArrangementUnboundedLine { Point origin; Point direction; std::size_t line; };

struct LineArrangementResult {
    std::vector<Point> vertices;
    std::vector<LineArrangementEdge> bounded_edges;
    std::vector<LineArrangementRay> rays;
    std::vector<LineArrangementUnboundedLine> unbounded_lines;
};
```

- `LineArrangementEdge` は両端の頂点番号と、元の直線の番号をもつ。
- `LineArrangementRay` は始点の頂点番号、単位方向ベクトル、直線の番号をもつ。
- `LineArrangementUnboundedLine` は通過点 `origin`、単位方向ベクトル、直線の番号をもつ。
- `LineArrangementResult` は `vertices`, `bounded_edges`, `rays`, `unbounded_lines` をもつ。

## 時間計算量

メンバの参照は $O(1)$、全走査は格納要素数に対して線形。

## 注意点

半直線の方向は、有限な頂点から無限遠へ向かう向きである。
