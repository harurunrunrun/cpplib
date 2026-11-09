---
title: Hemisphere Containment with Default Seed (固定シードによる半球包含判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/point_set/points_in_hemisphere_default.hpp
---

固定シードで決まる制約順を用いて、原点から各点への方向をすべて含む閉半球が存在するかを判定する。

## API

- `points_in_hemisphere(points)`: 閉半球が存在すれば `true` を返す。`points_in_hemisphere_with_seed(points, 0xD1B54A32D192ED03)` と同じである。

## API別の時間計算量・空間計算量

点数を $N$ とする。固定シードに対する最悪時間 $O(N^2)$、追加領域 $O(N)$。デフォルトのシードに対して敵対的に作られていない入力では期待時間 $O(N)$。

## 注意点

- 空集合には `true` を返す。原点から点までの距離（ベクトルの長さ）は結果に影響しない。
- 各点は有限な非零ベクトルでなければならず、違反時は `std::invalid_argument` を送出する。
- 境界上の方向も許す閉半球判定である。内積の判定には `long double` の機械イプシロンの 256 倍程度の許容誤差を用いる。
- 固定シードは `0xD1B54A32D192ED03` で、同じ入力には常に同じ結果を返す。
