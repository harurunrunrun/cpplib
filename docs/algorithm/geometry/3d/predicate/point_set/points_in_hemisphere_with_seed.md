---
title: Hemisphere Containment with Seed (seed指定の半球包含判定)
documentation_of: ../../../../../../src/algorithm/geometry/3d/predicate/point_set/points_in_hemisphere_with_seed.hpp
---

指定seedの制約順で、原点から各点への方向を全て含む閉半球の存在を判定する。

## API

- `points_in_hemisphere_with_seed(points, random_seed)`: 閉半球が存在すれば
  `true`。seedはSeidel法へ制約を挿入する順序だけを変える。

## API別の時間計算量・空間計算量

点数を $N$ とする。seedを入力から独立に一様選択すると期待時間 $O(N)$、
最悪時間 $O(N^2)$、追加領域 $O(N)$。

## 注意点

各点は有限な非零ベクトルでなければならず、違反時は
`std::invalid_argument`。空集合には `true`。seedは判定の意味と
許容誤差を変えず、同じ入力とseedには同じ結果を返す。
