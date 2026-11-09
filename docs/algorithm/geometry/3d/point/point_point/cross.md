---
title: 3D Cross Product (三次元外積)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/point_point/cross.hpp
---

## API

- `cross(left, right)`: 右手系の外積ベクトル `left` $\times$ `right` を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 検査は行わない。入力と中間値は有限な `long double` の範囲に収まる必要がある。
