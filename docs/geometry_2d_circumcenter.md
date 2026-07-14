---
title: 2D Circumcenter (二次元外心)
documentation_of: ../src/algorithm/geometry/2d/circumcenter.hpp
---

## API

- `circumcenter(first, second, third)`: 3点を通る円の中心を返す。

## API別の時間計算量・空間計算量

- `circumcenter`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

3点が共線で一意な外心を持たない場合は `std::invalid_argument` を送出する。退化判定は2辺の角度基準で行う。
最初の頂点からの差分を最大辺長で正規化して中心offsetを計算するため、絶対座標の和や座標差の二乗を中間計算しない。
