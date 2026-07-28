---
title: Ear Clipping Triangulation from Polygon Points (多角形頂点列の耳切り法三角形分割)
documentation_of: ../../../../../../src/algorithm/geometry/2d/triangle/point_set/ear_clipping_triangulation_points.hpp
---

単純多角形の周回頂点列を入力添字の三角形列へ分割する。
API名は互換性のため維持するが、実装は単調分割を用いる。

## API

```cpp
std::vector<EarClippingTriangle> ear_clipping_triangulation(
    const std::vector<Point>& polygon
);
```

時計回り・反時計回りの両方を受理し、返す三角形を反時計回りに揃える。
連続重複点、末尾に重ねた始点、辺上の中間点を除いてから分割する。

## 時間計算量

入力頂点数を $N$ とする。

| API | 時間計算量 | 空間計算量（返り値を含む） |
| --- | --- | --- |
| `ear_clipping_triangulation(polygon)` | $O(N\log N)$ | $O(N)$ |

## 注意点

入力は面積非零の単純多角形でなければならない。不正・退化入力には
`std::invalid_argument`、安全な内部サイズを計算できない場合には `std::length_error` を送出し得る。
