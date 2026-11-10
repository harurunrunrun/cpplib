---
title: Stable Numeric Helpers (安定数値計算補助)
documentation_of: ../../../../src/approximate/clustering/detail/stable_numeric.hpp
---

クラスタリングの実装で、巨大な有限値を扱う距離・積・和を検査付きで計算する内部 API である。

## API

### `stable_absolute_difference(left, right)`

符号を考慮し、$\lvert \mathrm{left}-\mathrm{right} \rvert$ をオーバーフローさせずに計算する。時間計算量・追加空間計算量はともに $O(1)$ である。

### `stable_euclidean_distance(left, right)`

`std::hypot` を逐次適用して、2 点間のユークリッド距離を返す。次元を $D$ として、時間計算量は $O(D)$、追加空間計算量は $O(1)$ である。

### `checked_nonnegative_product(left, right, message)`

非負の有限値どうしの積を返す。時間計算量・追加空間計算量はともに $O(1)$ である。

### `checked_finite_sum(left, right, message)`

有限値どうしの和を返す。時間計算量・追加空間計算量はともに $O(1)$ である。

## 注意点

- `approximate::clustering::detail` の内部 API である。
- 非有限値と次元の不一致は `std::invalid_argument`、`long double` の範囲を超える結果は `std::overflow_error`（`message` を持つ）として拒否する。
