---
title: Delaunay Triangulation from Points (点列のDelaunay三角形分割)
documentation_of: ../../../../src/algorithm/geometry/2d/delaunay_triangulation_points.hpp
---

二次元点列のDelaunay三角形分割を構築する。

## API

```cpp
DelaunayTriangulationResult delaunay_triangulation(
    const std::vector<Point>& points
);
```

重複点をまとめ、三角形、無向辺、各入力点の代表、site代表列を返す。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 空間計算量（返り値を含む） |
| --- | --- | --- |
| `delaunay_triangulation(points)` | $O(N\log N)$ | $O(N)$ |

## 注意点

- 座標は有限でなければならず、NaNまたは無限大には `std::invalid_argument` を送出する。
- 外接円判定を有限な `long double` で計算できない場合は `std::overflow_error` を送出する。
- 0点または1種類の点では辺と三角形は空となる。
- 全siteが一直線上なら座標順で隣接するsiteの辺だけを返す。
- 重複判定と幾何述語にはライブラリ共通の許容誤差を用いる。

## 使用例

```cpp
std::vector<Point> points{
    {0, 0}, {2, 0}, {0, 2}, {2, 2},
};
const DelaunayTriangulationResult result = delaunay_triangulation(points);
```
