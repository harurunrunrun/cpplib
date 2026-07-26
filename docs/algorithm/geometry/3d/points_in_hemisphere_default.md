---
title: Hemisphere Containment with Default Seed (固定seedによる半球包含判定)
documentation_of: ../../../../src/algorithm/geometry/3d/points_in_hemisphere_default.hpp
---

固定seedの制約順で、原点から各点への方向を全て含む閉半球の存在を判定する。

## API

- `points_in_hemisphere(points)`: 閉半球が存在すれば `true`。
  空集合には `true` を返し、点までの距離は結果に影響しない。

## API別の時間計算量・空間計算量

点数を $N$ として固定seedに対する最悪時間 $O(N^2)$、追加領域 $O(N)$。
既定seedに敵対しない入力では期待 $O(N)$ 時間。

## 注意点

各点は有限な非零ベクトルでなければならず、違反時は
`std::invalid_argument`。境界上の方向も許す閉半球判定である。
固定seedは `0xD1B54A32D192ED03` で、同じ入力には同じ結果を返す。
