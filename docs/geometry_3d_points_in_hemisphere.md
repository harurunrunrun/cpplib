---
title: Hemisphere Containment (半球包含判定)
documentation_of: ../src/algorithm/geometry/3d/points_in_hemisphere.hpp
---

## API

- `points_in_hemisphere(points)`: 全方向を含む閉半球が存在すれば`true`を返す。空集合にも`true`を返す。

## API別の時間計算量・空間計算量

点数を$N$として時間$O(N^3)$、追加領域$O(N)$。

## 注意点

各点は有限な非零ベクトルでなければならず、長さは無視する。境界の大円上にある点を許す閉半球判定であり、開半球判定とは異なる。候補支持平面を点方向および2点の外積から列挙する。
