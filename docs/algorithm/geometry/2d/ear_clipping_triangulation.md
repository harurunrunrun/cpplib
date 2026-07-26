---
title: Ear Clipping Triangulation Aggregator (耳切り法三角形分割集約ヘッダ)
documentation_of: ../../../../src/algorithm/geometry/2d/ear_clipping_triangulation.hpp
---

三角形の添字型と単純多角形の頂点列入力APIをまとめる後方互換集約ヘッダ。

## 構成

| leaf header | 提供する要素 |
| --- | --- |
| `ear_clipping_triangle.hpp` | `EarClippingTriangle` |
| `ear_clipping_triangulation_points.hpp` | `ear_clipping_triangulation(polygon)` |
| `ear_clipping_triangulation_detail.hpp` | 単純性検査・単調分割・三角形分割の内部実装 |

points leafがdetail leafへ一方向に依存する。`simple_polygon_triangulation.hpp` もpoints leafを
利用するため、両公開ヘッダー間に循環includeはない。

## 集約されるAPI

```cpp
using EarClippingTriangle = std::array<std::size_t, 3>;
std::vector<EarClippingTriangle> ear_clipping_triangulation(
    const std::vector<Point>& polygon
);
```

単純多角形を入力添字からなる反時計回りの三角形へ分割する。

## API別の時間計算量・空間計算量

入力頂点数を $N$ とする。

| API・操作 | 時間計算量 | 空間計算量（返り値を含む） |
| --- | --- | --- |
| `ear_clipping_triangulation(polygon)` | $O(N\log N)$ | $O(N)$ |
| triangleの構築・添字参照 | $O(1)$ | $O(1)$ |

## 注意点

自己交差、非連続位置の重複点、隣接辺の重なり、正規化後3頂点未満の入力には
`std::invalid_argument` を送出する。座標と中間演算は有限でなければならない。
