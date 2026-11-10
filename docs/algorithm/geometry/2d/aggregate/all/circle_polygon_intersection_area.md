---
title: Circle Polygon Intersection Area (円と多角形の共通部分面積) [CGL_7_H]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/circle_polygon_intersection_area.hpp
---

円板と単純多角形の共通部分の面積を求める。多角形の向きは時計回り・反時計回りのどちらでもよい。

```cpp
long double circle_polygon_intersection_area(
    const Circle& circle,
    const std::vector<Point>& polygon
)
```

符号付き面積（多角形が反時計回りなら正）が必要な場合は次を使う。`circle_polygon_intersection_area` はこの値の絶対値を返す。

```cpp
long double signed_circle_polygon_intersection_area(
    const Circle& circle,
    const std::vector<Point>& polygon
)
```

## 計算量

$N$ を `polygon` の頂点数とする。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `signed_circle_polygon_intersection_area(circle, polygon)` | $O(N)$ | $O(1)$ |
| `circle_polygon_intersection_area(circle, polygon)` | $O(N)$ | $O(1)$ |

各辺と円の交点の候補は高々 2 個であり、辺ごとの処理は $O(1)$ である。

## 注意点

- 円の中心座標と半径は有限値でなければならない。非有限値または負の半径には `std::invalid_argument` を送出する。
- 半径が $0$、または頂点数が $2$ 未満なら $0$ を返す。
- 接触・包含の判定に使う距離の二乗や 4 次の量は、比較する各項の大きさに `GEOMETRY_EPS` を掛けた相対的な許容誤差で判定する。座標と中間値は有限な `long double` の範囲に収まる必要がある。
- 端点が一致する辺だけを退化した辺として扱う。非常に小さい正の半径や微小な多角形も、入力の大きさに応じて処理する。
