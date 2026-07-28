---
title: Safe Normalized Scalar Difference (安全な正規化スカラー差)
documentation_of: ../../../../../../src/algorithm/geometry/3d/scalar/scalar/long_double_safe_normalized_difference.hpp
---

## API

- `geometry3d_safe_normalized_difference(left, right, positive_scale)` はoverflowを避けて `(left-right)/positive_scale` を返す。

## API別の時間計算量・空間計算量

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 注意点

引数は有限、`positive_scale` は正でなければならない。違反時は `std::invalid_argument`、非零差を表現できない場合は `std::overflow_error` を送出する。
