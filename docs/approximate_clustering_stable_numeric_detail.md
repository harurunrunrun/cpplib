---
title: Stable Numeric Helpers (安定数値計算補助)
documentation_of: ../src/approximate/clustering/detail/stable_numeric.hpp
---

# Stable Numeric Helpers (安定数値計算補助)

クラスタリング実装で、巨大な有限値を扱う距離・積・和を検査付きで計算する内部API。

## API

### `stable_absolute_difference(left, right)`

符号を考慮して `|left-right|` をoverflowさせずに計算する。時間・追加空間計算量は $O(1)$。

### `stable_euclidean_distance(left, right)`

`std::hypot` を逐次適用し、2点間のEuclid距離を返す。次元を $D$ として時間計算量は $O(D)$、追加空間計算量は $O(1)$。

### `checked_nonnegative_product(left, right, message)`

非負有限値の積を返す。時間・追加空間計算量は $O(1)$。

### `checked_finite_sum(left, right, message)`

有限値の和を返す。時間・追加空間計算量は $O(1)$。

## 注意点

`approximate::clustering::detail` の内部APIである。非有限値、次元不一致、または `long double` の範囲を超える結果を例外として拒否する。
