---
title: Ear Clipping Triangulation Aggregator (耳切り法三角形分割集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/ear_clipping_triangulation.hpp
---

三角形の添字型と、単純多角形の頂点列を入力とする API をまとめた後方互換の集約ヘッダである。

## 構成

| leaf header | 提供する要素 |
| --- | --- |
| `ear_clipping_triangle.hpp` | `EarClippingTriangle` |
| `ear_clipping_triangulation_points.hpp` | `ear_clipping_triangulation(polygon)` |
| `ear_clipping_triangulation_detail.hpp` | 単純性の検査・単調多角形への分割・三角形分割の内部実装 |

points の leaf header は detail の leaf header に一方向に依存する。`simple_polygon_triangulation.hpp` も points の leaf header を利用するので、2 つの公開ヘッダの間に循環 include はない。

## 集約される API

```cpp
using EarClippingTriangle = std::array<std::size_t, 3>;
std::vector<EarClippingTriangle> ear_clipping_triangulation(const std::vector<Point>& polygon);
```

単純多角形を、入力の添字からなる反時計回りの三角形に分割する。

## 計算量

入力の頂点数を $N$ とする。

| API・操作 | 時間計算量 | 空間計算量（返り値を含む） |
| --- | --- | --- |
| `ear_clipping_triangulation(polygon)` | $O(N\log N)$ | $O(N)$ |
| 三角形の構築・添字の参照 | $O(1)$ | $O(1)$ |

## 注意点

- 自己交差、隣接しない位置での重複点、隣接する辺の重なり、正規化後に 3 頂点未満となる入力には `std::invalid_argument` を送出する。
- 座標と中間値は有限でなければならない。
