---
title: 2D Circumcenter (二次元外心)
documentation_of: ../../../../../../src/algorithm/geometry/2d/point/point_point_point/circumcenter.hpp
---

## API

- `Point circumcenter(first, second, third)`: 3 点を通る円の中心を返す。

## API別の時間計算量・空間計算量

- `circumcenter`: 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

3 点が共線で外心が一意に定まらない場合は `std::invalid_argument` を送出する。退化の判定は、2 辺の単位方向ベクトルの外積（角度）を基準に行う。
最初の頂点からの差分を長い方の辺の長さで正規化して中心へのずれを計算するので、絶対座標の和や座標差の 2 乗をそのまま中間計算することはない。
