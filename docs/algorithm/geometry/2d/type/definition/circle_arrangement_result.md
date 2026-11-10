---
title: Circle Arrangement Result (円アレンジメント結果型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/circle_arrangement_result.hpp
---

円アレンジメントの頂点と円弧を格納する結果型。

## API

```cpp
struct CircleArrangementArc {
    std::size_t circle;
    std::size_t first_vertex;
    std::size_t second_vertex;
    long double first_angle;
    long double second_angle;
    bool full_circle;
};

struct CircleArrangementResult {
    std::vector<Point> vertices;
    std::vector<CircleArrangementArc> arcs;
};
```

- `CircleArrangementArc` は、円の番号 `circle`、両端の頂点番号、角度範囲 `[first_angle, second_angle]`、全円かどうかのフラグ `full_circle` をもつ。
- `CircleArrangementResult` は頂点列 `vertices` と円弧列 `arcs` をもつ。

## 時間計算量

各メンバの参照は $O(1)$、全走査は格納要素数に対して線形。

## 注意点

頂点をもたない全円の弧では、両端の頂点番号は `static_cast<std::size_t>(-1)` である。
