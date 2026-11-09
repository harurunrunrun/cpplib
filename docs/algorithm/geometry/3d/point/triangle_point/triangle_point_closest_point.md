---
title: Triangle-Point Closest Point (三角形上の最近点)
documentation_of: ../../../../../../src/algorithm/geometry/3d/point/triangle_point/triangle_point_closest_point.hpp
---

## API

- `closest_point(triangle, point)`: 閉三角形領域上で `point` に最も近い点を返す。

## API別の時間計算量・空間計算量

- 時間 $O(1)$、追加領域 $O(1)$。

## 注意点

- 退化三角形（exact dyadic 演算による厳密判定）や非有限入力には `std::invalid_argument` を送出する。
- 座標を頂点 `a` 基準で正規化して計算する。照会点を局所座標で表現できない場合は `std::overflow_error` を送出する。
