---
title: Constrained Delaunay Triangulation Result (制約付きDelaunay三角形分割結果型)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/constrained_delaunay_triangulation_result.hpp
---

制約付き Delaunay 三角形分割の三角形、全辺、制約辺を格納する。

## API

```cpp
struct ConstrainedDelaunayTriangulationResult {
    std::vector<std::array<std::size_t, 3>> triangles;
    std::vector<std::pair<std::size_t, std::size_t>> edges;
    std::vector<std::pair<std::size_t, std::size_t>> constraints;
};
```

- `triangles`: 三角形の 3 頂点の番号。
- `edges`: 三角形分割の全辺。
- `constraints`: 制約辺。

## 時間計算量

メンバの参照は $O(1)$、各列の全走査は列の長さに対して線形。

## 注意点

頂点番号は元の点列に対する 0-indexed の添字である。
