---
title: Voronoi Diagram from Points (点列の Voronoi 図)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/voronoi_diagram_points.hpp
---

2 次元の点列を site とする Voronoi 図を構築する。

## API

```cpp
VoronoiDiagramResult voronoi_diagram(
    const std::vector<Point>& points
);
```

Delaunay 三角形分割の双対をとり、有限な頂点と、線分・半直線・直線の辺、および各セルの辺を返す。返り値 `VoronoiDiagramResult` のメンバは次のとおり。

- `vertices`: Voronoi 頂点の座標（座標順）。共円な三角形の外心は 1 つの頂点にまとめる。
- `edges`: `VoronoiEdge` の列。種別 `kind` が `SEGMENT`（線分）、`RAY`（半直線）、`LINE`（直線）のいずれかで、隣接する 2 つの site を `first_site`, `second_site` に持つ。
- `cell_edges`: 入力添字ごとの、そのセルを囲む辺の添字の列。site の代表添字の位置にのみ要素が入る。
- `representative`, `sites`: `delaunay_triangulation` と同じ意味の site 代表情報。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 空間計算量（返り値を含む） |
| --- | --- | --- |
| `voronoi_diagram(points)` | $O(N \log N)$ | $O(N)$ |

## 注意点

- 座標は有限でなければならず、NaN または無限大を含むと `std::invalid_argument` を送出する。
- Delaunay 述語または外心の計算が有限な範囲を超える場合は `std::overflow_error` を送出する。
- 点が 0 個、または site が 1 種類しかない場合、頂点と辺は空になる。
- すべての site が（与えられた座標のもとで厳密に）一直線上にある場合は、隣接する site 間の垂直二等分線を `LINE` として返す。ほぼ一直線上に並ぶ site からは、非常に遠い頂点ができうる。
- 外接円判定が許容誤差の範囲で共円とみなす隣接三角形の外心は、1 つの頂点にまとめる。
- 非退化な場合、凸包の辺に対応する無限辺は `RAY` として返す。
- `cell_edges` の各列内の順序は未規定である。

## 使用例

```cpp
std::vector<Point> points{
    {0, 0}, {2, 0}, {1, 2},
};
const VoronoiDiagramResult result = voronoi_diagram(points);
```
