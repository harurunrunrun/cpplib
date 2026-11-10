---
title: 2D Convex Diameter (二次元凸多角形の直径) [CGL_4_B]
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/point_set/convex_diameter.hpp
---

凸多角形の頂点間距離の最大値（直径）を rotating calipers で求める。

## API

```cpp
long double convex_diameter(const std::vector<Point>& convex_polygon);
```

## 時間計算量

頂点数を $N$ として、時間計算量 $O(N)$、正規化用の追加領域 $O(N)$。

## 注意点

- 時計回りの入力、連続する重複頂点、共線な頂点、点や線分への退化に対応する。頂点が 1 個以下なら 0 を返す。
- 入力は凸でなければならない（凸性は検査しない）。
- 共線判定は辺どうしの角度で、rotating calipers での面積差の比較は比較する面積と同じ 2 次のスケールに対する相対許容誤差で行う。
