---
title: 2D Convex Diameter (二次元凸多角形の直径) [CGL_4_B]
documentation_of: ../src/algorithm/geometry/2d/scalar/convex_diameter.hpp
---

## API

- `convex_diameter(polygon)`: 凸多角形の2頂点間距離の最大値を返す。

## API別の時間計算量・空間計算量

- 頂点数を $N$ として、時間 $O(N)$、正規化用領域 $O(N)$。

## 注意点

時計回り、連続重複、共線頂点、点・線分への退化に対応する。入力は凸でなければならない。
共線判定は辺同士の角度、rotating calipersの面積差は比較する面積と同じ二次元scaleに対する相対許容誤差を用いる。
