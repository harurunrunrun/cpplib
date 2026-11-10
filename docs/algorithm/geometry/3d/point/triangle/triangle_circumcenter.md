---
title: 3D Triangle Circumcenter (三次元三角形の外心)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/triangle/triangle_circumcenter.hpp
---

## API

- `triangle_circumcenter(triangle)`: 三角形を含む平面上にある外心を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- `circumcircle(triangle).center` を返すため、`circumcircle` と同じ exact dyadic による外心計算を用いる。
- 退化三角形や非有限入力には `std::invalid_argument`、結果を表現できない場合は `std::overflow_error` を送出する。
