---
title: Spherical Triangle Area (球面三角形面積)
documentation_of: ../src/algorithm/geometry/3d/scalar/spherical_triangle_area.hpp
---

## API

- `spherical_triangle_area(first, second, third, radius=1)`: 各頂点を短い大円弧で結ぶ球面三角形の非負面積を返す。

## API別の時間計算量・空間計算量

時間$O(1)$、追加領域$O(1)$。

## 注意点

頂点は有限な非零ベクトル、半径は有限な非負値でなければならない。頂点の長さは無視する。反対な頂点を含み境界が一意でない場合は`std::domain_error`、面積のoverflowには`std::overflow_error`を送出する。
