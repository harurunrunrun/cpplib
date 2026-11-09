---
title: Delaunay Triangulation from Points (点列の Delaunay 三角形分割)
documentation_of: ../../../../../../src/algorithm/geometry/2d/result/point_set/delaunay_triangulation_points.hpp
---

2 次元の点列に対する Delaunay 三角形分割を構築する。

## API

```cpp
DelaunayTriangulationResult delaunay_triangulation(
    const std::vector<Point>& points
);
```

重複点を 1 つの site にまとめてから分割統治法で三角形分割を構築する。返り値 `DelaunayTriangulationResult` のメンバは次のとおり。添字はすべて入力列の添字である。

- `triangles`: 三角形の頂点添字の組。各組は最小の添字が先頭に来るように回転してあり、全体はソート済みである。共円な 4 点以上からなる面は扇状に三角形分割される。
- `edges`: 無向辺 `(first, second)`（`first < second`）のソート済み列。
- `representative`: 各入力点が属する site の代表添字。
- `sites`: 各 site の代表添字の列。代表は同じ site に属する点のうち最小の添字である。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 空間計算量（返り値を含む） |
| --- | --- | --- |
| `delaunay_triangulation(points)` | $O(N \log N)$ | $O(N)$ |

## 注意点

- 座標は有限でなければならず、NaN または無限大を含むと `std::invalid_argument` を送出する。
- 外接円判定がオーバーフローしないよう、座標の絶対値が $(\texttt{LDBL_MAX} / 256)^{1/4}$ を超える場合は `std::overflow_error` を送出する。
- 点が 0 個、または site が 1 種類しかない場合、辺と三角形は空になる。
- すべての site が一直線上にある場合は、座標順で隣接する site 間の辺だけを返す。
- 重複判定と幾何述語にはライブラリ共通の許容誤差を用いる。

## 使用例

```cpp
std::vector<Point> points{
    {0, 0}, {2, 0}, {0, 2}, {2, 2},
};
const DelaunayTriangulationResult result = delaunay_triangulation(points);
```
