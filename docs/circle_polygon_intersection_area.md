---
title: Circle Polygon Intersection Area (円と多角形の共通部分面積) [CGL_7_H]
documentation_of: ../src/algorithm/geometry/2d/scalar/circle_polygon_intersection_area.hpp
---

円板と単純多角形の共通部分の面積を求める。多角形は時計回り、反時計回りのどちらでもよい。

```cpp
long double circle_polygon_intersection_area(
    const Circle& circle,
    const vector<Point>& polygon
)
```

符号付き面積が必要な場合は次を使う。

```cpp
long double signed_circle_polygon_intersection_area(
    const Circle& circle,
    const vector<Point>& polygon
)
```

## 計算量の概要

$O(N)$。

## API別の時間計算量・空間計算量

$N$ をpolygon頂点数とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `signed_circle_polygon_intersection_area(circle,polygon)` | $O(N)$ | $O(1)$ |
| `circle_polygon_intersection_area(circle,polygon)` | $O(N)$ | $O(1)$ |

各辺と円の交点候補は高々2個であり、辺ごとの処理は $O(1)$。

## 注意点

座標と中間演算は有限な `long double` の範囲に収まる必要がある。境界・退化判定には各APIで明記した許容誤差を用いる。

- 円の中心座標と半径は有限値であること。非有限値または負半径にはstd::invalid_argumentを送出する。
- 接触・包含の距離二乗や4次量は、比較する各項の大きさにGEOMETRY_EPSを掛けた相対許容誤差で判定する。中間値は有限なlong doubleに収まること。
- 同一端点の辺だけを退化辺とする。tinyな正半径や微小多角形も入力scaleに応じて処理する。
