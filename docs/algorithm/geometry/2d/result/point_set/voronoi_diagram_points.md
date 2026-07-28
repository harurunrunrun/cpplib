---
title: Voronoi Diagram from Points (点列のVoronoi図)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/voronoi_diagram_points.hpp
---

二次元点列をsiteとするVoronoi図を構築する。

## API

```cpp
VoronoiDiagramResult voronoi_diagram(
    const std::vector<Point>& points
);
```

Delaunay三角形分割を双対化し、有限頂点、線分、半直線、直線と各cellの辺を返す。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 空間計算量（返り値を含む） |
| --- | --- | --- |
| `voronoi_diagram(points)` | $O(N\log N)$ | $O(N)$ |

## 注意点

- 座標は有限でなければならず、NaNまたは無限大には `std::invalid_argument` を送出する。
- Delaunay述語または外心計算が有限範囲を超える場合は `std::overflow_error` を送出する。
- 0点または1種類のsiteでは頂点と辺は空となる。
- 全siteが一直線上なら隣接site間の垂直二等分線を `LINE` として返す。
- 非退化な凸包に対応する無限辺は `RAY` として返す。
- `cell_edges` の順序は未規定である。

## 使用例

```cpp
std::vector<Point> points{
    {0, 0}, {2, 0}, {1, 2},
};
const VoronoiDiagramResult result = voronoi_diagram(points);
```
